/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsDomGamepad_h
#define nsDomGamepad_h

#include "mozilla/ErrorResult.h"
#include "mozilla/StandardInteger.h"
#include "nsCOMPtr.h"
#include "nsIDOMGamepad.h"
#include "nsString.h"
#include "nsTArray.h"
#include "nsWrapperCache.h"

class nsDOMGamepad : public nsIDOMGamepad
                   , public nsWrapperCache
{
public:
  nsDOMGamepad(nsISupports* aParent,
               const nsAString& aID, uint32_t aIndex,
               uint32_t aNumButtons, uint32_t aNumAxes);
  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS(nsDOMGamepad)

  nsDOMGamepad();
  void SetConnected(bool aConnected);
  void SetButton(uint32_t aButton, double aValue);
  void SetAxis(uint32_t aAxis, double aValue);
  void SetIndex(uint32_t aIndex);

  // Make the state of this gamepad equivalent to other.
  void SyncState(nsDOMGamepad* other);

  // Return a new nsDOMGamepad containing the same data as this object,
  // parented to aParent.
  already_AddRefed<nsDOMGamepad> Clone(nsISupports* aParent);

  nsISupports* GetParentObject() const
  {
    return mParent;
  }

  virtual JSObject*
  WrapObject(JSContext* aCx, JSObject* aScope) MOZ_OVERRIDE;

  void GetId(nsAString& aID) const
  {
    aID = mID;
  }

  bool Connected() const
  {
    return mConnected;
  }

  uint32_t Index() const
  {
    return mIndex;
  }

  already_AddRefed<nsIVariant> GetButtons(mozilla::ErrorResult& aRv)
  {
    nsCOMPtr<nsIVariant> buttons;
    aRv = GetButtons(getter_AddRefs(buttons));
    return buttons.forget();
  }

  already_AddRefed<nsIVariant> GetAxes(mozilla::ErrorResult& aRv)
  {
    nsCOMPtr<nsIVariant> axes;
    aRv = GetAxes(getter_AddRefs(axes));
    return axes.forget();
  }

private:
  virtual ~nsDOMGamepad() {}

  nsresult GetButtons(nsIVariant** aButtons);
  nsresult GetAxes(nsIVariant** aAxes);

protected:
  nsCOMPtr<nsISupports> mParent;
  nsString mID;
  uint32_t mIndex;

  // true if this gamepad is currently connected.
  bool mConnected;

  // Current state of buttons, axes.
  nsTArray<double> mButtons;
  nsTArray<double> mAxes;
};

#endif // nsDomGamepad_h
