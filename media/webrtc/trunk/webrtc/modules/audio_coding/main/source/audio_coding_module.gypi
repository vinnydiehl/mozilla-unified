# Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

{
  'variables': {
    'audio_coding_dependencies': [
      'CNG',
      'NetEq',
      '<(webrtc_root)/common_audio/common_audio.gyp:resampler',
      '<(webrtc_root)/common_audio/common_audio.gyp:signal_processing',
      '<(webrtc_root)/common_audio/common_audio.gyp:vad',
      '<(webrtc_root)/system_wrappers/source/system_wrappers.gyp:system_wrappers',
    ],
    'audio_coding_defines': [],
    'conditions': [
      ['include_opus==1', {
        'audio_coding_dependencies': ['webrtc_opus',],
        'audio_coding_defines': ['WEBRTC_CODEC_OPUS',],
        'audio_coding_sources': [
          'acm_opus.cc',
          'acm_opus.h',
        ],
      }],
      ['include_g711==1', {
        'audio_coding_dependencies': ['G711',],
        'audio_coding_defines': ['WEBRTC_CODEC_G711',],
        'audio_coding_sources': [
          'acm_pcma.cc',
          'acm_pcma.h',
          'acm_pcmu.cc',
          'acm_pcmu.h',
        ],
      }],
      ['include_g722==1', {
        'audio_coding_dependencies': ['G722',],
        'audio_coding_defines': ['WEBRTC_CODEC_G722',],
        'audio_coding_sources': [
          'acm_g722.cc',
          'acm_g722.h',
          'acm_g7221.cc',
          'acm_g7221.h',
          'acm_g7221c.cc',
          'acm_g7221c.h',
        ],
      }],
      ['include_ilbc==1', {
        'audio_coding_dependencies': ['iLBC',],
        'audio_coding_defines': ['WEBRTC_CODEC_ILBC',],
        'audio_coding_sources': [
          'acm_ilbc.cc',
          'acm_ilbc.h',
        ],
      }],
      ['include_isac==1', {
        'audio_coding_dependencies': ['iSAC', 'iSACFix',],
        'audio_coding_defines': ['WEBRTC_CODEC_ISAC', 'WEBRTC_CODEC_ISACFX',],
        'audio_coding_sources': [
          'acm_isac.cc',
          'acm_isac.h',
          'acm_isac_macros.h',
        ],
      }],
      ['include_pcm16b==1', {
        'audio_coding_dependencies': ['PCM16B',],
        'audio_coding_defines': ['WEBRTC_CODEC_PCM16',],
        'audio_coding_sources': [
          'acm_pcm16b.cc',
          'acm_pcm16b.h',
        ],
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'audio_coding_module',
      'type': 'static_library',
      'defines': [
        '<@(audio_coding_defines)',
      ],
      'dependencies': [
        '<@(audio_coding_dependencies)',
      ],
      'include_dirs': [
        '../interface',
        '../../../interface',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../interface',
          '../../../interface',
        ],
      },
      'sources': [
#        '<@(audio_coding_sources)',
        '../interface/audio_coding_module.h',
        '../interface/audio_coding_module_typedefs.h',
        'acm_cng.cc',
        'acm_cng.h',
        'acm_codec_database.cc',
        'acm_codec_database.h',
        'acm_dtmf_detection.cc',
        'acm_dtmf_detection.h',
        'acm_dtmf_playout.cc',
        'acm_dtmf_playout.h',
        'acm_generic_codec.cc',
        'acm_generic_codec.h',
        'acm_neteq.cc',
        'acm_neteq.h',
# cheat until I get audio_coding_sources to work
        'acm_opus.cc',
        'acm_opus.h',
        'acm_pcm16b.cc',
        'acm_pcm16b.h',
        'acm_pcma.cc',
        'acm_pcma.h',
        'acm_pcmu.cc',
        'acm_pcmu.h',
        'acm_red.cc',
        'acm_red.h',
        'acm_resampler.cc',
        'acm_resampler.h',
        'audio_coding_module.cc',
        'audio_coding_module_impl.cc',
        'audio_coding_module_impl.h',
      ],
    },
  ],
  'conditions': [
    ['include_tests==1', {
      'targets': [
        {
          'target_name': 'audio_coding_module_test',
          'type': 'executable',
          'dependencies': [
            'audio_coding_module',
            '<(webrtc_root)/test/test.gyp:test_support_main',
            '<(DEPTH)/testing/gtest.gyp:gtest',
            '<(webrtc_root)/system_wrappers/source/system_wrappers.gyp:system_wrappers',
            '<(webrtc_root)/modules/modules.gyp:webrtc_utility',
          ],
          'include_dirs': [
            '<(webrtc_root)/common_audio/resampler/include',
          ],
          'defines': [
            '<@(audio_coding_defines)',
          ],
          'sources': [
             '../test/ACMTest.cc',
             '../test/APITest.cc',
             '../test/Channel.cc',
             '../test/dual_stream_unittest.cc',
             '../test/EncodeDecodeTest.cc',
             '../test/iSACTest.cc',
             '../test/opus_test.cc',
             '../test/PCMFile.cc',
             '../test/RTPFile.cc',
             '../test/SpatialAudio.cc',
             '../test/TestAllCodecs.cc',
             '../test/Tester.cc',
             '../test/TestFEC.cc',
             '../test/TestStereo.cc',
             '../test/TestVADDTX.cc',
             '../test/TimedTrace.cc',
             '../test/TwoWayCommunication.cc',
             '../test/utility.cc',
             '../test/initial_delay_unittest.cc',
          ],
        },
        {
          'target_name': 'delay_test',
          'type': 'executable',
          'dependencies': [
            'audio_coding_module',
            '<(DEPTH)/testing/gtest.gyp:gtest',
            '<(webrtc_root)/test/test.gyp:test_support_main',
            '<(webrtc_root)/system_wrappers/source/system_wrappers.gyp:system_wrappers',
            '<(DEPTH)/third_party/google-gflags/google-gflags.gyp:google-gflags',
          ],
          'sources': [
             '../test/delay_test.cc',
             '../test/Channel.cc',
             '../test/PCMFile.cc',
           ],
        }, # delay_test
        {
          'target_name': 'audio_coding_unittests',
          'type': 'executable',
          'dependencies': [
            'audio_coding_module',
            'CNG',
            'iSACFix',
            'NetEq',
            'NetEq4',
            'NetEq4TestTools',
            'neteq_unittest_tools',
            'PCM16B',  # Needed by NetEq tests.
            '<(webrtc_root)/common_audio/common_audio.gyp:vad',
            '<(DEPTH)/testing/gmock.gyp:gmock',
            '<(DEPTH)/testing/gtest.gyp:gtest',
            '<(webrtc_root)/test/test.gyp:test_support_main',
            '<(webrtc_root)/system_wrappers/source/system_wrappers.gyp:system_wrappers',
          ],
          'sources': [
             'acm_neteq_unittest.cc',
             '../../codecs/cng/cng_unittest.cc',
             '../../codecs/isac/fix/source/filters_unittest.cc',
             '../../codecs/isac/fix/source/filterbanks_unittest.cc',
             '../../codecs/isac/fix/source/lpc_masking_model_unittest.cc',
             '../../codecs/isac/fix/source/transform_unittest.cc',
            '../../codecs/isac/main/source/isac_unittest.cc',
             '../../codecs/opus/opus_unittest.cc',
             # Test for NetEq 4.
             '../../neteq4/audio_multi_vector_unittest.cc',
             '../../neteq4/audio_vector_unittest.cc',
             '../../neteq4/background_noise_unittest.cc',
             '../../neteq4/buffer_level_filter_unittest.cc',
             '../../neteq4/comfort_noise_unittest.cc',
             '../../neteq4/decision_logic_unittest.cc',
             '../../neteq4/decoder_database_unittest.cc',
             '../../neteq4/delay_manager_unittest.cc',
             '../../neteq4/delay_peak_detector_unittest.cc',
             '../../neteq4/dsp_helper_unittest.cc',
             '../../neteq4/dtmf_buffer_unittest.cc',
             '../../neteq4/dtmf_tone_generator_unittest.cc',
             '../../neteq4/expand_unittest.cc',
             '../../neteq4/merge_unittest.cc',
             '../../neteq4/neteq_external_decoder_unittest.cc',
             '../../neteq4/neteq_impl_unittest.cc',
             '../../neteq4/neteq_stereo_unittest.cc',
             '../../neteq4/neteq_unittest.cc',
             '../../neteq4/normal_unittest.cc',
             '../../neteq4/packet_buffer_unittest.cc',
             '../../neteq4/payload_splitter_unittest.cc',
             '../../neteq4/post_decode_vad_unittest.cc',
             '../../neteq4/random_vector_unittest.cc',
             '../../neteq4/sync_buffer_unittest.cc',
             '../../neteq4/timestamp_scaler_unittest.cc',
             '../../neteq4/time_stretch_unittest.cc',
             '../../neteq4/mock/mock_audio_decoder.h',
             '../../neteq4/mock/mock_audio_vector.h',
             '../../neteq4/mock/mock_buffer_level_filter.h',
             '../../neteq4/mock/mock_decoder_database.h',
             '../../neteq4/mock/mock_delay_manager.h',
             '../../neteq4/mock/mock_delay_peak_detector.h',
             '../../neteq4/mock/mock_dtmf_buffer.h',
             '../../neteq4/mock/mock_dtmf_tone_generator.h',
             '../../neteq4/mock/mock_external_decoder_pcm16b.h',
             '../../neteq4/mock/mock_packet_buffer.h',
             '../../neteq4/mock/mock_payload_splitter.h',
          ],
          # Disable warnings to enable Win64 build, issue 1323.
          'msvs_disabled_warnings': [
            4267,  # size_t to int truncation.
          ],
        }, # audio_coding_unittests
      ],
    }],
  ],
}
