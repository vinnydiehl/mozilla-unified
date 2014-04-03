/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 *
 * Copyright (C) 2009, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Peter Varga (pvarga@inf.u-szeged.hu), University of Szeged
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef yarr_YarrPattern_h
#define yarr_YarrPattern_h

#include "yarr/wtfbridge.h"
#include "yarr/ASCIICType.h"

namespace JSC { namespace Yarr {

struct PatternDisjunction;

enum ErrorCode {
    NoError,
    PatternTooLarge,
    QuantifierOutOfOrder,
    QuantifierWithoutAtom,
    QuantifierTooLarge,
    MissingParentheses,
    ParenthesesUnmatched,
    ParenthesesTypeInvalid,
    CharacterClassUnmatched,
    CharacterClassOutOfOrder,
    CharacterClassInvalidRange,
    EscapeUnterminated,
    RuntimeError,
    NumberOfErrorCodes
};

static inline const char* errorMessage(ErrorCode code)
{

#define REGEXP_ERROR_PREFIX "Invalid regular expression: "
   // The order of this array must match the ErrorCode enum.
   static const char* errorMessages[NumberOfErrorCodes] = {
       0, // NoError
       REGEXP_ERROR_PREFIX "regular expression too large",
       REGEXP_ERROR_PREFIX "numbers out of order in {} quantifier",
       REGEXP_ERROR_PREFIX "nothing to repeat",
       REGEXP_ERROR_PREFIX "number too large in {} quantifier",
       REGEXP_ERROR_PREFIX "missing )",
       REGEXP_ERROR_PREFIX "unmatched parentheses",
       REGEXP_ERROR_PREFIX "unrecognized character after (?",
       REGEXP_ERROR_PREFIX "missing terminating ] for character class",
       REGEXP_ERROR_PREFIX "character class out of order"
       REGEXP_ERROR_PREFIX "range out of order in character class",
       REGEXP_ERROR_PREFIX "\\ at end of pattern"
   };
#undef REGEXP_ERROR_PREFIX

   return errorMessages[code];
}

struct CharacterRange {
    UChar begin;
    UChar end;

    CharacterRange(UChar begin, UChar end)
        : begin(begin)
        , end(end)
    {
    }
};

struct CharacterClass {
    WTF_MAKE_FAST_ALLOCATED;
public:
    // All CharacterClass instances have to have the full set of matches and ranges,
    // they may have an optional m_table for faster lookups (which must match the
    // specified matches and ranges)
    CharacterClass()
        : m_table(0)
    {
    }
    CharacterClass(const char* table, bool inverted)
        : m_table(table)
        , m_tableInverted(inverted)
    {
    }
    Vector<UChar> m_matches;
    Vector<CharacterRange> m_ranges;
    Vector<UChar> m_matchesUnicode;
    Vector<CharacterRange> m_rangesUnicode;

    const char* m_table;
    bool m_tableInverted;
};

enum QuantifierType {
    QuantifierFixedCount,
    QuantifierGreedy,
    QuantifierNonGreedy
};

struct PatternTerm {
    enum Type {
        TypeAssertionBOL,
        TypeAssertionEOL,
        TypeAssertionWordBoundary,
        TypePatternCharacter,
        TypeCharacterClass,
        TypeBackReference,
        TypeForwardReference,
        TypeParenthesesSubpattern,
        TypeParentheticalAssertion,
        TypeDotStarEnclosure
    } type;
    bool m_capture :1;
    bool m_invert :1;
    union {
        UChar patternCharacter;
        CharacterClass* characterClass;
        unsigned backReferenceSubpatternId;
        struct {
            PatternDisjunction* disjunction;
            unsigned subpatternId;
            unsigned lastSubpatternId;
            bool isCopy;
            bool isTerminal;
        } parentheses;
        struct {
            bool bolAnchor : 1;
            bool eolAnchor : 1;
        } anchors;
    };
    QuantifierType quantityType;
    Checked<unsigned> quantityCount;
    int inputPosition;
    unsigned frameLocation;

    PatternTerm(UChar ch)
        : type(PatternTerm::TypePatternCharacter)
        , m_capture(false)
        , m_invert(false)
    {
        patternCharacter = ch;
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    PatternTerm(CharacterClass* charClass, bool invert)
        : type(PatternTerm::TypeCharacterClass)
        , m_capture(false)
        , m_invert(invert)
    {
        characterClass = charClass;
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    PatternTerm(Type type, unsigned subpatternId, PatternDisjunction* disjunction, bool capture = false, bool invert = false)
        : type(type)
        , m_capture(capture)
        , m_invert(invert)
    {
        parentheses.disjunction = disjunction;
        parentheses.subpatternId = subpatternId;
        parentheses.isCopy = false;
        parentheses.isTerminal = false;
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    PatternTerm(Type type, bool invert = false)
        : type(type)
        , m_capture(false)
        , m_invert(invert)
    {
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    PatternTerm(unsigned spatternId)
        : type(TypeBackReference)
        , m_capture(false)
        , m_invert(false)
    {
        backReferenceSubpatternId = spatternId;
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    PatternTerm(bool bolAnchor, bool eolAnchor)
        : type(TypeDotStarEnclosure)
        , m_capture(false)
        , m_invert(false)
    {
        anchors.bolAnchor = bolAnchor;
        anchors.eolAnchor = eolAnchor;
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    // No-argument constructor for js::Vector.
    PatternTerm()
        : type(PatternTerm::TypePatternCharacter)
        , m_capture(false)
        , m_invert(false)
    {
        patternCharacter = 0;
        quantityType = QuantifierFixedCount;
        quantityCount = 1;
    }

    static PatternTerm ForwardReference()
    {
        return PatternTerm(TypeForwardReference);
    }

    static PatternTerm BOL()
    {
        return PatternTerm(TypeAssertionBOL);
    }

    static PatternTerm EOL()
    {
        return PatternTerm(TypeAssertionEOL);
    }

    static PatternTerm WordBoundary(bool invert)
    {
        return PatternTerm(TypeAssertionWordBoundary, invert);
    }

    bool invert()
    {
        return m_invert;
    }

    bool capture()
    {
        return m_capture;
    }

    void quantify(unsigned count, QuantifierType type)
    {
        quantityCount = count;
        quantityType = type;
    }
};

struct PatternAlternative {
    WTF_MAKE_FAST_ALLOCATED;
public:
    PatternAlternative(PatternDisjunction* disjunction)
        : m_parent(disjunction)
        , m_onceThrough(false)
        , m_hasFixedSize(false)
        , m_startsWithBOL(false)
        , m_containsBOL(false)
    {
    }

    PatternTerm& lastTerm()
    {
        ASSERT(m_terms.size());
        return m_terms[m_terms.size() - 1];
    }

    void removeLastTerm()
    {
        ASSERT(m_terms.size());
        m_terms.shrink(m_terms.size() - 1);
    }

    void setOnceThrough()
    {
        m_onceThrough = true;
    }

    bool onceThrough()
    {
        return m_onceThrough;
    }

    Vector<PatternTerm> m_terms;
    PatternDisjunction* m_parent;
    unsigned m_minimumSize;
    bool m_onceThrough : 1;
    bool m_hasFixedSize : 1;
    bool m_startsWithBOL : 1;
    bool m_containsBOL : 1;
};

struct PatternDisjunction {
    WTF_MAKE_FAST_ALLOCATED;
public:
    PatternDisjunction(PatternAlternative* parent = 0)
        : m_parent(parent)
        , m_hasFixedSize(false)
    {
    }

    ~PatternDisjunction()
    {
        deleteAllValues(m_alternatives);
    }

    PatternAlternative* addNewAlternative()
    {
        PatternAlternative* alternative = js_new<PatternAlternative>(this);
        m_alternatives.append(alternative);
        return alternative;
    }

    Vector<PatternAlternative*> m_alternatives;
    PatternAlternative* m_parent;
    unsigned m_minimumSize;
    unsigned m_callFrameSize;
    bool m_hasFixedSize;
};

// You probably don't want to be calling these functions directly
// (please to be calling newlineCharacterClass() et al on your
// friendly neighborhood YarrPattern instance to get nicely
// cached copies).
CharacterClass* newlineCreate();
CharacterClass* digitsCreate();
CharacterClass* spacesCreate();
CharacterClass* wordcharCreate();
CharacterClass* nondigitsCreate();
CharacterClass* nonspacesCreate();
CharacterClass* nonwordcharCreate();

struct TermChain {
    TermChain(PatternTerm term)
        : term(term)
    {}

    PatternTerm term;
    Vector<TermChain> hotTerms;
};

struct YarrPattern {
    YarrPattern(const String& pattern, bool ignoreCase, bool multiline, ErrorCode* error);

    ~YarrPattern()
    {
        deleteAllValues(m_disjunctions);
        deleteAllValues(m_userCharacterClasses);
    }

    void reset()
    {
        m_numSubpatterns = 0;
        m_maxBackReference = 0;

        m_containsBackreferences = false;
        m_containsBOL = false;

        newlineCached = 0;
        digitsCached = 0;
        spacesCached = 0;
        wordcharCached = 0;
        nondigitsCached = 0;
        nonspacesCached = 0;
        nonwordcharCached = 0;

        deleteAllValues(m_disjunctions);
        m_disjunctions.clear();
        deleteAllValues(m_userCharacterClasses);
        m_userCharacterClasses.clear();
    }

    bool containsIllegalBackReference()
    {
        return m_maxBackReference > m_numSubpatterns;
    }

    CharacterClass* newlineCharacterClass()
    {
        if (!newlineCached)
            m_userCharacterClasses.append(newlineCached = newlineCreate());
        return newlineCached;
    }
    CharacterClass* digitsCharacterClass()
    {
        if (!digitsCached)
            m_userCharacterClasses.append(digitsCached = digitsCreate());
        return digitsCached;
    }
    CharacterClass* spacesCharacterClass()
    {
        if (!spacesCached)
            m_userCharacterClasses.append(spacesCached = spacesCreate());
        return spacesCached;
    }
    CharacterClass* wordcharCharacterClass()
    {
        if (!wordcharCached)
            m_userCharacterClasses.append(wordcharCached = wordcharCreate());
        return wordcharCached;
    }
    CharacterClass* nondigitsCharacterClass()
    {
        if (!nondigitsCached)
            m_userCharacterClasses.append(nondigitsCached = nondigitsCreate());
        return nondigitsCached;
    }
    CharacterClass* nonspacesCharacterClass()
    {
        if (!nonspacesCached)
            m_userCharacterClasses.append(nonspacesCached = nonspacesCreate());
        return nonspacesCached;
    }
    CharacterClass* nonwordcharCharacterClass()
    {
        if (!nonwordcharCached)
            m_userCharacterClasses.append(nonwordcharCached = nonwordcharCreate());
        return nonwordcharCached;
    }

    bool m_ignoreCase : 1;
    bool m_multiline : 1;
    bool m_containsBackreferences : 1;
    bool m_containsBOL : 1;
    unsigned m_numSubpatterns;
    unsigned m_maxBackReference;
    PatternDisjunction* m_body;
    Vector<PatternDisjunction*, 4> m_disjunctions;
    Vector<CharacterClass*> m_userCharacterClasses;

private:
    ErrorCode compile(const String& patternString);

    CharacterClass* newlineCached;
    CharacterClass* digitsCached;
    CharacterClass* spacesCached;
    CharacterClass* wordcharCached;
    CharacterClass* nondigitsCached;
    CharacterClass* nonspacesCached;
    CharacterClass* nonwordcharCached;
};

} } // namespace JSC::Yarr

#endif /* yarr_YarrPattern_h */
