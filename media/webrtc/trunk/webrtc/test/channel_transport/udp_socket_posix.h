/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_TEST_CHANNEL_TRANSPORT_UDP_SOCKET_POSIX_H_
#define WEBRTC_TEST_CHANNEL_TRANSPORT_UDP_SOCKET_POSIX_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "webrtc/system_wrappers/interface/condition_variable_wrapper.h"
#include "webrtc/system_wrappers/interface/critical_section_wrapper.h"
#include "webrtc/test/channel_transport/udp_socket_wrapper.h"

namespace webrtc {
namespace test {

#define SOCKET_ERROR -1

class UdpSocketPosix : public UdpSocketWrapper
{
public:
    UdpSocketPosix(const int32_t id, UdpSocketManager* mgr,
                   bool ipV6Enable = false);

    virtual ~UdpSocketPosix();

    virtual int32_t ChangeUniqueId(const int32_t id) OVERRIDE;

    virtual bool SetCallback(CallbackObj obj,
                             IncomingSocketCallback cb) OVERRIDE;

    virtual bool Bind(const SocketAddress& name) OVERRIDE;

    virtual bool SetSockopt(int32_t level, int32_t optname,
                            const int8_t* optval, int32_t optlen) OVERRIDE;

    virtual int32_t SetTOS(const int32_t serviceType) OVERRIDE;

    virtual int32_t SendTo(const int8_t* buf, int32_t len,
                           const SocketAddress& to) OVERRIDE;

    // Deletes socket in addition to closing it.
    // TODO (hellner): make destructor protected.
    virtual void CloseBlocking() OVERRIDE;

    virtual SOCKET GetFd();
    virtual int32_t GetError();

    virtual bool ValidHandle() OVERRIDE;

    virtual bool SetQos(int32_t /*serviceType*/,
                        int32_t /*tokenRate*/,
                        int32_t /*bucketSize*/,
                        int32_t /*peekBandwith*/,
                        int32_t /*minPolicedSize*/,
                        int32_t /*maxSduSize*/,
                        const SocketAddress& /*stRemName*/,
                        int32_t /*overrideDSCP*/) OVERRIDE;

    bool CleanUp();
    void HasIncoming();
    bool WantsIncoming();
    void ReadyForDeletion();
private:
    friend class UdpSocketManagerPosix;

    int32_t _id;
    IncomingSocketCallback _incomingCb;
    CallbackObj _obj;
    int32_t _error;

    SOCKET _socket;
    UdpSocketManager* _mgr;
    ConditionVariableWrapper* _closeBlockingCompletedCond;
    ConditionVariableWrapper* _readyForDeletionCond;

    bool _closeBlockingActive;
    bool _closeBlockingCompleted;
    bool _readyForDeletion;

    CriticalSectionWrapper* _cs;
};

}  // namespace test
}  // namespace webrtc

#endif  // WEBRTC_TEST_CHANNEL_TRANSPORT_UDP_SOCKET_POSIX_H_
