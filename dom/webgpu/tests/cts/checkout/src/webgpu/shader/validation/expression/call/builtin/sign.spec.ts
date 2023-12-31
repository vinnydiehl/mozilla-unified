const builtin = 'sign';
export const description = `
Validation tests for the ${builtin}() builtin.
`;

import { makeTestGroup } from '../../../../../../common/framework/test_group.js';
import { keysOf, objectsToRecord } from '../../../../../../common/util/data_tables.js';
import {
  TypeF16,
  TypeF32,
  elementType,
  kAllFloatAndSignedIntegerScalarsAndVectors,
  kAllUnsignedIntegerScalarsAndVectors,
} from '../../../../../util/conversion.js';
import { ShaderValidationTest } from '../../../shader_validation_test.js';

import {
  fullRangeForType,
  kConstantAndOverrideStages,
  stageSupportsType,
  validateConstOrOverrideBuiltinEval,
} from './const_override_validation.js';

export const g = makeTestGroup(ShaderValidationTest);

const kValuesTypes = objectsToRecord(kAllFloatAndSignedIntegerScalarsAndVectors);

g.test('values')
  .desc(
    `
Validates that constant evaluation and override evaluation of ${builtin}() inputs rejects invalid values
`
  )
  .params(u =>
    u
      .combine('stage', kConstantAndOverrideStages)
      .combine('type', keysOf(kValuesTypes))
      .filter(u => stageSupportsType(u.stage, kValuesTypes[u.type]))
      .beginSubcases()
      .expand('value', u => fullRangeForType(kValuesTypes[u.type]))
  )
  .beforeAllSubcases(t => {
    if (elementType(kValuesTypes[t.params.type]) === TypeF16) {
      t.selectDeviceOrSkipTestCase('shader-f16');
    }
  })
  .fn(t => {
    const expectedResult = true; // Result should always be representable by the type
    validateConstOrOverrideBuiltinEval(
      t,
      builtin,
      expectedResult,
      [kValuesTypes[t.params.type].create(t.params.value)],
      t.params.stage
    );
  });

const kUnsignedIntegerArgumentTypes = objectsToRecord([
  TypeF32,
  ...kAllUnsignedIntegerScalarsAndVectors,
]);

g.test('unsigned_integer_argument')
  .desc(
    `
Validates that scalar and vector integer arguments are rejected by ${builtin}()
`
  )
  .params(u => u.combine('type', keysOf(kUnsignedIntegerArgumentTypes)))
  .fn(t => {
    const type = kUnsignedIntegerArgumentTypes[t.params.type];
    validateConstOrOverrideBuiltinEval(
      t,
      builtin,
      /* expectedResult */ type === TypeF32,
      [type.create(1)],
      'constant'
    );
  });
