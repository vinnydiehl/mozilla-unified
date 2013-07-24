/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MediaStreamAudioSourceNode_h_
#define MediaStreamAudioSourceNode_h_

#include "AudioNode.h"

namespace mozilla {
namespace dom {

class MediaStreamAudioSourceNode : public AudioNode
{
public:
  MediaStreamAudioSourceNode(AudioContext* aContext, const DOMMediaStream* aMediaStream);

  virtual JSObject* WrapObject(JSContext* aCx, JS::Handle<JSObject*> aScope) MOZ_OVERRIDE;

  virtual void DestroyMediaStream() MOZ_OVERRIDE;

  virtual uint16_t NumberOfInputs() const MOZ_OVERRIDE { return 0; }

private:
  nsRefPtr<MediaInputPort> mInputPort;
};

}
}

#endif

