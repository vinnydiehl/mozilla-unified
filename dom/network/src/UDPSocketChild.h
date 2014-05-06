/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_UDPSocketChild_h__
#define mozilla_dom_UDPSocketChild_h__

#include "mozilla/net/PUDPSocketChild.h"
#include "nsIUDPSocketChild.h"
#include "nsCycleCollectionParticipant.h"
#include "nsCOMPtr.h"

#define UDPSOCKETCHILD_CID \
  {0xb47e5a0f, 0xd384, 0x48ef, { 0x88, 0x85, 0x42, 0x59, 0x79, 0x3d, 0x9c, 0xf0 }}

namespace mozilla {
namespace dom {

class UDPSocketChildBase : public nsIUDPSocketChild {
public:
  NS_DECL_ISUPPORTS

  void AddIPDLReference();
  void ReleaseIPDLReference();

protected:
  UDPSocketChildBase();
  virtual ~UDPSocketChildBase();
  nsCOMPtr<nsIUDPSocketInternal> mSocket;
  bool mIPCOpen;
};

class UDPSocketChild : public mozilla::net::PUDPSocketChild
                     , public UDPSocketChildBase
{
public:
  NS_DECL_NSIUDPSOCKETCHILD
  NS_IMETHOD_(MozExternalRefCountType) Release() MOZ_OVERRIDE;

  UDPSocketChild();
  virtual ~UDPSocketChild();

  virtual bool RecvCallbackOpened(const UDPAddressInfo& aAddressInfo) MOZ_OVERRIDE;
  virtual bool RecvCallbackClosed() MOZ_OVERRIDE;
  virtual bool RecvCallbackReceivedData(const UDPAddressInfo& aAddressInfo,
                                        const InfallibleTArray<uint8_t>& aData) MOZ_OVERRIDE;
  virtual bool RecvCallbackError(const nsCString& aMessage,
                                 const nsCString& aFilename,
                                 const uint32_t& aLineNumber) MOZ_OVERRIDE;

private:
  nsresult SendDataInternal(const UDPSocketAddr& aAddr,
                            const uint8_t* aData,
                            const uint32_t aByteLength);

  uint16_t mLocalPort;
  nsCString mLocalAddress;
  nsCString mFilterName;
};

} // namespace dom
} // namespace mozilla

#endif // !defined(mozilla_dom_UDPSocketChild_h__)
