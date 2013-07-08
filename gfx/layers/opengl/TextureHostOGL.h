/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_GFX_TEXTUREOGL_H
#define MOZILLA_GFX_TEXTUREOGL_H

#include "ImageLayerOGL.h"
#include "GLContextTypes.h"
#include "gfx2DGlue.h"
#include "mozilla/layers/Effects.h"
#include "gfxReusableSurfaceWrapper.h"
#include "TiledLayerBuffer.h" // for TILEDLAYERBUFFER_TILE_SIZE

#ifdef MOZ_WIDGET_GONK
#include <ui/GraphicBuffer.h>
#endif

namespace mozilla {
namespace layers {

class TextureImageDeprecatedTextureHostOGL;
class CompositorOGL;

/*
 * TextureHost implementations for the OpenGL backend.
 *
 * Note that it is important to be careful about the ownership model with
 * the OpenGL backend, due to some widget limitation on Linux: before
 * the nsBaseWidget associated with our OpenGL context has been completely
 * deleted, every resource belonging to the OpenGL context MUST have been
 * released. At the moment the teardown sequence happens in the middle of
 * the nsBaseWidget's destructor, meaning that at a given moment we must be
 * able to easily find and release all the GL resources.
 * The point is: be careful about the ownership model and limit the number
 * of objects sharing references to GL resources to make the tear down
 * sequence as simple as possible.
 */

/**
 * TextureSourceOGL provides the necessary API for CompositorOGL to composite
 * a TextureSource.
 */
class TextureSourceOGL
{
public:
  virtual bool IsValid() const = 0;
  virtual void BindTexture(GLenum aTextureUnit) = 0;
  virtual void ReleaseTexture() = 0;
  virtual gfx::IntSize GetSize() const = 0;
  virtual gfx::SurfaceFormat GetTextureFormat() const {
    MOZ_CRASH("unhandled texture format");
  }
  // TODO: Noone's implementing this anymore, should see if we need this.
  virtual GLenum GetTextureTarget() const { return LOCAL_GL_TEXTURE_2D; }
  virtual GLenum GetWrapMode() const = 0;// { return LOCAL_GL_CLAMP_TO_EDGE; } // default
  virtual gfx3DMatrix GetTextureTransform() { return gfx3DMatrix(); }

  virtual TextureImageDeprecatedTextureHostOGL* AsTextureImageDeprecatedTextureHost() { return nullptr; }
};

inline ShaderProgramType
GetProgramTypeForTexture(const DeprecatedTextureHost *aDeprecatedTextureHost)
{
  switch (aDeprecatedTextureHost->GetFormat()) {
  case gfx::FORMAT_B8G8R8A8:
    return BGRALayerProgramType;;
  case gfx::FORMAT_B8G8R8X8:
    return BGRXLayerProgramType;;
  case gfx::FORMAT_R8G8B8X8:
    return RGBXLayerProgramType;;
  case gfx::FORMAT_R8G8B8A8:
    return RGBALayerProgramType;;
  default:
    MOZ_CRASH("unhandled program type");
  }
}

/**
 * DeprecatedTextureHost implementation using a TextureImage as the underlying texture.
 */
class TextureImageDeprecatedTextureHostOGL : public DeprecatedTextureHost
                                           , public TextureSourceOGL
                                           , public TileIterator
{
public:
  TextureImageDeprecatedTextureHostOGL(gl::TextureImage* aTexImage = nullptr)
    : mTexture(aTexImage)
    , mGL(nullptr)
    , mIterating(false)
  {
    MOZ_COUNT_CTOR(TextureImageDeprecatedTextureHostOGL);
  }

  ~TextureImageDeprecatedTextureHostOGL();

  TextureSourceOGL* AsSourceOGL() MOZ_OVERRIDE
  {
    return this;
  }

  virtual TextureImageDeprecatedTextureHostOGL* AsTextureImageDeprecatedTextureHost() MOZ_OVERRIDE
  {
    return this;
  }

  // This is a hack that is here to not break on-main-thread ThebesLayerBuffer
  // please do not use it anywhere else, use SetCompositor instead.
  void SetGLContext(gl::GLContext* aGL)
  {
    mGL = aGL;
  }

  // DeprecatedTextureHost

  void UpdateImpl(const SurfaceDescriptor& aImage,
                  nsIntRegion* aRegion = nullptr,
                  nsIntPoint* aOffset = nullptr) MOZ_OVERRIDE;

  virtual void SetCompositor(Compositor* aCompositor) MOZ_OVERRIDE;

  virtual void EnsureBuffer(const nsIntSize& aSize, gfxContentType aType) MOZ_OVERRIDE;

  virtual void CopyTo(const nsIntRect& aSourceRect,
                      DeprecatedTextureHost *aDest,
                      const nsIntRect& aDestRect) MOZ_OVERRIDE;

  bool IsValid() const MOZ_OVERRIDE
  {
    return !!mTexture;
  }

  virtual bool Lock() MOZ_OVERRIDE;

  virtual already_AddRefed<gfxImageSurface> GetAsSurface() MOZ_OVERRIDE;

  // textureSource
  void BindTexture(GLenum aTextureUnit) MOZ_OVERRIDE
  {
    mTexture->BindTexture(aTextureUnit);
  }

  void ReleaseTexture() MOZ_OVERRIDE
  {
    mTexture->ReleaseTexture();
  }

  gfx::IntSize GetSize() const MOZ_OVERRIDE;

  gfx::SurfaceFormat GetTextureFormat() const MOZ_OVERRIDE
  {
    return GetFormat();
  }

  GLenum GetWrapMode() const MOZ_OVERRIDE
  {
    return mTexture->GetWrapMode();
  }

  gl::TextureImage* GetTextureImage()
  {
    return mTexture;
  }

  void SetTextureImage(gl::TextureImage* aImage)
  {
    mTexture = aImage;
  }

  // TileIterator

  TileIterator* AsTileIterator() MOZ_OVERRIDE
  {
    return this;
  }

  void BeginTileIteration() MOZ_OVERRIDE
  {
    mTexture->BeginTileIteration();
    mIterating = true;
  }

  void EndTileIteration() MOZ_OVERRIDE
  {
    mIterating = false;
  }

  nsIntRect GetTileRect() MOZ_OVERRIDE
  {
    return mTexture->GetTileRect();
  }

  size_t GetTileCount() MOZ_OVERRIDE
  {
    return mTexture->GetTileCount();
  }

  bool NextTile() MOZ_OVERRIDE
  {
    return mTexture->NextTile();
  }

#ifdef MOZ_LAYERS_HAVE_LOG
  virtual const char* Name() { return "TextureImageDeprecatedTextureHostOGL"; }
#endif

protected:
  nsRefPtr<gl::TextureImage> mTexture;
  gl::GLContext* mGL;
  bool mIterating;
};


/**
 * DeprecatedTextureHost implementation for YCbCr images in the OpenGL backend.
 *
 * This DeprecatedTextureHost is a little bit particular in that it implements
 * the TextureSource interface, as it is required that a DeprecatedTextureHost
 * provides access to a TextureSource, but does not implement the
 * DeprecatedTextureHostOGL interface. Instead it contains 3 channels (one per
 * plane) that implement the TextureSourceOGL interface, and
 * YCbCrDeprecatedTextureHostOGL's TextureSource implementation provide access
 * to these channels with the GetSubSource method.
 */
class YCbCrDeprecatedTextureHostOGL : public DeprecatedTextureHost
{
public:
  YCbCrDeprecatedTextureHostOGL()
    : mGL(nullptr)
  {
    MOZ_COUNT_CTOR(YCbCrDeprecatedTextureHostOGL);
    mYTexture  = new Channel;
    mCbTexture = new Channel;
    mCrTexture = new Channel;
    mFormat = gfx::FORMAT_YUV;
  }

  ~YCbCrDeprecatedTextureHostOGL()
  {
    MOZ_COUNT_DTOR(YCbCrDeprecatedTextureHostOGL);
  }

  virtual void SetCompositor(Compositor* aCompositor) MOZ_OVERRIDE;

  virtual void UpdateImpl(const SurfaceDescriptor& aImage,
                          nsIntRegion* aRegion = nullptr,
                          nsIntPoint* aOffset = nullptr) MOZ_OVERRIDE;

  virtual bool Lock() MOZ_OVERRIDE;

  virtual bool IsValid() const MOZ_OVERRIDE
  {
    return mYTexture->IsValid()
        && mCbTexture->IsValid()
        && mCrTexture->IsValid();
  }

  struct Channel : public TextureSourceOGL
                 , public TextureSource
  {
    TextureSourceOGL* AsSourceOGL() MOZ_OVERRIDE
    {
      return this;
    }
    nsRefPtr<gl::TextureImage> mTexImage;

    void BindTexture(GLenum aUnit) MOZ_OVERRIDE
    {
      mTexImage->BindTexture(aUnit);
    }
    void ReleaseTexture() MOZ_OVERRIDE
    {
      mTexImage->ReleaseTexture();
    }
    virtual bool IsValid() const MOZ_OVERRIDE
    {
      return !!mTexImage;
    }
    virtual gfx::IntSize GetSize() const MOZ_OVERRIDE
    {
      return gfx::IntSize(mTexImage->GetSize().width, mTexImage->GetSize().height);
    }
    virtual GLenum GetWrapMode() const MOZ_OVERRIDE
    {
      return mTexImage->GetWrapMode();
    }
  };

  // TextureSource implementation

  TextureSource* GetSubSource(int index) MOZ_OVERRIDE
  {
    switch (index) {
      case 0 : return mYTexture.get();
      case 1 : return mCbTexture.get();
      case 2 : return mCrTexture.get();
    }
    return nullptr;
  }

  gfx::IntSize GetSize() const MOZ_OVERRIDE
  {
    if (!mYTexture->mTexImage) {
      NS_WARNING("YCbCrDeprecatedTextureHost::GetSize called but no data has been set yet");
      return gfx::IntSize(0,0);
    }
    return mYTexture->GetSize();
  }

  virtual already_AddRefed<gfxImageSurface> GetAsSurface() MOZ_OVERRIDE;

#ifdef MOZ_LAYERS_HAVE_LOG
  virtual const char* Name() { return "YCbCrDeprecatedTextureHostOGL"; }
#endif

private:
  RefPtr<Channel> mYTexture;
  RefPtr<Channel> mCbTexture;
  RefPtr<Channel> mCrTexture;
  gl::GLContext* mGL;
};

class SharedDeprecatedTextureHostOGL : public DeprecatedTextureHost
                           , public TextureSourceOGL
{
public:
  typedef gfxASurface::gfxContentType ContentType;
  typedef mozilla::gl::GLContext GLContext;
  typedef mozilla::gl::TextureImage TextureImage;

  SharedDeprecatedTextureHostOGL()
    : mGL(nullptr)
    , mTextureHandle(0)
    , mWrapMode(LOCAL_GL_CLAMP_TO_EDGE)
    , mSharedHandle(0)
    , mShareType(GLContext::SameProcess)
  {}

  virtual void SetCompositor(Compositor* aCompositor) MOZ_OVERRIDE;

  virtual ~SharedDeprecatedTextureHostOGL()
  {
    if (mSharedHandle || mTextureHandle) {
      DeleteTextures();
    }
  }

  virtual GLuint GetTextureHandle()
  {
    return mTextureHandle;
  }

  virtual TextureSourceOGL* AsSourceOGL() MOZ_OVERRIDE { return this; }

  bool IsValid() const MOZ_OVERRIDE { return !!mSharedHandle; }

  // override from DeprecatedTextureHost, we support both buffered
  // and unbuffered operation.
  virtual void UpdateImpl(const SurfaceDescriptor& aImage,
                          nsIntRegion* aRegion = nullptr,
                          nsIntPoint* aOffset = nullptr) MOZ_OVERRIDE;
  virtual void SwapTexturesImpl(const SurfaceDescriptor& aImage,
                                nsIntRegion* aRegion = nullptr) MOZ_OVERRIDE;
  virtual bool Lock() MOZ_OVERRIDE;
  virtual void Unlock() MOZ_OVERRIDE;

  virtual GLenum GetWrapMode() const MOZ_OVERRIDE { return mWrapMode; }
  virtual void SetWrapMode(GLenum aMode) { mWrapMode = aMode; }

  gfx::SurfaceFormat GetTextureFormat() const MOZ_OVERRIDE
  {
    return mFormat;
  }

  virtual GLenum GetTextureTarget() const MOZ_OVERRIDE
  {
    return mTextureTarget;
  }

  gfx::IntSize GetSize() const MOZ_OVERRIDE {
    return mSize;
  }

  void BindTexture(GLenum activetex) MOZ_OVERRIDE
  {
    MOZ_ASSERT(mGL);
    // Lock already bound us!
    MOZ_ASSERT(activetex == LOCAL_GL_TEXTURE0);
  }
  void ReleaseTexture() MOZ_OVERRIDE {}
  GLuint GetTextureID() { return mTextureHandle; }
  ContentType GetContentType()
  {
    return (mFormat == gfx::FORMAT_B8G8R8A8) ?
             gfxASurface::CONTENT_COLOR_ALPHA :
             gfxASurface::CONTENT_COLOR;
  }

  virtual gfx3DMatrix GetTextureTransform() MOZ_OVERRIDE;

  virtual already_AddRefed<gfxImageSurface> GetAsSurface() MOZ_OVERRIDE;

#ifdef MOZ_LAYERS_HAVE_LOG
  virtual const char* Name() { return "SharedDeprecatedTextureHostOGL"; }
#endif

protected:
  void DeleteTextures();

  gfx::IntSize mSize;
  nsRefPtr<gl::GLContext> mGL;
  GLuint mTextureHandle;
  GLenum mWrapMode;
  GLenum mTextureTarget;
  gl::SharedTextureHandle mSharedHandle;
  gl::GLContext::SharedTextureShareType mShareType;
};

class SurfaceStreamHostOGL : public DeprecatedTextureHost
                           , public TextureSourceOGL
{
public:
  typedef gfxASurface::gfxContentType ContentType;
  typedef mozilla::gl::GLContext GLContext;
  typedef mozilla::gl::TextureImage TextureImage;

  virtual ~SurfaceStreamHostOGL()
  {
    DeleteTextures();
    *mBuffer = SurfaceDescriptor();
  }

  virtual void SetCompositor(Compositor* aCompositor) MOZ_OVERRIDE;

  virtual GLuint GetTextureHandle()
  {
    return mTextureHandle;
  }

  virtual TextureSourceOGL* AsSourceOGL() MOZ_OVERRIDE { return this; }

  bool IsValid() const MOZ_OVERRIDE { return true; }

  // override from DeprecatedTextureHost
  virtual void SwapTexturesImpl(const SurfaceDescriptor& aImage,
                                nsIntRegion* aRegion = nullptr) MOZ_OVERRIDE;
  virtual bool Lock() MOZ_OVERRIDE;
  virtual void Unlock() MOZ_OVERRIDE;

  virtual GLenum GetWrapMode() const MOZ_OVERRIDE {
    return mWrapMode;
  }
  virtual void SetWrapMode(GLenum aMode) {
    mWrapMode = aMode;
  }

  gfx::SurfaceFormat GetTextureFormat() const MOZ_OVERRIDE
  {
    return mFormat;
  }

  gfx::IntSize GetSize() const MOZ_OVERRIDE {
    return mSize;
  }

  virtual GLenum GetTextureTarget() const MOZ_OVERRIDE
  {
    return LOCAL_GL_TEXTURE_2D;
  }

  void BindTexture(GLenum activetex) MOZ_OVERRIDE {
    MOZ_ASSERT(mGL);
    mGL->fActiveTexture(activetex);
    mGL->fBindTexture(LOCAL_GL_TEXTURE_2D, mTextureHandle);
  }
  void ReleaseTexture() MOZ_OVERRIDE {
  }
  GLuint GetTextureID() { return mTextureHandle; }
  ContentType GetContentType() {
    return (mFormat == gfx::FORMAT_B8G8R8A8) ?
             gfxASurface::CONTENT_COLOR_ALPHA :
             gfxASurface::CONTENT_COLOR;
  }

  virtual already_AddRefed<gfxImageSurface> GetAsSurface() MOZ_OVERRIDE;

#ifdef MOZ_LAYERS_HAVE_LOG
  virtual const char* Name() { return "SurfaceStreamHostOGL"; }
#endif

  SurfaceStreamHostOGL()
    : mGL(nullptr)
    , mTextureHandle(0)
    , mUploadTexture(0)
    , mWrapMode(LOCAL_GL_CLAMP_TO_EDGE)
  {}

protected:
  void DeleteTextures();

  gfx::IntSize mSize;
  nsRefPtr<GLContext> mGL;
  GLuint mTextureHandle;
  GLuint mUploadTexture;
  GLenum mWrapMode;
};

class TiledDeprecatedTextureHostOGL : public DeprecatedTextureHost
                          , public TextureSourceOGL
{
public:
  TiledDeprecatedTextureHostOGL()
    : mTextureHandle(0)
    , mGL(nullptr)
  {}
  ~TiledDeprecatedTextureHostOGL();

  virtual void SetCompositor(Compositor* aCompositor);

  // have to pass the size in here (every time) because of DrawQuad API :-(
  virtual void Update(gfxReusableSurfaceWrapper* aReusableSurface, TextureFlags aFlags, const gfx::IntSize& aSize) MOZ_OVERRIDE;
  virtual bool Lock() MOZ_OVERRIDE;
  virtual void Unlock() MOZ_OVERRIDE {}

  virtual TextureSourceOGL* AsSourceOGL() MOZ_OVERRIDE { return this; }
  virtual bool IsValid() const MOZ_OVERRIDE { return true; }
  virtual GLenum GetWrapMode() const MOZ_OVERRIDE { return LOCAL_GL_CLAMP_TO_EDGE; }
  virtual void BindTexture(GLenum aTextureUnit) MOZ_OVERRIDE
  {
    mGL->fActiveTexture(aTextureUnit);
    mGL->fBindTexture(LOCAL_GL_TEXTURE_2D, mTextureHandle);
  }
  virtual void ReleaseTexture() MOZ_OVERRIDE {}
  virtual gfx::IntSize GetSize() const MOZ_OVERRIDE
  {
    return mSize;
  }

  gfx::SurfaceFormat GetTextureFormat() const MOZ_OVERRIDE
  {
    return GetFormat();
  }

  virtual void SwapTexturesImpl(const SurfaceDescriptor& aImage,
                                nsIntRegion* aRegion = nullptr)
  { MOZ_ASSERT(false, "Tiles should not use this path"); }

  virtual already_AddRefed<gfxImageSurface> GetAsSurface() MOZ_OVERRIDE;

#ifdef MOZ_LAYERS_HAVE_LOG
  virtual const char* Name() { return "TiledDeprecatedTextureHostOGL"; }
#endif

protected:
  void DeleteTextures();

  virtual uint64_t GetIdentifier() const MOZ_OVERRIDE {
    return static_cast<uint64_t>(mTextureHandle);
  }

private:
  GLenum GetTileType()
  {
    // Deduce the type that was assigned in GetFormatAndTileForImageFormat
    return mGLFormat == LOCAL_GL_RGB ? LOCAL_GL_UNSIGNED_SHORT_5_6_5 : LOCAL_GL_UNSIGNED_BYTE;
  }

  gfx::IntSize mSize;
  GLuint mTextureHandle;
  GLenum mGLFormat;
  nsRefPtr<gl::GLContext> mGL;
};

#ifdef MOZ_WIDGET_GONK

// For direct texturing with gralloc buffers. The corresponding DeprecatedTextureClient is DeprecatedTextureClientShmem,
// which automatically gets gralloc when it can, in which case the compositor sees that the
// SurfaceDescriptor is gralloc, and decides to use a GrallocDeprecatedTextureHostOGL to do direct texturing,
// saving the cost of a texture upload.
class GrallocDeprecatedTextureHostOGL
  : public DeprecatedTextureHost
  , public TextureSourceOGL
{
public:
  GrallocDeprecatedTextureHostOGL();

  ~GrallocDeprecatedTextureHostOGL();

  virtual void SetCompositor(Compositor* aCompositor) MOZ_OVERRIDE;

  virtual void UpdateImpl(const SurfaceDescriptor& aImage,
                          nsIntRegion* aRegion = nullptr,
                          nsIntPoint* aOffset = nullptr) MOZ_OVERRIDE;
  virtual void SwapTexturesImpl(const SurfaceDescriptor& aImage,
                          nsIntRegion* aRegion = nullptr) MOZ_OVERRIDE;
  virtual bool Lock() MOZ_OVERRIDE;
  virtual void Unlock() MOZ_OVERRIDE;

  virtual gfx::IntSize GetSize() const MOZ_OVERRIDE
  {
    return mGraphicBuffer.get() ? gfx::IntSize(mGraphicBuffer->getWidth(), mGraphicBuffer->getHeight()) : gfx::IntSize(0, 0);
  }

  gfx::SurfaceFormat GetTextureFormat() const MOZ_OVERRIDE
  {
    if (mTextureTarget == LOCAL_GL_TEXTURE_EXTERNAL) {
      return gfx::FORMAT_R8G8B8A8;
    }
    MOZ_ASSERT(mTextureTarget == LOCAL_GL_TEXTURE_2D);
    return mFormat;
  }

  GLenum GetWrapMode() const MOZ_OVERRIDE
  {
    return LOCAL_GL_CLAMP_TO_EDGE;
  }

  bool IsValid() const MOZ_OVERRIDE;

  virtual already_AddRefed<gfxImageSurface> GetAsSurface() MOZ_OVERRIDE;

#ifdef MOZ_LAYERS_HAVE_LOG
  virtual const char* Name() { return "GrallocDeprecatedTextureHostOGL"; }
#endif

  void BindTexture(GLenum aTextureUnit) MOZ_OVERRIDE;
  void ReleaseTexture() MOZ_OVERRIDE {}

  virtual gfx::SurfaceFormat GetFormat() const;

  virtual TextureSourceOGL* AsSourceOGL() MOZ_OVERRIDE
  {
    return this;
  }

  // only overridden for hacky fix in gecko 23 for bug 862324
  // see bug 865908 about fixing this.
  virtual void SetBuffer(SurfaceDescriptor* aBuffer, ISurfaceAllocator* aAllocator) MOZ_OVERRIDE;

  // used only for hacky fix in gecko 23 for bug 862324
  virtual void ForgetBuffer()
  {
    if (mBuffer) {
      // Intentionally don't destroy the actor held by mBuffer here.
      // The point is that this is only called from GrallocBufferActor::ActorDestroy
      // where we know that the actor is already being deleted.
      // See bug 862324 comment 39.
      delete mBuffer;
      mBuffer = nullptr;
    }

    mGraphicBuffer = nullptr;
    DeleteTextures();
  }

  virtual LayerRenderState GetRenderState() MOZ_OVERRIDE;

private:
  gl::GLContext* gl() const;

  void DeleteTextures();

  RefPtr<CompositorOGL> mCompositor;
  android::sp<android::GraphicBuffer> mGraphicBuffer;
  GLenum mTextureTarget;
  EGLImage mEGLImage;
  //Set when the composer needs to swap RB pixels of gralloc buffer
  bool mIsRBSwapped;
};
#endif

} // namespace
} // namespace

#endif /* MOZILLA_GFX_TEXTUREOGL_H */
