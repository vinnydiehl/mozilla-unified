/* vim: se cin sw=2 ts=2 et : */
/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __mozilla_widget_GfxInfoBase_h__
#define __mozilla_widget_GfxInfoBase_h__

#include "nsIGfxInfo.h"
#include "nsCOMPtr.h"
#include "nsIObserver.h"
#include "nsWeakReference.h"
#include "GfxDriverInfo.h"
#include "nsTArray.h"
#include "nsString.h"
#include "GfxInfoCollector.h"
#include "nsIGfxInfoDebug.h"

namespace mozilla {
namespace widget {  

class GfxInfoBase : public nsIGfxInfo,
                    public nsIObserver,
                    public nsSupportsWeakReference
#ifdef DEBUG
                  , public nsIGfxInfoDebug
#endif
{
public:
  GfxInfoBase();
  virtual ~GfxInfoBase();

  NS_DECL_ISUPPORTS
  NS_DECL_NSIOBSERVER

  // We only declare a subset of the nsIGfxInfo interface. It's up to derived
  // classes to implement the rest of the interface.  
  // Derived classes need to use
  // using GfxInfoBase::GetFeatureStatus;
  // using GfxInfoBase::GetFeatureSuggestedDriverVersion;
  // using GfxInfoBase::GetWebGLParameter;
  // using GfxInfoBase::GetAzureEnabled;
  // to import the relevant methods into their namespace.
  NS_IMETHOD GetFeatureStatus(PRInt32 aFeature, PRInt32 *_retval);
  NS_IMETHOD GetFeatureSuggestedDriverVersion(PRInt32 aFeature, nsAString & _retval);
  NS_IMETHOD GetWebGLParameter(const nsAString & aParam, nsAString & _retval);
  NS_IMETHOD GetAzureEnabled(bool *aAzureEnabled);

  NS_IMETHOD GetFailures(PRUint32 *failureCount, char ***failures);
  NS_IMETHOD_(void) LogFailure(const nsACString &failure);
  NS_IMETHOD GetInfo(JSContext*, jsval*);

  // Initialization function. If you override this, you must call this class's
  // version of Init first.
  // We need Init to be called separately from the constructor so we can
  // register as an observer after all derived classes have been constructed
  // and we know we have a non-zero refcount.
  // Ideally, Init() would be void-return, but the rules of
  // NS_GENERIC_FACTORY_CONSTRUCTOR_INIT require it be nsresult return.
  virtual nsresult Init();
  
  // only useful on X11
  NS_IMETHOD_(void) GetData() { }

  static void AddCollector(GfxInfoCollectorBase* collector);
  static void RemoveCollector(GfxInfoCollectorBase* collector);

  static nsTArray<GfxDriverInfo>* mDriverInfo;
  static bool mDriverInfoObserverInitialized;

protected:

  virtual nsresult GetFeatureStatusImpl(PRInt32 aFeature, PRInt32* aStatus,
                                        nsAString& aSuggestedDriverVersion,
                                        const nsTArray<GfxDriverInfo>& aDriverInfo,
                                        OperatingSystem* aOS = nsnull);

  // Gets the driver info table. Used by GfxInfoBase to check for general cases
  // (while subclasses check for more specific ones).
  virtual const nsTArray<GfxDriverInfo>& GetGfxDriverInfo() = 0;

private:
  virtual PRInt32 FindBlocklistedDeviceInList(const nsTArray<GfxDriverInfo>& aDriverInfo,
                                              nsAString& aSuggestedVersion,
                                              PRInt32 aFeature,
                                              OperatingSystem os);

  void EvaluateDownloadedBlacklist(nsTArray<GfxDriverInfo>& aDriverInfo);

  nsCString mFailures[9]; // The choice of 9 is Ehsan's
  PRUint32 mFailureCount;

};

}
}

#endif /* __mozilla_widget_GfxInfoBase_h__ */
