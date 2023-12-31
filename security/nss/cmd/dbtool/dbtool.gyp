# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
{
  'includes': [
    '../../coreconf/config.gypi',
    '../../cmd/platlibs.gypi'
  ],
  'targets': [
    {
      'target_name': 'dbtool',
      'type': 'executable',
      'sources': [
        'dbtool.c',
        '../../lib/softoken/sdb.c'
      ],
      'dependencies': [
        '<(DEPTH)/exports.gyp:dbm_exports',
        '<(DEPTH)/exports.gyp:nss_exports'
      ],
      'conditions': [
         [ 'use_system_sqlite==1', {
           'dependencies': [
             '<(DEPTH)/lib/sqlite/sqlite.gyp:sqlite3',
           ],
         }, {
           'dependencies': [
             '<(DEPTH)/lib/sqlite/sqlite.gyp:sqlite',
           ],
         }],
     ] }
  ],
  'variables': {
    'module': 'nss'
  }
}
