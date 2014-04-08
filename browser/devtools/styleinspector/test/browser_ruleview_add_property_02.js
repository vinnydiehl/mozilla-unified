/* vim: set ft=javascript ts=2 et sw=2 tw=80: */
/* Any copyright is dedicated to the Public Domain.
 http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

// Test all sorts of additions and updates of properties in the rule-view
// FIXME: TO BE SPLIT IN *MANY* SMALLER TESTS

let test = asyncTest(function*() {
  yield addTab("data:text/html;charset=utf-8,browser_ruleview_ui.js");
  let {toolbox, inspector, view} = yield openRuleView();

  info("Creating the test document");
  let style = "" +
    "#testid {" +
    "  background-color: blue;" +
    "}" +
    ".testclass, .unmatched {" +
    "  background-color: green;" +
    "}";
  let styleNode = addStyle(content.document, style);
  content.document.body.innerHTML = "<div id='testid' class='testclass'>Styled Node</div>" +
                                    "<div id='testid2'>Styled Node</div>";

  yield testCreateNew(inspector, view);
  yield inspector.once("inspector-updated");
});

function* testCreateNew(inspector, ruleView) {
  // Create a new property.
  let elementRuleEditor = ruleView.element.children[0]._ruleEditor;
  let editor = yield focusEditableField(elementRuleEditor.closeBrace);

  is(inplaceEditor(elementRuleEditor.newPropSpan), editor,
    "Next focused editor should be the new property editor.");

  let input = editor.input;

  ok(input.selectionStart === 0 && input.selectionEnd === input.value.length,
    "Editor contents are selected.");

  // Try clicking on the editor's input again, shouldn't cause trouble (see bug 761665).
  EventUtils.synthesizeMouse(input, 1, 1, {}, ruleView.doc.defaultView);
  input.select();

  info("Entering the property name");
  input.value = "background-color";

  info("Pressing RETURN and waiting for the value field focus");
  let onFocus = once(elementRuleEditor.element, "focus", true);
  EventUtils.sendKey("return", ruleView.doc.defaultView);
  yield onFocus;
  yield elementRuleEditor.rule._applyingModifications;

  editor = inplaceEditor(ruleView.doc.activeElement);

  is(elementRuleEditor.rule.textProps.length,  1, "Should have created a new text property.");
  is(elementRuleEditor.propertyList.children.length, 1, "Should have created a property editor.");
  let textProp = elementRuleEditor.rule.textProps[0];
  is(editor, inplaceEditor(textProp.editor.valueSpan), "Should be editing the value span now.");

  editor.input.value = "purple";
  let onBlur = once(editor.input, "blur");
  editor.input.blur();
  yield onBlur;
  yield elementRuleEditor.rule._applyingModifications;

  is(textProp.value, "purple", "Text prop should have been changed.");
}
