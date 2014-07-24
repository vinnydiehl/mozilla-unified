/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "nspr.h"

#include "mozilla/dom/SmartCardEvent.h"
#include "mozilla/unused.h"
#include "nsIDOMCryptoLegacy.h"
#include "nsIDOMDocument.h"
#include "nsIDOMWindow.h"
#include "nsIDOMWindowCollection.h"
#include "nsISimpleEnumerator.h"
#include "nsIWindowWatcher.h"
#include "nsServiceManagerUtils.h"
#include "nsSmartCardMonitor.h"
#include "pk11func.h"

using namespace mozilla;
using namespace mozilla::dom;

//
// The SmartCard monitoring thread should start up for each module we load
// that has removable tokens. This code calls an NSS function which waits
// until there is a change in the token state. NSS uses the 
// C_WaitForSlotEvent() call in PKCS #11 if the module implements the call,
// otherwise NSS will poll the token in a loop with a delay of 'latency' 
// between polls. Note that the C_WaitForSlotEvent() may wake up on any type
// of token event, so it's necessary to filter these events down to just the
// insertion and removal events we are looking for.
//
// Once the event is found, It is passed to nsNSSComponent for dispatching
// on the UI thread, and forwarding to any interested listeners (including
// javascript).
//

//This class is used to run the callback code
//passed to the event handlers for smart card notification
class nsTokenEventRunnable : public nsIRunnable {
public:
  nsTokenEventRunnable(const nsAString& aType, const nsAString& aTokenName);

  NS_IMETHOD Run ();
  NS_DECL_THREADSAFE_ISUPPORTS
protected:
  virtual ~nsTokenEventRunnable();
private:
  nsresult DispatchEventToWindow(nsIDOMWindow* domWin);

  nsString mType;
  nsString mTokenName;
};

// ISuuports implementation for nsTokenEventRunnable
NS_IMPL_ISUPPORTS(nsTokenEventRunnable, nsIRunnable)

nsTokenEventRunnable::nsTokenEventRunnable(const nsAString& aType,
                                           const nsAString& aTokenName)
  : mType(aType)
  , mTokenName(aTokenName)
{
}

nsTokenEventRunnable::~nsTokenEventRunnable() { }

//Implementation that runs the callback passed to
//crypto.generateCRMFRequest as an event.
NS_IMETHODIMP
nsTokenEventRunnable::Run()
{
  // 'Dispatch' the event to all the windows. 'DispatchEventToWindow()' will
  // first check to see if a given window has requested crypto events.
  nsresult rv;
  nsCOMPtr<nsIWindowWatcher> windowWatcher =
                            do_GetService(NS_WINDOWWATCHER_CONTRACTID, &rv);

  if (NS_FAILED(rv)) {
    return rv;
  }

  nsCOMPtr<nsISimpleEnumerator> enumerator;
  rv = windowWatcher->GetWindowEnumerator(getter_AddRefs(enumerator));
  if (NS_FAILED(rv)) {
    return rv;
  }

  bool hasMoreWindows;

  while (NS_SUCCEEDED(enumerator->HasMoreElements(&hasMoreWindows))
         && hasMoreWindows) {
    nsCOMPtr<nsISupports> supports;
    enumerator->GetNext(getter_AddRefs(supports));
    nsCOMPtr<nsIDOMWindow> domWin(do_QueryInterface(supports));
    if (domWin) {
      nsresult rv2 = DispatchEventToWindow(domWin);
      if (NS_FAILED(rv2)) {
        // return the last failure, don't let a single failure prevent
        // continued delivery of events.
        rv = rv2;
      }
    }
  }
  return rv;
}

nsresult
nsTokenEventRunnable::DispatchEventToWindow(nsIDOMWindow* domWin)
{
  if (!domWin) {
    return NS_OK;
  }

  // first walk the children and dispatch their events
  nsresult rv;
  nsCOMPtr<nsIDOMWindowCollection> frames;
  rv = domWin->GetFrames(getter_AddRefs(frames));
  if (NS_FAILED(rv)) {
    return rv;
  }

  uint32_t length;
  frames->GetLength(&length);
  uint32_t i;
  for (i = 0; i < length; i++) {
    nsCOMPtr<nsIDOMWindow> childWin;
    frames->Item(i, getter_AddRefs(childWin));
    DispatchEventToWindow(childWin);
  }

  // check if we've enabled smart card events on this window
  // NOTE: it's not an error to say that we aren't going to dispatch
  // the event.
  nsCOMPtr<nsIDOMCrypto> crypto;
  domWin->GetCrypto(getter_AddRefs(crypto));
  if (!crypto) {
    return NS_OK; // nope, it doesn't have a crypto property
  }

  bool boolrv;
  crypto->GetEnableSmartCardEvents(&boolrv);
  if (!boolrv) {
    return NS_OK; // nope, it's not enabled.
  }

  // dispatch the event ...

  // find the document
  nsCOMPtr<nsIDOMDocument> doc;
  rv = domWin->GetDocument(getter_AddRefs(doc));
  if (!doc) {
    return NS_FAILED(rv) ? rv : NS_ERROR_FAILURE;
  }

  nsCOMPtr<EventTarget> d = do_QueryInterface(doc);

  SmartCardEventInit init;
  init.mBubbles = false;
  init.mCancelable = true;
  init.mTokenName = mTokenName;

  nsRefPtr<SmartCardEvent> event = SmartCardEvent::Constructor(d, mType, init);
  event->SetTrusted(true);

  return d->DispatchEvent(event, &boolrv);
}

// self linking and removing double linked entry
// adopts the thread it is passed.
class SmartCardThreadEntry {
public:
 SmartCardThreadEntry *next;
 SmartCardThreadEntry *prev;
 SmartCardThreadEntry **head;
 SmartCardMonitoringThread *thread;
 SmartCardThreadEntry(SmartCardMonitoringThread *thread_,
   SmartCardThreadEntry *next_, SmartCardThreadEntry *prev_,
   SmartCardThreadEntry **head_) : 
   next(next_), prev(prev_), head(head_), thread(thread_) { 
    if (prev) { prev->next = this; } else { *head = this; }
    if (next) { next->prev = this; }
  }
  ~SmartCardThreadEntry() {
    if (prev) { prev->next = next; } else { *head = next; }
    if (next) { next->prev = prev; }
    // NOTE: automatically stops the thread
    delete thread;
  }
};

//
// SmartCardThreadList is a class to help manage the running threads.
// That way new threads could be started and old ones terminated as we
// load and unload modules.
//
SmartCardThreadList::SmartCardThreadList() : head(0)
{
}

SmartCardThreadList::~SmartCardThreadList()
{
  // the head is self linking and unlinking, the following
  // loop removes all entries on the list.
  // it will also stop the thread if it happens to be running
  while (head) {
    delete head;
  }
}

void
SmartCardThreadList::Remove(SECMODModule *aModule)
{
  for (SmartCardThreadEntry *current = head; current; current = current->next) {
    if (current->thread->GetModule() == aModule) {
      // NOTE: automatically stops the thread and dequeues it from the list
      delete current;
      return;
    }
  }
}

// adopts the thread passed to it. Starts the thread as well
nsresult
SmartCardThreadList::Add(SmartCardMonitoringThread *thread)
{
  SmartCardThreadEntry *current = new SmartCardThreadEntry(thread, head, nullptr,
                                                           &head);
  // OK to forget current here, it's on the list.
  unused << current;

  return thread->Start();
}


// We really should have a Unity PL Hash function...
static PLHashNumber
unity(const void *key) { return PLHashNumber(NS_PTR_TO_INT32(key)); }

SmartCardMonitoringThread::SmartCardMonitoringThread(SECMODModule *module_)
  : mThread(nullptr)
{
  mModule = SECMOD_ReferenceModule(module_);
  // simple hash functions, most modules have less than 3 slots, so 10 buckets
  // should be plenty
  mHash = PL_NewHashTable(10, unity, PL_CompareValues, 
                           PL_CompareStrings, nullptr, 0);
}

//
// when we shutdown the thread, be sure to stop it first. If not, it just might
// crash when the mModule it is looking at disappears.
//
SmartCardMonitoringThread::~SmartCardMonitoringThread()
{
  Stop();
  SECMOD_DestroyModule(mModule);
  if (mHash) {
    PL_HashTableDestroy(mHash);
  }
}

nsresult
SmartCardMonitoringThread::Start()
{
  if (!mThread) {
    mThread = PR_CreateThread(PR_SYSTEM_THREAD, LaunchExecute, this,
                              PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD,
                              PR_JOINABLE_THREAD, 0);
  }
  return mThread ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

//
// Should only stop if we are through with the module.
// CancelWait has the side effect of losing all the keys and
// current operations on the module!. (See the comment in
// SECMOD_CancelWait for why this is so..).
//
void SmartCardMonitoringThread::Stop()
{
  SECStatus rv;

  rv = SECMOD_CancelWait(mModule);
  if (rv != SECSuccess) {
    // we didn't wake up the Wait, so don't try to join the thread 
    // otherwise we will hang forever...
    return;
  }
 
  // confused about the memory model here? NSPR owns the memory for
  // threads. non-joinable threads are freed when the thread dies.
  // joinable threads are freed after the call to PR_JoinThread.
  // That means if SECMOD_CancelWait fails, we'll leak the mThread
  // structure. this is considered preferable to hanging (which is
  // what will happen if we try to join a thread that blocked).
  if (mThread) {
    PR_JoinThread(mThread);
    mThread = 0; 
  }
}

//
// remember the name and series of a token in a particular slot.
// This is important because the name is no longer available when
// the token is removed. If listeners depended on this information,
// They would be out of luck. It also is a handy way of making sure
// we don't generate spurious insertion and removal events as the slot
// cycles through various states.
//
void
SmartCardMonitoringThread::SetTokenName(CK_SLOT_ID slotid, 
                                       const char *tokenName, uint32_t series)
{
  if (mHash) {
    if (tokenName) {
      int len = strlen(tokenName) + 1;
      /* this must match the allocator used in
       * PLHashAllocOps.freeEntry DefaultFreeEntry */
      char *entry = (char *)PR_Malloc(len+sizeof(uint32_t));
     
      if (entry) {  
        memcpy(entry,&series,sizeof(uint32_t));
        memcpy(&entry[sizeof(uint32_t)],tokenName,len);

        PL_HashTableAdd(mHash,(void *)(uintptr_t)slotid, entry); /* adopt */
        return;
      }
    } 
    else {
      // if tokenName was not provided, remove the old one (implicit delete)
      PL_HashTableRemove(mHash,(void *)(uintptr_t)slotid);
    }
  }
}

// retrieve the name saved above
const char *
SmartCardMonitoringThread::GetTokenName(CK_SLOT_ID slotid)
{
  const char *tokenName = nullptr;
  const char *entry;

  if (mHash) {
    entry = (const char *)PL_HashTableLookupConst(mHash,(void *)(uintptr_t)slotid);
    if (entry) {
      tokenName = &entry[sizeof(uint32_t)];
    }
  }
  return tokenName;
}

// retrieve the series saved in SetTokenName above
uint32_t
SmartCardMonitoringThread::GetTokenSeries(CK_SLOT_ID slotid)
{
  uint32_t series = 0;
  const char *entry;

  if (mHash) {
    entry = (const char *)PL_HashTableLookupConst(mHash,(void *)(uintptr_t)slotid);
    if (entry) {
      memcpy(&series,entry,sizeof(uint32_t));
    }
  }
  return series;
}

//
// helper function to pass the event off to nsNSSComponent.
//
nsresult
SmartCardMonitoringThread::SendEvent(const nsAString &eventType,
                                     const char *tokenName)
{
  nsCOMPtr<nsIRunnable> runnable =
                               new nsTokenEventRunnable(eventType, NS_ConvertUTF8toUTF16(tokenName));

  return NS_DispatchToMainThread(runnable);
}

//
// This is the main loop.
//
void SmartCardMonitoringThread::Execute()
{
  PK11SlotInfo *slot;
  const char *tokenName = nullptr;

  //
  // populate token names for already inserted tokens.
  //
  PK11SlotList *sl =
            PK11_FindSlotsByNames(mModule->dllName, nullptr, nullptr, true);
  PK11SlotListElement *sle;
 
  if (sl) {
    for (sle=PK11_GetFirstSafe(sl); sle; 
                                      sle=PK11_GetNextSafe(sl,sle,false)) {
      SetTokenName(PK11_GetSlotID(sle->slot), 
                  PK11_GetTokenName(sle->slot), PK11_GetSlotSeries(sle->slot));
    }
    PK11_FreeSlotList(sl);
  }

  // loop starts..
  do {
    slot = SECMOD_WaitForAnyTokenEvent(mModule, 0, PR_SecondsToInterval(1)  );
    if (!slot) {
      break;
    }

    // now we have a potential insertion or removal event, see if the slot
    // is present to determine which it is...
    if (PK11_IsPresent(slot)) {
      // insertion
      CK_SLOT_ID slotID = PK11_GetSlotID(slot);
      uint32_t series = PK11_GetSlotSeries(slot);

      // skip spurious insertion events...
      if (series != GetTokenSeries(slotID)) {
        // if there's a token name, then we have not yet issued a remove
        // event for the previous token, do so now...
        tokenName = GetTokenName(slotID);
        if (tokenName) {
          SendEvent(NS_LITERAL_STRING("smartcard-remove"), tokenName);
        }
        tokenName = PK11_GetTokenName(slot);
        // save the token name and series
        SetTokenName(slotID, tokenName, series);
        SendEvent(NS_LITERAL_STRING("smartcard-insert"), tokenName);
      }
    } else {
      // retrieve token name 
      CK_SLOT_ID slotID = PK11_GetSlotID(slot);
      tokenName = GetTokenName(slotID);
      // if there's not a token name, then the software isn't expecting
      // a (or another) remove event.
      if (tokenName) {
        SendEvent(NS_LITERAL_STRING("smartcard-remove"), tokenName);
        // clear the token name (after we send it)
        SetTokenName(slotID, nullptr, 0);
      }
    }
    PK11_FreeSlot(slot);

  } while (1);
}

// accessor to help searching active Monitoring threads
const SECMODModule * SmartCardMonitoringThread::GetModule() 
{
  return mModule;
}

// C-like calling sequence to glue into PR_CreateThread.
void SmartCardMonitoringThread::LaunchExecute(void *arg)
{
  PR_SetCurrentThreadName("SmartCard");

  ((SmartCardMonitoringThread*)arg)->Execute();
}

