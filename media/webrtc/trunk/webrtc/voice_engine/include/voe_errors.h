/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VOICE_ENGINE_VOE_ERRORS_H
#define WEBRTC_VOICE_ENGINE_VOE_ERRORS_H

// Warnings
#define VE_PORT_NOT_DEFINED 8001
#define VE_CHANNEL_NOT_VALID 8002
#define VE_FUNC_NOT_SUPPORTED 8003
#define VE_INVALID_LISTNR 8004
#define VE_INVALID_ARGUMENT 8005
#define VE_INVALID_PORT_NMBR 8006
#define VE_INVALID_PLNAME 8007
#define VE_INVALID_PLFREQ 8008
#define VE_INVALID_PLTYPE 8009
#define VE_INVALID_PACSIZE 8010
#define VE_NOT_SUPPORTED 8011
#define VE_ALREADY_LISTENING 8012
#define VE_CHANNEL_NOT_CREATED 8013
#define VE_MAX_ACTIVE_CHANNELS_REACHED 8014
#define VE_REC_CANNOT_PREPARE_HEADER 8015
#define VE_REC_CANNOT_ADD_BUFFER 8016
#define VE_PLAY_CANNOT_PREPARE_HEADER 8017
#define VE_ALREADY_SENDING 8018
#define VE_INVALID_IP_ADDRESS 8019
#define VE_ALREADY_PLAYING 8020
#define VE_NOT_ALL_VERSION_INFO 8021
#define VE_DTMF_OUTOF_RANGE 8022
#define VE_INVALID_CHANNELS 8023
#define VE_SET_PLTYPE_FAILED 8024
#define VE_ENCRYPT_NOT_INITED 8025
#define VE_NOT_INITED 8026
#define VE_NOT_SENDING 8027
#define VE_EXT_TRANSPORT_NOT_SUPPORTED 8028
#define VE_EXTERNAL_TRANSPORT_ENABLED 8029
#define VE_STOP_RECORDING_FAILED 8030
#define VE_INVALID_RATE 8031
#define VE_INVALID_PACKET 8032
#define VE_NO_GQOS 8033
#define VE_INVALID_TIMESTAMP 8034
#define VE_RECEIVE_PACKET_TIMEOUT 8035
#define VE_STILL_PLAYING_PREV_DTMF 8036
#define VE_INIT_FAILED_WRONG_EXPIRY 8037
#define VE_SENDING 8038
#define VE_ENABLE_IPV6_FAILED 8039
#define VE_FUNC_NO_STEREO 8040
// Range 8041-8080 is not used
#define VE_FW_TRAVERSAL_ALREADY_INITIALIZED 8081
#define VE_PACKET_RECEIPT_RESTARTED 8082
#define VE_NOT_ALL_INFO 8083
#define VE_CANNOT_SET_SEND_CODEC 8084
#define VE_CODEC_ERROR 8085
#define VE_NETEQ_ERROR 8086
#define VE_RTCP_ERROR 8087
#define VE_INVALID_OPERATION 8088
#define VE_CPU_INFO_ERROR 8089
#define VE_SOUNDCARD_ERROR 8090
#define VE_SPEECH_LEVEL_ERROR 8091
#define VE_SEND_ERROR 8092
#define VE_CANNOT_REMOVE_CONF_CHANNEL 8093
#define VE_PLTYPE_ERROR 8094
#define VE_SET_FEC_FAILED 8095
#define VE_CANNOT_GET_PLAY_DATA 8096
#define VE_APM_ERROR 8097
#define VE_RUNTIME_PLAY_WARNING 8098
#define VE_RUNTIME_REC_WARNING 8099
#define VE_NOT_PLAYING 8100
#define VE_SOCKETS_NOT_INITED 8101
#define VE_CANNOT_GET_SOCKET_INFO 8102
#define VE_INVALID_MULTICAST_ADDRESS 8103
#define VE_DESTINATION_NOT_INITED 8104
#define VE_RECEIVE_SOCKETS_CONFLICT 8105
#define VE_SEND_SOCKETS_CONFLICT 8106
#define VE_TYPING_NOISE_WARNING 8107
#define VE_SATURATION_WARNING 8108
#define VE_NOISE_WARNING 8109
#define VE_CANNOT_GET_SEND_CODEC 8110
#define VE_CANNOT_GET_REC_CODEC 8111
#define VE_ALREADY_INITED 8112
#define VE_CANNOT_SET_SECONDARY_SEND_CODEC 8113
#define VE_CANNOT_GET_SECONDARY_SEND_CODEC 8114
#define VE_CANNOT_REMOVE_SECONDARY_SEND_CODEC 8115
#define VE_TYPING_NOISE_OFF_WARNING 8116

// Errors causing limited functionality
#define VE_RTCP_SOCKET_ERROR 9001
#define VE_MIC_VOL_ERROR 9002
#define VE_SPEAKER_VOL_ERROR 9003
#define VE_CANNOT_ACCESS_MIC_VOL 9004
#define VE_CANNOT_ACCESS_SPEAKER_VOL 9005
#define VE_GET_MIC_VOL_ERROR 9006
#define VE_GET_SPEAKER_VOL_ERROR 9007
#define VE_THREAD_RTCP_ERROR 9008
#define VE_CANNOT_INIT_APM 9009
#define VE_SEND_SOCKET_TOS_ERROR 9010
#define VE_CANNOT_RETRIEVE_DEVICE_NAME 9013
#define VE_SRTP_ERROR 9014
// 9015 is not used
#define VE_INTERFACE_NOT_FOUND 9016
#define VE_TOS_GQOS_CONFLICT 9017
#define VE_CANNOT_ADD_CONF_CHANNEL 9018
#define VE_BUFFER_TOO_SMALL 9019
#define VE_CANNOT_EXECUTE_SETTING 9020
#define VE_CANNOT_RETRIEVE_SETTING 9021
// 9022 is not used
#define VE_RTP_KEEPALIVE_FAILED 9023
#define VE_SEND_DTMF_FAILED 9024
#define VE_CANNOT_RETRIEVE_CNAME 9025
#define VE_DECRYPTION_FAILED 9026
#define VE_ENCRYPTION_FAILED 9027
#define VE_CANNOT_RETRIEVE_RTP_STAT 9028
#define VE_GQOS_ERROR 9029
#define VE_BINDING_SOCKET_TO_LOCAL_ADDRESS_FAILED 9030
#define VE_TOS_INVALID 9031
#define VE_TOS_ERROR 9032
#define VE_CANNOT_RETRIEVE_VALUE 9033

// Critical errors that stops voice functionality
#define VE_PLAY_UNDEFINED_SC_ERR 10001
#define VE_REC_CANNOT_OPEN_SC 10002
#define VE_SOCKET_ERROR 10003
#define VE_MMSYSERR_INVALHANDLE 10004
#define VE_MMSYSERR_NODRIVER 10005
#define VE_MMSYSERR_NOMEM 10006
#define VE_WAVERR_UNPREPARED 10007
#define VE_WAVERR_STILLPLAYING 10008
#define VE_UNDEFINED_SC_ERR 10009
#define VE_UNDEFINED_SC_REC_ERR 10010
#define VE_THREAD_ERROR 10011
#define VE_CANNOT_START_RECORDING 10012
#define VE_PLAY_CANNOT_OPEN_SC 10013
#define VE_NO_WINSOCK_2 10014
#define VE_SEND_SOCKET_ERROR 10015
#define VE_BAD_FILE 10016
#define VE_EXPIRED_COPY 10017
#define VE_NOT_AUTHORISED 10018
#define VE_RUNTIME_PLAY_ERROR 10019
#define VE_RUNTIME_REC_ERROR 10020
#define VE_BAD_ARGUMENT 10021
#define VE_LINUX_API_ONLY 10022
#define VE_REC_DEVICE_REMOVED 10023
#define VE_NO_MEMORY 10024
#define VE_BAD_HANDLE 10025
#define VE_RTP_RTCP_MODULE_ERROR 10026
#define VE_AUDIO_CODING_MODULE_ERROR 10027
#define VE_AUDIO_DEVICE_MODULE_ERROR 10028
#define VE_CANNOT_START_PLAYOUT 10029
#define VE_CANNOT_STOP_RECORDING 10030
#define VE_CANNOT_STOP_PLAYOUT 10031
#define VE_CANNOT_INIT_CHANNEL 10032
#define VE_RECV_SOCKET_ERROR 10033
#define VE_SOCKET_TRANSPORT_MODULE_ERROR 10034
#define VE_AUDIO_CONF_MIX_MODULE_ERROR 10035

// Warnings for other platforms (reserved range 8061-8080)
#define VE_IGNORED_FUNCTION 8061

#endif  //  WEBRTC_VOICE_ENGINE_VOE_ERRORS_H
