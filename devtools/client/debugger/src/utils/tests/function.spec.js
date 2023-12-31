/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at <http://mozilla.org/MPL/2.0/>. */

import { findFunctionText } from "../function";

import { getFunctionSymbols } from "../../workers/parser/getSymbols";
import { populateOriginalSource } from "../../workers/parser/tests/helpers";

describe("function", () => {
  describe("findFunctionText", () => {
    it("finds function", () => {
      const source = populateOriginalSource("func");
      const functions = getFunctionSymbols(source.id);
      const text = findFunctionText(14, source, source.content, { functions });
      expect(text).toMatchSnapshot();
    });

    it("finds function signature", () => {
      const source = populateOriginalSource("func");
      const functions = getFunctionSymbols(source.id);

      const text = findFunctionText(13, source, source.content, { functions });
      expect(text).toMatchSnapshot();
    });

    it("misses function closing brace", () => {
      const source = populateOriginalSource("func");
      const functions = getFunctionSymbols(source.id);

      const text = findFunctionText(15, source, source.content, { functions });

      // TODO: we should try and match the closing bracket.
      expect(text).toEqual(null);
    });

    it("finds property function", () => {
      const source = populateOriginalSource("func");
      const functions = getFunctionSymbols(source.id);

      const text = findFunctionText(29, source, source.content, { functions });
      expect(text).toMatchSnapshot();
    });

    it("finds class function", () => {
      const source = populateOriginalSource("func");
      const functions = getFunctionSymbols(source.id);

      const text = findFunctionText(33, source, source.content, { functions });
      expect(text).toMatchSnapshot();
    });

    it("cant find function", () => {
      const source = populateOriginalSource("func");
      const functions = getFunctionSymbols(source.id);

      const text = findFunctionText(20, source, source.content, { functions });
      expect(text).toEqual(null);
    });
  });
});
