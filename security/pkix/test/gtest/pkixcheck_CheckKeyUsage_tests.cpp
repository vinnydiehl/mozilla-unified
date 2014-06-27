/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This code is made available to you under your choice of the following sets
 * of licensing terms:
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* Copyright 2013 Mozilla Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pkixgtest.h"

using namespace mozilla::pkix;
using namespace mozilla::pkix::test;

namespace mozilla { namespace pkix {

extern Result CheckKeyUsage(EndEntityOrCA endEntityOrCA,
                            const SECItem* encodedKeyUsage,
                            KeyUsage requiredKeyUsageIfPresent);

} } // namespace mozilla::pkix

#define ASSERT_BAD(x) \
  ASSERT_RecoverableError(SEC_ERROR_INADEQUATE_KEY_USAGE, x)

// Make it easy to define test data for the common, simplest cases.
#define NAMED_SIMPLE_KU(name, unusedBits, bits) \
  const uint8_t name##_bytes[4] = { \
    0x03/*BIT STRING*/, 0x02/*LENGTH=2*/, unusedBits, bits \
  }; \
  const SECItem name = { \
    siBuffer, \
    const_cast<uint8_t*>(name##_bytes), \
    4 \
  }

static uint8_t dummy;
static const SECItem empty_null    = { siBuffer, nullptr, 0 };
static const SECItem empty_nonnull = { siBuffer, &dummy, 0 };

// Note that keyCertSign is really the only interesting case for CA
// certificates since we don't support cRLSign.

TEST(pkixcheck_CheckKeyUsage, EE_none)
{
  // The input SECItem is nullptr. This means the cert had no keyUsage
  // extension. This is always valid because no key usage in an end-entity
  // means that there are no key usage restrictions.

  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, nullptr,
                               KeyUsage::noParticularKeyUsageRequired));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, nullptr,
                               KeyUsage::digitalSignature));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, nullptr,
                               KeyUsage::nonRepudiation));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, nullptr,
                               KeyUsage::keyEncipherment));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, nullptr,
                               KeyUsage::dataEncipherment));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, nullptr,
                               KeyUsage::keyAgreement));
}

TEST(pkixcheck_CheckKeyUsage, EE_empty)
{
  // The input SECItem is empty. The cert had an empty keyUsage extension,
  // which is syntactically invalid.
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &empty_null,
                           KeyUsage::digitalSignature));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &empty_nonnull,
                           KeyUsage::digitalSignature));
}

TEST(pkixcheck_CheckKeyUsage, CA_none)
{
  // A CA certificate does not have a KU extension.
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeCA, nullptr,
                               KeyUsage::keyCertSign));
}

TEST(pkixcheck_CheckKeyUsage, CA_empty)
{
  // A CA certificate has an empty KU extension.
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &empty_null,
                           KeyUsage::keyCertSign));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &empty_nonnull,
                           KeyUsage::keyCertSign));
}

TEST(pkixchekc_CheckKeyusage, maxUnusedBits)
{
  NAMED_SIMPLE_KU(encoded, 7, 0x80);
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &encoded,
                               KeyUsage::digitalSignature));
}

TEST(pkixchekc_CheckKeyusage, tooManyUnusedBits)
{
  static uint8_t oneValueByteData[] = {
    0x03/*BIT STRING*/, 0x02/*LENGTH=2*/, 8/*unused bits*/, 0x80
  };
  const SECItem oneValueByte = {
    siBuffer,
    oneValueByteData,
    sizeof(oneValueByteData)
  };
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &oneValueByte,
                           KeyUsage::digitalSignature));

  static uint8_t twoValueBytesData[] = {
    0x03/*BIT STRING*/, 0x03/*LENGTH=3*/, 8/*unused bits*/, 0x01, 0x00
  };
  const SECItem twoValueBytes = {
    siBuffer,
    twoValueBytesData,
    sizeof(twoValueBytesData)
  };
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &twoValueBytes,
                           KeyUsage::digitalSignature));
}

TEST(pkixcheck_CheckKeyUsage, NoValueBytes_NoPaddingBits)
{
  static const uint8_t DER_BYTES[] = {
    0x03/*BIT STRING*/, 0x01/*LENGTH=1*/, 0/*unused bits*/
  };
  static const SECItem DER = {
    siBuffer,
    const_cast<uint8_t*>(DER_BYTES),
    sizeof(DER_BYTES)
  };

  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &DER,
                           KeyUsage::digitalSignature));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &DER,
                           KeyUsage::keyCertSign));
}

TEST(pkixcheck_CheckKeyUsage, NoValueBytes_7PaddingBits)
{
  static const uint8_t DER_BYTES[] = {
    0x03/*BIT STRING*/, 0x01/*LENGTH=1*/, 7/*unused bits*/
  };
  static const SECItem DER = {
    siBuffer,
    const_cast<uint8_t*>(DER_BYTES),
    sizeof(DER_BYTES)
  };

  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &DER,
                           KeyUsage::digitalSignature));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &DER,
                           KeyUsage::keyCertSign));
}

void ASSERT_SimpleCase(uint8_t unusedBits, uint8_t bits, KeyUsage usage)
{
  // Test that only the right bit is accepted for the usage for both EE and CA
  // certs.
  NAMED_SIMPLE_KU(good, unusedBits, bits);
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &good, usage));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeCA, &good, usage));

  // We use (~bits >> unusedBits) << unusedBits) instead of using the same
  // calculation that is in CheckKeyUsage to validate that the calculation in
  // CheckKeyUsage is correct.

  // Test that none of the other non-padding bits are mistaken for the given
  // key usage in the single-byte value case.
  NAMED_SIMPLE_KU(notGood, unusedBits,
                  static_cast<uint8_t>((~bits >> unusedBits) << unusedBits));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &notGood, usage));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &notGood, usage));

  // Test that none of the other non-padding bits are mistaken for the given
  // key usage in the two-byte value case.
  uint8_t twoByteNotGoodData[] = {
    0x03/*BIT STRING*/, 0x03/*LENGTH=3*/, unusedBits,
    static_cast<uint8_t>(~bits),
    static_cast<uint8_t>((0xFFu >> unusedBits) << unusedBits)
  };
  const SECItem twoByteNotGood = {
    siBuffer, twoByteNotGoodData, sizeof(twoByteNotGoodData)
  };
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &twoByteNotGood,
                           usage));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &twoByteNotGood, usage));
}

TEST(pkixcheck_CheckKeyUsage, simpleCases)
{
  ASSERT_SimpleCase(7, 0x80, KeyUsage::digitalSignature);
  ASSERT_SimpleCase(6, 0x40, KeyUsage::nonRepudiation);
  ASSERT_SimpleCase(5, 0x20, KeyUsage::keyEncipherment);
  ASSERT_SimpleCase(4, 0x10, KeyUsage::dataEncipherment);
  ASSERT_SimpleCase(3, 0x08, KeyUsage::keyAgreement);
}

// Only CAs are allowed to assert keyCertSign
TEST(pkixcheck_CheckKeyUsage, keyCertSign)
{
  NAMED_SIMPLE_KU(good, 2, 0x04);
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &good,
                           KeyUsage::keyCertSign));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeCA, &good,
                               KeyUsage::keyCertSign));

  // Test that none of the other non-padding bits are mistaken for the given
  // key usage in the one-byte value case.
  NAMED_SIMPLE_KU(notGood, 2, 0xFB);
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &notGood,
                           KeyUsage::keyCertSign));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &notGood,
                           KeyUsage::keyCertSign));

  // Test that none of the other non-padding bits are mistaken for the given
  // key usage in the two-byte value case.
  static uint8_t twoByteNotGoodData[] = {
    0x03/*BIT STRING*/, 0x03/*LENGTH=3*/, 2/*unused bits*/, 0xFBu, 0xFCu
  };
  static const SECItem twoByteNotGood = {
    siBuffer, twoByteNotGoodData, sizeof(twoByteNotGoodData)
  };
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &twoByteNotGood,
                           KeyUsage::keyCertSign));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &twoByteNotGood,
                           KeyUsage::keyCertSign));
}

TEST(pkixcheck_CheckKeyUsage, unusedBitNotZero)
{
  // single byte control case
  static uint8_t controlOneValueByteData[] = {
    0x03/*BIT STRING*/, 0x02/*LENGTH=2*/, 7/*unused bits*/, 0x80
  };
  const SECItem controlOneValueByte = {
    siBuffer,
    controlOneValueByteData,
    sizeof(controlOneValueByteData)
  };
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity,
                               &controlOneValueByte,
                               KeyUsage::digitalSignature));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeCA, &controlOneValueByte,
                               KeyUsage::digitalSignature));

  // single-byte test case
  static uint8_t oneValueByteData[] = {
    0x03/*BIT STRING*/, 0x02/*LENGTH=2*/, 7/*unused bits*/, 0x80 | 0x01
  };
  const SECItem oneValueByte = {
    siBuffer,
    oneValueByteData,
    sizeof(oneValueByteData)
  };
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &oneValueByte,
                           KeyUsage::digitalSignature));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &oneValueByte,
                           KeyUsage::digitalSignature));

  // two-byte control case
  static uint8_t controlTwoValueBytesData[] = {
    0x03/*BIT STRING*/, 0x03/*LENGTH=3*/, 7/*unused bits*/,
    0x80 | 0x01, 0x80
  };
  const SECItem controlTwoValueBytes = {
    siBuffer,
    controlTwoValueBytesData,
    sizeof(controlTwoValueBytesData)
  };
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity,
                               &controlTwoValueBytes,
                               KeyUsage::digitalSignature));
  ASSERT_Success(CheckKeyUsage(EndEntityOrCA::MustBeCA, &controlTwoValueBytes,
                               KeyUsage::digitalSignature));

  // two-byte test case
  static uint8_t twoValueBytesData[] = {
    0x03/*BIT STRING*/, 0x03/*LENGTH=3*/, 7/*unused bits*/,
    0x80 | 0x01, 0x80 | 0x01
  };
  const SECItem twoValueBytes = {
    siBuffer,
    twoValueBytesData,
    sizeof(twoValueBytesData)
  };
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeEndEntity, &twoValueBytes,
                           KeyUsage::digitalSignature));
  ASSERT_BAD(CheckKeyUsage(EndEntityOrCA::MustBeCA, &twoValueBytes,
                           KeyUsage::digitalSignature));
}
