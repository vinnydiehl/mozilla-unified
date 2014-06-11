/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim:cindent:ts=2:et:sw=2:
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This Original Code has been modified by IBM Corporation. Modifications made
 * by IBM described herein are Copyright (c) International Business Machines
 * Corporation, 2000. Modifications to Mozilla code or documentation identified
 * per MPL Section 3.3
 *
 * Date             Modified by     Description of modification
 * 04/20/2000       IBM Corp.      OS/2 VisualAge build.
 */

/* storage of the frame tree and information about it */

#ifndef _nsFrameManager_h_
#define _nsFrameManager_h_

#include "nsFrameManagerBase.h"
#include "nsFrameList.h"
#include "nsIPresShell.h" // Needed for our inline GetPresContext() impl.

class nsIFrame;
class nsIContent;
class nsContainerFrame;

namespace mozilla {
/**
 * Node in a linked list, containing the style for an element that
 * does not have a frame but whose parent does have a frame.
 */
struct UndisplayedNode {
  UndisplayedNode(nsIContent* aContent, nsStyleContext* aStyle)
    : mContent(aContent),
      mStyle(aStyle),
      mNext(nullptr)
  {
    MOZ_COUNT_CTOR(mozilla::UndisplayedNode);
  }

  ~UndisplayedNode()
  {
    MOZ_COUNT_DTOR(mozilla::UndisplayedNode);

    // Delete mNext iteratively to avoid blowing up the stack (bug 460461).
    UndisplayedNode* cur = mNext;
    while (cur) {
      UndisplayedNode* next = cur->mNext;
      cur->mNext = nullptr;
      delete cur;
      cur = next;
    }
  }

  nsCOMPtr<nsIContent>      mContent;
  nsRefPtr<nsStyleContext>  mStyle;
  UndisplayedNode*          mNext;
};

} // namespace mozilla

/**
 * Frame manager interface. The frame manager serves two purposes:
 * <li>provides a service for mapping from content to frame and from
 * out-of-flow frame to placeholder frame.
 * <li>handles structural modifications to the frame model. If the frame model
 * lock can be acquired, then the changes are processed immediately; otherwise,
 * they're queued and processed later.
 *
 * Do not add virtual methods (a vtable pointer) or members to this class, or
 * else you'll break the validity of the reinterpret_cast in nsIPresShell's
 * FrameManager() method.
 */

class nsFrameManager : public nsFrameManagerBase
{
  typedef mozilla::layout::FrameChildListID ChildListID;

public:
  nsFrameManager(nsIPresShell *aPresShell, nsStyleSet* aStyleSet) {
    mPresShell = aPresShell;
    mStyleSet = aStyleSet;
    MOZ_ASSERT(mPresShell, "need a pres shell");
    MOZ_ASSERT(mStyleSet, "need a style set");
  }
  ~nsFrameManager();

  /*
   * After Destroy is called, it is an error to call any FrameManager methods.
   * Destroy should be called when the frame tree managed by the frame
   * manager is no longer being displayed.
   */
  void Destroy();

  // Placeholder frame functions
  nsPlaceholderFrame* GetPlaceholderFrameFor(const nsIFrame* aFrame);
  nsresult
    RegisterPlaceholderFrame(nsPlaceholderFrame* aPlaceholderFrame);

  void
    UnregisterPlaceholderFrame(nsPlaceholderFrame* aPlaceholderFrame);

  void      ClearPlaceholderFrameMap();

  // Mapping undisplayed content
  nsStyleContext* GetUndisplayedContent(nsIContent* aContent);
  mozilla::UndisplayedNode*
    GetAllUndisplayedContentIn(nsIContent* aParentContent);
  void SetUndisplayedContent(nsIContent* aContent,
                                         nsStyleContext* aStyleContext);
  void ChangeUndisplayedContent(nsIContent* aContent,
                                            nsStyleContext* aStyleContext);
  void ClearUndisplayedContentIn(nsIContent* aContent,
                                             nsIContent* aParentContent);
  void ClearAllUndisplayedContentIn(nsIContent* aParentContent);

  // Functions for manipulating the frame model
  void AppendFrames(nsContainerFrame* aParentFrame,
                    ChildListID       aListID,
                    nsFrameList&      aFrameList);

  void InsertFrames(nsContainerFrame* aParentFrame,
                    ChildListID       aListID,
                    nsIFrame*         aPrevFrame,
                    nsFrameList&      aFrameList);

  void RemoveFrame(ChildListID     aListID,
                   nsIFrame*       aOldFrame);

  /*
   * Notification that a frame is about to be destroyed. This allows any
   * outstanding references to the frame to be cleaned up.
   */
  void     NotifyDestroyingFrame(nsIFrame* aFrame);

  /*
   * Capture/restore frame state for the frame subtree rooted at aFrame.
   * aState is the document state storage object onto which each frame
   * stores its state.  Callers of CaptureFrameState are responsible for
   * traversing next continuations of special siblings of aFrame as
   * needed; this method will only work with actual frametree descendants
   * of aFrame.
   */

  void CaptureFrameState(nsIFrame*              aFrame,
                                     nsILayoutHistoryState* aState);

  void RestoreFrameState(nsIFrame*              aFrame,
                                     nsILayoutHistoryState* aState);

  /*
   * Add/restore state for one frame
   */
  void CaptureFrameStateFor(nsIFrame*              aFrame,
                                        nsILayoutHistoryState* aState);

  void RestoreFrameStateFor(nsIFrame*              aFrame,
                                        nsILayoutHistoryState* aState);
};

#endif
