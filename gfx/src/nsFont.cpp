/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsFont.h"
#include "gfxFont.h"                    // for gfxFontStyle
#include "gfxFontConstants.h"           // for NS_FONT_KERNING_AUTO, etc
#include "gfxFontFeatures.h"            // for gfxFontFeature, etc
#include "gfxFontUtils.h"               // for TRUETYPE_TAG
#include "nsCRT.h"                      // for nsCRT
#include "nsDebug.h"                    // for NS_ASSERTION
#include "nsISupports.h"
#include "nsUnicharUtils.h"
#include "nscore.h"                     // for char16_t
#include "mozilla/ArrayUtils.h"
#include "mozilla/gfx/2D.h"

using namespace mozilla;

nsFont::nsFont(const FontFamilyList& aFontlist, uint8_t aStyle,
               uint16_t aWeight, int16_t aStretch,
               uint8_t aDecoration, nscoord aSize)
  : fontlist(aFontlist)
{
  Init();
  style = aStyle;
  weight = aWeight;
  stretch = aStretch;
  decorations = aDecoration;
  size = aSize;
}

nsFont::nsFont(FontFamilyType aGenericType, uint8_t aStyle,
               uint16_t aWeight, int16_t aStretch, uint8_t aDecoration,
               nscoord aSize)
  : fontlist(aGenericType)
{
  Init();
  style = aStyle;
  weight = aWeight;
  stretch = aStretch;
  decorations = aDecoration;
  size = aSize;
}

void
nsFont::Init()
{
  systemFont = false;
  smoothing = NS_FONT_SMOOTHING_AUTO;
  sizeAdjust = 0.0;
  kerning = NS_FONT_KERNING_AUTO;
  synthesis = NS_FONT_SYNTHESIS_WEIGHT | NS_FONT_SYNTHESIS_STYLE;

  variantAlternates = 0;
  variantCaps = NS_FONT_VARIANT_CAPS_NORMAL;
  variantEastAsian = 0;
  variantLigatures = 0;
  variantNumeric = 0;
  variantPosition = NS_FONT_VARIANT_POSITION_NORMAL;
}

nsFont::nsFont(const nsFont& aOther)
  : fontlist(aOther.fontlist)
{
  style = aOther.style;
  systemFont = aOther.systemFont;
  weight = aOther.weight;
  stretch = aOther.stretch;
  decorations = aOther.decorations;
  smoothing = aOther.smoothing;
  size = aOther.size;
  sizeAdjust = aOther.sizeAdjust;
  kerning = aOther.kerning;
  synthesis = aOther.synthesis;
  fontFeatureSettings = aOther.fontFeatureSettings;
  languageOverride = aOther.languageOverride;
  variantAlternates = aOther.variantAlternates;
  variantCaps = aOther.variantCaps;
  variantEastAsian = aOther.variantEastAsian;
  variantLigatures = aOther.variantLigatures;
  variantNumeric = aOther.variantNumeric;
  variantPosition = aOther.variantPosition;
  alternateValues = aOther.alternateValues;
  featureValueLookup = aOther.featureValueLookup;
}

nsFont::nsFont()
{
}

nsFont::~nsFont()
{
}

bool nsFont::BaseEquals(const nsFont& aOther) const
{
  if ((style == aOther.style) &&
      (systemFont == aOther.systemFont) &&
      (weight == aOther.weight) &&
      (stretch == aOther.stretch) &&
      (size == aOther.size) &&
      (sizeAdjust == aOther.sizeAdjust) &&
      (fontlist == aOther.fontlist) &&
      (kerning == aOther.kerning) &&
      (synthesis == aOther.synthesis) &&
      (fontFeatureSettings == aOther.fontFeatureSettings) &&
      (languageOverride == aOther.languageOverride) &&
      (variantAlternates == aOther.variantAlternates) &&
      (variantCaps == aOther.variantCaps) &&
      (variantEastAsian == aOther.variantEastAsian) &&
      (variantLigatures == aOther.variantLigatures) &&
      (variantNumeric == aOther.variantNumeric) &&
      (variantPosition == aOther.variantPosition) &&
      (alternateValues == aOther.alternateValues) &&
      (featureValueLookup == aOther.featureValueLookup) &&
      (smoothing == aOther.smoothing)) {
    return true;
  }
  return false;
}

bool nsFont::Equals(const nsFont& aOther) const
{
  if (BaseEquals(aOther) &&
      (decorations == aOther.decorations)) {
    return true;
  }
  return false;
}

nsFont& nsFont::operator=(const nsFont& aOther)
{
  fontlist = aOther.fontlist;
  style = aOther.style;
  systemFont = aOther.systemFont;
  weight = aOther.weight;
  stretch = aOther.stretch;
  decorations = aOther.decorations;
  smoothing = aOther.smoothing;
  size = aOther.size;
  sizeAdjust = aOther.sizeAdjust;
  kerning = aOther.kerning;
  synthesis = aOther.synthesis;
  fontFeatureSettings = aOther.fontFeatureSettings;
  languageOverride = aOther.languageOverride;
  variantAlternates = aOther.variantAlternates;
  variantCaps = aOther.variantCaps;
  variantEastAsian = aOther.variantEastAsian;
  variantLigatures = aOther.variantLigatures;
  variantNumeric = aOther.variantNumeric;
  variantPosition = aOther.variantPosition;
  alternateValues = aOther.alternateValues;
  featureValueLookup = aOther.featureValueLookup;
  return *this;
}

void
nsFont::CopyAlternates(const nsFont& aOther)
{
  variantAlternates = aOther.variantAlternates;
  alternateValues = aOther.alternateValues;
  featureValueLookup = aOther.featureValueLookup;
}

// mapping from bitflag to font feature tag/value pair
//
// these need to be kept in sync with the constants listed
// in gfxFontConstants.h (e.g. NS_FONT_VARIANT_EAST_ASIAN_JIS78)

// NS_FONT_VARIANT_EAST_ASIAN_xxx values
const gfxFontFeature eastAsianDefaults[] = {
  { TRUETYPE_TAG('j','p','7','8'), 1 },
  { TRUETYPE_TAG('j','p','8','3'), 1 },
  { TRUETYPE_TAG('j','p','9','0'), 1 },
  { TRUETYPE_TAG('j','p','0','4'), 1 },
  { TRUETYPE_TAG('s','m','p','l'), 1 },
  { TRUETYPE_TAG('t','r','a','d'), 1 },
  { TRUETYPE_TAG('f','w','i','d'), 1 },
  { TRUETYPE_TAG('p','w','i','d'), 1 },
  { TRUETYPE_TAG('r','u','b','y'), 1 }
};

static_assert(MOZ_ARRAY_LENGTH(eastAsianDefaults) ==
              eFeatureEastAsian_numFeatures,
              "eFeatureEastAsian_numFeatures should be correct");

// NS_FONT_VARIANT_LIGATURES_xxx values
const gfxFontFeature ligDefaults[] = {
  { TRUETYPE_TAG('l','i','g','a'), 0 },  // none value means all off
  { TRUETYPE_TAG('l','i','g','a'), 1 },
  { TRUETYPE_TAG('l','i','g','a'), 0 },
  { TRUETYPE_TAG('d','l','i','g'), 1 },
  { TRUETYPE_TAG('d','l','i','g'), 0 },
  { TRUETYPE_TAG('h','l','i','g'), 1 },
  { TRUETYPE_TAG('h','l','i','g'), 0 },
  { TRUETYPE_TAG('c','a','l','t'), 1 },
  { TRUETYPE_TAG('c','a','l','t'), 0 }
};

static_assert(MOZ_ARRAY_LENGTH(ligDefaults) ==
              eFeatureLigatures_numFeatures,
              "eFeatureLigatures_numFeatures should be correct");

// NS_FONT_VARIANT_NUMERIC_xxx values
const gfxFontFeature numericDefaults[] = {
  { TRUETYPE_TAG('l','n','u','m'), 1 },
  { TRUETYPE_TAG('o','n','u','m'), 1 },
  { TRUETYPE_TAG('p','n','u','m'), 1 },
  { TRUETYPE_TAG('t','n','u','m'), 1 },
  { TRUETYPE_TAG('f','r','a','c'), 1 },
  { TRUETYPE_TAG('a','f','r','c'), 1 },
  { TRUETYPE_TAG('z','e','r','o'), 1 },
  { TRUETYPE_TAG('o','r','d','n'), 1 }
};

static_assert(MOZ_ARRAY_LENGTH(numericDefaults) ==
              eFeatureNumeric_numFeatures,
              "eFeatureNumeric_numFeatures should be correct");

static void
AddFontFeaturesBitmask(uint32_t aValue, uint32_t aMin, uint32_t aMax,
                      const gfxFontFeature aFeatureDefaults[],
                      nsTArray<gfxFontFeature>& aFeaturesOut)

{
  uint32_t i, m;

  for (i = 0, m = aMin; m <= aMax; i++, m <<= 1) {
    if (m & aValue) {
      const gfxFontFeature& feature = aFeatureDefaults[i];
      aFeaturesOut.AppendElement(feature);
    }
  }
}

void nsFont::AddFontFeaturesToStyle(gfxFontStyle *aStyle) const
{
  // add in font-variant features
  gfxFontFeature setting;

  // -- kerning
  setting.mTag = TRUETYPE_TAG('k','e','r','n');
  switch (kerning) {
    case NS_FONT_KERNING_NONE:
      setting.mValue = 0;
      aStyle->featureSettings.AppendElement(setting);
      break;
    case NS_FONT_KERNING_NORMAL:
      setting.mValue = 1;
      aStyle->featureSettings.AppendElement(setting);
      break;
    default:
      // auto case implies use user agent default
      break;
  }

  // -- alternates
  if (variantAlternates & NS_FONT_VARIANT_ALTERNATES_HISTORICAL) {
    setting.mValue = 1;
    setting.mTag = TRUETYPE_TAG('h','i','s','t');
    aStyle->featureSettings.AppendElement(setting);
  }

  // -- copy font-specific alternate info into style
  //    (this will be resolved after font-matching occurs)
  aStyle->alternateValues.AppendElements(alternateValues);
  aStyle->featureValueLookup = featureValueLookup;

  // -- caps
  aStyle->variantCaps = variantCaps;

  // -- east-asian
  if (variantEastAsian) {
    AddFontFeaturesBitmask(variantEastAsian,
                           NS_FONT_VARIANT_EAST_ASIAN_JIS78,
                           NS_FONT_VARIANT_EAST_ASIAN_RUBY,
                           eastAsianDefaults, aStyle->featureSettings);
  }

  // -- ligatures
  if (variantLigatures) {
    AddFontFeaturesBitmask(variantLigatures,
                           NS_FONT_VARIANT_LIGATURES_NONE,
                           NS_FONT_VARIANT_LIGATURES_NO_CONTEXTUAL,
                           ligDefaults, aStyle->featureSettings);

    if (variantLigatures & NS_FONT_VARIANT_LIGATURES_COMMON) {
      // liga already enabled, need to enable clig also
      setting.mTag = TRUETYPE_TAG('c','l','i','g');
      setting.mValue = 1;
      aStyle->featureSettings.AppendElement(setting);
    } else if (variantLigatures & NS_FONT_VARIANT_LIGATURES_NO_COMMON) {
      // liga already disabled, need to disable clig also
      setting.mTag = TRUETYPE_TAG('c','l','i','g');
      setting.mValue = 0;
      aStyle->featureSettings.AppendElement(setting);
    } else if (variantLigatures & NS_FONT_VARIANT_LIGATURES_NONE) {
      // liga already disabled, need to disable dlig, hlig, calt, clig
      setting.mValue = 0;
      setting.mTag = TRUETYPE_TAG('d','l','i','g');
      aStyle->featureSettings.AppendElement(setting);
      setting.mTag = TRUETYPE_TAG('h','l','i','g');
      aStyle->featureSettings.AppendElement(setting);
      setting.mTag = TRUETYPE_TAG('c','a','l','t');
      aStyle->featureSettings.AppendElement(setting);
      setting.mTag = TRUETYPE_TAG('c','l','i','g');
      aStyle->featureSettings.AppendElement(setting);
    }
  }

  // -- numeric
  if (variantNumeric) {
    AddFontFeaturesBitmask(variantNumeric,
                           NS_FONT_VARIANT_NUMERIC_LINING,
                           NS_FONT_VARIANT_NUMERIC_ORDINAL,
                           numericDefaults, aStyle->featureSettings);
  }

  // -- position
  aStyle->variantSubSuper = variantPosition;

  // indicate common-path case when neither variantCaps or variantSubSuper are set
  aStyle->noFallbackVariantFeatures =
    (aStyle->variantCaps == NS_FONT_VARIANT_CAPS_NORMAL) &&
    (variantPosition == NS_FONT_VARIANT_POSITION_NORMAL);

  // add in features from font-feature-settings
  aStyle->featureSettings.AppendElements(fontFeatureSettings);

  // enable grayscale antialiasing for text
  if (smoothing == NS_FONT_SMOOTHING_GRAYSCALE) {
    aStyle->useGrayscaleAntialiasing = true;
  }
}
