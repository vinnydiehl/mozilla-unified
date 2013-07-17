/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _nsAccessiblePivot_H_
#define _nsAccessiblePivot_H_

#include "nsIAccessiblePivot.h"

#include "Accessible-inl.h"
#include "nsAutoPtr.h"
#include "nsTObserverArray.h"
#include "nsCycleCollectionParticipant.h"
#include "mozilla/Attributes.h"

class nsIAccessibleTraversalRule;
class RuleCache;

/**
 * Class represents an accessible pivot.
 */
class nsAccessiblePivot MOZ_FINAL : public nsIAccessiblePivot
{
public:
  typedef mozilla::a11y::Accessible Accessible;

  nsAccessiblePivot(Accessible* aRoot);

  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_CLASS_AMBIGUOUS(nsAccessiblePivot, nsIAccessiblePivot)

  NS_DECL_NSIACCESSIBLEPIVOT

  /*
   * A simple getter for the pivot's position.
   */
  Accessible* Position() { return mPosition; }

private:
  nsAccessiblePivot() MOZ_DELETE;
  nsAccessiblePivot(const nsAccessiblePivot&) MOZ_DELETE;
  void operator = (const nsAccessiblePivot&) MOZ_DELETE;

  /*
   * Notify all observers on a pivot change. Return true if it has changed and
   * observers have been notified.
   */
  bool NotifyOfPivotChange(Accessible* aOldAccessible,
                           int32_t aOldStart, int32_t aOldEnd,
                           PivotMoveReason aReason);

  /*
   * Check to see that the given accessible is a descendant of given ancestor
   */
  bool IsDescendantOf(Accessible* aAccessible, Accessible* aAncestor);


  /*
   * Search in preorder for the first accessible to match the rule.
   */
  Accessible* SearchForward(Accessible* aAccessible,
                            nsIAccessibleTraversalRule* aRule,
                            bool aSearchCurrent,
                            nsresult* aResult);

  /*
   * Reverse search in preorder for the first accessible to match the rule.
   */
  Accessible* SearchBackward(Accessible* aAccessible,
                             nsIAccessibleTraversalRule* aRule,
                             bool aSearchCurrent,
                             nsresult* aResult);

  /*
   * Get the effective root for this pivot, either the true root or modal root.
   */
  Accessible* GetActiveRoot() const
  {
    if (mModalRoot) {
      NS_ENSURE_FALSE(mModalRoot->IsDefunct(), mRoot);
      return mModalRoot;
    }

    return mRoot;
  }

  /*
   * Update the pivot, and notify observers. Return true if it moved.
   */
  bool MovePivotInternal(Accessible* aPosition, PivotMoveReason aReason);

  /*
   * Get initial node we should start a search from with a given rule.
   *
   * When we do a move operation from one position to another,
   * the initial position can be inside of a subtree that is ignored by
   * the given rule. We need to step out of the ignored subtree and start
   * the search from there.
   *
   */
  Accessible* AdjustStartPosition(Accessible* aAccessible, RuleCache& aCache,
                                  uint16_t* aFilterResult, nsresult* aResult);

  /*
   * The root accessible.
   */
  nsRefPtr<Accessible> mRoot;

  /*
   * The temporary modal root accessible.
   */
  nsRefPtr<Accessible> mModalRoot;

  /*
   * The current pivot position.
   */
  nsRefPtr<Accessible> mPosition;

  /*
   * The text start offset ofthe pivot.
   */
  int32_t mStartOffset;

  /*
   * The text end offset ofthe pivot.
   */
  int32_t mEndOffset;

  /*
   * The list of pivot-changed observers.
   */
  nsTObserverArray<nsCOMPtr<nsIAccessiblePivotObserver> > mObservers;
};

#endif
