/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at <http://mozilla.org/MPL/2.0/>. */

import {
  actions,
  selectors,
  createStore,
  makeSource,
} from "../../../utils/test-head";

import { initialSourceBlackBoxState } from "../../../reducers/source-blackbox";

describe("blackbox", () => {
  it("should blackbox and unblackbox a source based on the current state of the source", async () => {
    const store = createStore({
      blackBox: async () => true,
      getSourceActorBreakableLines: async () => [],
    });
    const { dispatch, getState } = store;

    const fooSource = await dispatch(
      actions.newGeneratedSource(makeSource("foo"))
    );
    await dispatch(actions.toggleBlackBox(fooSource));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    let blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual([]);

    await dispatch(actions.toggleBlackBox(fooSource));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(false);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual(undefined);
  });

  it("should blackbox and unblackbox a source when explicilty specified", async () => {
    const store = createStore({
      blackBox: async () => true,
      getSourceActorBreakableLines: async () => [],
    });
    const { dispatch, getState } = store;

    const fooSource = await dispatch(
      actions.newGeneratedSource(makeSource("foo"))
    );

    // check the state before trying to blackbox
    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(false);

    let blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual(undefined);

    // should blackbox the whole source
    await dispatch(actions.toggleBlackBox(fooSource, true, []));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual([]);

    // should unblackbox the whole source
    await dispatch(actions.toggleBlackBox(fooSource, false, []));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(false);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual(undefined);
  });

  it("should blackbox and unblackbox lines in a source", async () => {
    const store = createStore({
      blackBox: async () => true,
      getSourceActorBreakableLines: async () => [],
    });
    const { dispatch, getState } = store;

    const fooSource = await dispatch(
      actions.newGeneratedSource(makeSource("foo"))
    );

    const range1 = {
      start: { line: 10, column: 3 },
      end: { line: 15, column: 4 },
    };

    const range2 = {
      start: { line: 5, column: 3 },
      end: { line: 7, column: 6 },
    };

    await dispatch(actions.toggleBlackBox(fooSource, true, [range1]));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    let blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual([range1]);

    // add new blackbox lines in the second range
    await dispatch(actions.toggleBlackBox(fooSource, true, [range2]));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    // ranges are stored asc order
    expect(blackboxRanges[fooSource.url]).toEqual([range2, range1]);

    // un-blackbox lines in the first range
    await dispatch(actions.toggleBlackBox(fooSource, false, [range1]));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual([range2]);

    // un-blackbox lines in the second range
    await dispatch(actions.toggleBlackBox(fooSource, false, [range2]));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(false);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual(undefined);
  });

  it("should undo blackboxed lines when whole source unblackboxed", async () => {
    const store = createStore({
      blackBox: async () => true,
      getSourceActorBreakableLines: async () => [],
    });
    const { dispatch, getState } = store;

    const fooSource = await dispatch(
      actions.newGeneratedSource(makeSource("foo"))
    );

    const range1 = {
      start: { line: 1, column: 5 },
      end: { line: 3, column: 4 },
    };

    const range2 = {
      start: { line: 5, column: 3 },
      end: { line: 7, column: 6 },
    };

    await dispatch(actions.toggleBlackBox(fooSource, true, [range1, range2]));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    let blackboxRanges = selectors.getBlackBoxRanges(getState());
    // The ranges are ordered in based on the lines & cols in ascending
    expect(blackboxRanges[fooSource.url]).toEqual([range2, range1]);

    // un-blackbox the whole source
    await dispatch(actions.toggleBlackBox(fooSource));

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(false);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual(undefined);
  });

  it("should restore the blackboxed state correctly debugger load", async () => {
    const mockAsyncStoreBlackBoxedRanges = {
      "http://localhost:8000/examples/foo": [
        {
          start: { line: 1, column: 5 },
          end: { line: 3, column: 4 },
        },
      ],
    };

    function loadInitialState() {
      const blackboxedRanges = mockAsyncStoreBlackBoxedRanges;
      return {
        sourceBlackBox: initialSourceBlackBoxState({ blackboxedRanges }),
      };
    }
    const store = createStore(
      {
        blackBox: async () => true,
        getSourceActorBreakableLines: async () => [],
      },
      loadInitialState()
    );
    const { dispatch, getState } = store;

    const fooSource = await dispatch(
      actions.newGeneratedSource(makeSource("foo"))
    );

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    const blackboxRanges = selectors.getBlackBoxRanges(getState());
    const mockFooSourceRange = mockAsyncStoreBlackBoxedRanges[fooSource.url];
    expect(blackboxRanges[fooSource.url]).toEqual(mockFooSourceRange);
  });

  it("should unblackbox lines after blackboxed state has been restored", async () => {
    const mockAsyncStoreBlackBoxedRanges = {
      "http://localhost:8000/examples/foo": [
        {
          start: { line: 1, column: 5 },
          end: { line: 3, column: 4 },
        },
      ],
    };

    function loadInitialState() {
      const blackboxedRanges = mockAsyncStoreBlackBoxedRanges;
      return {
        sourceBlackBox: initialSourceBlackBoxState({ blackboxedRanges }),
      };
    }
    const store = createStore(
      {
        blackBox: async () => true,
        getSourceActorBreakableLines: async () => [],
      },
      loadInitialState()
    );
    const { dispatch, getState } = store;

    const fooSource = await dispatch(
      actions.newGeneratedSource(makeSource("foo"))
    );

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(true);

    let blackboxRanges = selectors.getBlackBoxRanges(getState());
    const mockFooSourceRange = mockAsyncStoreBlackBoxedRanges[fooSource.url];
    expect(blackboxRanges[fooSource.url]).toEqual(mockFooSourceRange);

    //unblackbox the blackboxed line
    await dispatch(
      actions.toggleBlackBox(fooSource, false, mockFooSourceRange)
    );

    expect(selectors.isSourceBlackBoxed(getState(), fooSource)).toEqual(false);

    blackboxRanges = selectors.getBlackBoxRanges(getState());
    expect(blackboxRanges[fooSource.url]).toEqual(undefined);
  });
});
