/* automatically generated by rust-bindgen */

use gecko_style_structs::nsStyleFont;
use gecko_style_structs::nsStyleColor;
use gecko_style_structs::nsStyleList;
use gecko_style_structs::nsStyleText;
use gecko_style_structs::nsStyleVisibility;
use gecko_style_structs::nsStyleUserInterface;
use gecko_style_structs::nsStyleTableBorder;
use gecko_style_structs::nsStyleSVG;
use gecko_style_structs::nsStyleVariables;
use gecko_style_structs::nsStyleBackground;
use gecko_style_structs::nsStylePosition;
use gecko_style_structs::nsStyleTextReset;
use gecko_style_structs::nsStyleDisplay;
use gecko_style_structs::nsStyleContent;
use gecko_style_structs::nsStyleUIReset;
use gecko_style_structs::nsStyleTable;
use gecko_style_structs::nsStyleMargin;
use gecko_style_structs::nsStylePadding;
use gecko_style_structs::nsStyleBorder;
use gecko_style_structs::nsStyleOutline;
use gecko_style_structs::nsStyleXUL;
use gecko_style_structs::nsStyleSVGReset;
use gecko_style_structs::nsStyleColumn;
use gecko_style_structs::nsStyleEffects;

pub enum nsIAtom { }
pub enum nsINode { }
pub type RawGeckoNode = nsINode;
pub enum Element { }
pub type RawGeckoElement = Element;
pub enum nsIDocument { }
pub type RawGeckoDocument = nsIDocument;
pub enum ServoNodeData { }
pub enum ServoComputedValues { }
pub enum RawServoStyleSheet { }
pub enum RawServoStyleSet { }
extern "C" {
    pub fn Gecko_ChildrenCount(node: *mut RawGeckoNode) -> u32;
    pub fn Gecko_NodeIsElement(node: *mut RawGeckoNode) -> bool;
    pub fn Gecko_GetParentNode(node: *mut RawGeckoNode) -> *mut RawGeckoNode;
    pub fn Gecko_GetFirstChild(node: *mut RawGeckoNode) -> *mut RawGeckoNode;
    pub fn Gecko_GetLastChild(node: *mut RawGeckoNode) -> *mut RawGeckoNode;
    pub fn Gecko_GetPrevSibling(node: *mut RawGeckoNode) -> *mut RawGeckoNode;
    pub fn Gecko_GetNextSibling(node: *mut RawGeckoNode) -> *mut RawGeckoNode;
    pub fn Gecko_GetParentElement(element: *mut RawGeckoElement)
     -> *mut RawGeckoElement;
    pub fn Gecko_GetFirstChildElement(element: *mut RawGeckoElement)
     -> *mut RawGeckoElement;
    pub fn Gecko_GetLastChildElement(element: *mut RawGeckoElement)
     -> *mut RawGeckoElement;
    pub fn Gecko_GetPrevSiblingElement(element: *mut RawGeckoElement)
     -> *mut RawGeckoElement;
    pub fn Gecko_GetNextSiblingElement(element: *mut RawGeckoElement)
     -> *mut RawGeckoElement;
    pub fn Gecko_GetDocumentElement(document: *mut RawGeckoDocument)
     -> *mut RawGeckoElement;
    pub fn Gecko_ElementState(element: *mut RawGeckoElement) -> u8;
    pub fn Gecko_IsHTMLElementInHTMLDocument(element: *mut RawGeckoElement)
     -> bool;
    pub fn Gecko_IsLink(element: *mut RawGeckoElement) -> bool;
    pub fn Gecko_IsTextNode(node: *mut RawGeckoNode) -> bool;
    pub fn Gecko_IsVisitedLink(element: *mut RawGeckoElement) -> bool;
    pub fn Gecko_IsUnvisitedLink(element: *mut RawGeckoElement) -> bool;
    pub fn Gecko_IsRootElement(element: *mut RawGeckoElement) -> bool;
    pub fn Gecko_GetNodeData(node: *mut RawGeckoNode) -> *mut ServoNodeData;
    pub fn Gecko_SetNodeData(node: *mut RawGeckoNode,
                             data: *mut ServoNodeData);
    pub fn Servo_DropNodeData(data: *mut ServoNodeData);
    pub fn Servo_StylesheetFromUTF8Bytes(bytes: *const u8, length: u32)
     -> *mut RawServoStyleSheet;
    pub fn Servo_AddRefStyleSheet(sheet: *mut RawServoStyleSheet);
    pub fn Servo_ReleaseStyleSheet(sheet: *mut RawServoStyleSheet);
    pub fn Servo_AppendStyleSheet(sheet: *mut RawServoStyleSheet,
                                  set: *mut RawServoStyleSet);
    pub fn Servo_PrependStyleSheet(sheet: *mut RawServoStyleSheet,
                                   set: *mut RawServoStyleSet);
    pub fn Servo_RemoveStyleSheet(sheet: *mut RawServoStyleSheet,
                                  set: *mut RawServoStyleSet);
    pub fn Servo_StyleSheetHasRules(sheet: *mut RawServoStyleSheet) -> bool;
    pub fn Servo_InitStyleSet() -> *mut RawServoStyleSet;
    pub fn Servo_DropStyleSet(set: *mut RawServoStyleSet);
    pub fn Servo_GetComputedValues(element: *mut RawGeckoElement)
     -> *mut ServoComputedValues;
    pub fn Servo_GetComputedValuesForAnonymousBox(parentStyleOrNull:
                                                      *mut ServoComputedValues,
                                                  pseudoTag: *mut nsIAtom,
                                                  set: *mut RawServoStyleSet)
     -> *mut ServoComputedValues;
    pub fn Servo_AddRefComputedValues(arg1: *mut ServoComputedValues);
    pub fn Servo_ReleaseComputedValues(arg1: *mut ServoComputedValues);
    pub fn Gecko_GetAttrAsUTF8(element: *mut RawGeckoElement, ns: *const u8,
                               name: *const u8, length: *mut u32)
     -> *const ::std::os::raw::c_char;
    pub fn Gecko_LocalName(element: *mut RawGeckoElement, length: *mut u32)
     -> *const u16;
    pub fn Gecko_Namespace(element: *mut RawGeckoElement, length: *mut u32)
     -> *const u16;
    pub fn Servo_RestyleDocument(doc: *mut RawGeckoDocument,
                                 set: *mut RawServoStyleSet);
    pub fn Gecko_Construct_nsStyleFont(ptr: *mut nsStyleFont);
    pub fn Gecko_CopyConstruct_nsStyleFont(ptr: *mut nsStyleFont,
                                           other: *const nsStyleFont);
    pub fn Gecko_Destroy_nsStyleFont(ptr: *mut nsStyleFont);
    pub fn Gecko_Construct_nsStyleColor(ptr: *mut nsStyleColor);
    pub fn Gecko_CopyConstruct_nsStyleColor(ptr: *mut nsStyleColor,
                                            other: *const nsStyleColor);
    pub fn Gecko_Destroy_nsStyleColor(ptr: *mut nsStyleColor);
    pub fn Gecko_Construct_nsStyleList(ptr: *mut nsStyleList);
    pub fn Gecko_CopyConstruct_nsStyleList(ptr: *mut nsStyleList,
                                           other: *const nsStyleList);
    pub fn Gecko_Destroy_nsStyleList(ptr: *mut nsStyleList);
    pub fn Gecko_Construct_nsStyleText(ptr: *mut nsStyleText);
    pub fn Gecko_CopyConstruct_nsStyleText(ptr: *mut nsStyleText,
                                           other: *const nsStyleText);
    pub fn Gecko_Destroy_nsStyleText(ptr: *mut nsStyleText);
    pub fn Gecko_Construct_nsStyleVisibility(ptr: *mut nsStyleVisibility);
    pub fn Gecko_CopyConstruct_nsStyleVisibility(ptr: *mut nsStyleVisibility,
                                                 other:
                                                     *const nsStyleVisibility);
    pub fn Gecko_Destroy_nsStyleVisibility(ptr: *mut nsStyleVisibility);
    pub fn Gecko_Construct_nsStyleUserInterface(ptr:
                                                    *mut nsStyleUserInterface);
    pub fn Gecko_CopyConstruct_nsStyleUserInterface(ptr:
                                                        *mut nsStyleUserInterface,
                                                    other:
                                                        *const nsStyleUserInterface);
    pub fn Gecko_Destroy_nsStyleUserInterface(ptr: *mut nsStyleUserInterface);
    pub fn Gecko_Construct_nsStyleTableBorder(ptr: *mut nsStyleTableBorder);
    pub fn Gecko_CopyConstruct_nsStyleTableBorder(ptr:
                                                      *mut nsStyleTableBorder,
                                                  other:
                                                      *const nsStyleTableBorder);
    pub fn Gecko_Destroy_nsStyleTableBorder(ptr: *mut nsStyleTableBorder);
    pub fn Gecko_Construct_nsStyleSVG(ptr: *mut nsStyleSVG);
    pub fn Gecko_CopyConstruct_nsStyleSVG(ptr: *mut nsStyleSVG,
                                          other: *const nsStyleSVG);
    pub fn Gecko_Destroy_nsStyleSVG(ptr: *mut nsStyleSVG);
    pub fn Gecko_Construct_nsStyleVariables(ptr: *mut nsStyleVariables);
    pub fn Gecko_CopyConstruct_nsStyleVariables(ptr: *mut nsStyleVariables,
                                                other:
                                                    *const nsStyleVariables);
    pub fn Gecko_Destroy_nsStyleVariables(ptr: *mut nsStyleVariables);
    pub fn Gecko_Construct_nsStyleBackground(ptr: *mut nsStyleBackground);
    pub fn Gecko_CopyConstruct_nsStyleBackground(ptr: *mut nsStyleBackground,
                                                 other:
                                                     *const nsStyleBackground);
    pub fn Gecko_Destroy_nsStyleBackground(ptr: *mut nsStyleBackground);
    pub fn Gecko_Construct_nsStylePosition(ptr: *mut nsStylePosition);
    pub fn Gecko_CopyConstruct_nsStylePosition(ptr: *mut nsStylePosition,
                                               other: *const nsStylePosition);
    pub fn Gecko_Destroy_nsStylePosition(ptr: *mut nsStylePosition);
    pub fn Gecko_Construct_nsStyleTextReset(ptr: *mut nsStyleTextReset);
    pub fn Gecko_CopyConstruct_nsStyleTextReset(ptr: *mut nsStyleTextReset,
                                                other:
                                                    *const nsStyleTextReset);
    pub fn Gecko_Destroy_nsStyleTextReset(ptr: *mut nsStyleTextReset);
    pub fn Gecko_Construct_nsStyleDisplay(ptr: *mut nsStyleDisplay);
    pub fn Gecko_CopyConstruct_nsStyleDisplay(ptr: *mut nsStyleDisplay,
                                              other: *const nsStyleDisplay);
    pub fn Gecko_Destroy_nsStyleDisplay(ptr: *mut nsStyleDisplay);
    pub fn Gecko_Construct_nsStyleContent(ptr: *mut nsStyleContent);
    pub fn Gecko_CopyConstruct_nsStyleContent(ptr: *mut nsStyleContent,
                                              other: *const nsStyleContent);
    pub fn Gecko_Destroy_nsStyleContent(ptr: *mut nsStyleContent);
    pub fn Gecko_Construct_nsStyleUIReset(ptr: *mut nsStyleUIReset);
    pub fn Gecko_CopyConstruct_nsStyleUIReset(ptr: *mut nsStyleUIReset,
                                              other: *const nsStyleUIReset);
    pub fn Gecko_Destroy_nsStyleUIReset(ptr: *mut nsStyleUIReset);
    pub fn Gecko_Construct_nsStyleTable(ptr: *mut nsStyleTable);
    pub fn Gecko_CopyConstruct_nsStyleTable(ptr: *mut nsStyleTable,
                                            other: *const nsStyleTable);
    pub fn Gecko_Destroy_nsStyleTable(ptr: *mut nsStyleTable);
    pub fn Gecko_Construct_nsStyleMargin(ptr: *mut nsStyleMargin);
    pub fn Gecko_CopyConstruct_nsStyleMargin(ptr: *mut nsStyleMargin,
                                             other: *const nsStyleMargin);
    pub fn Gecko_Destroy_nsStyleMargin(ptr: *mut nsStyleMargin);
    pub fn Gecko_Construct_nsStylePadding(ptr: *mut nsStylePadding);
    pub fn Gecko_CopyConstruct_nsStylePadding(ptr: *mut nsStylePadding,
                                              other: *const nsStylePadding);
    pub fn Gecko_Destroy_nsStylePadding(ptr: *mut nsStylePadding);
    pub fn Gecko_Construct_nsStyleBorder(ptr: *mut nsStyleBorder);
    pub fn Gecko_CopyConstruct_nsStyleBorder(ptr: *mut nsStyleBorder,
                                             other: *const nsStyleBorder);
    pub fn Gecko_Destroy_nsStyleBorder(ptr: *mut nsStyleBorder);
    pub fn Gecko_Construct_nsStyleOutline(ptr: *mut nsStyleOutline);
    pub fn Gecko_CopyConstruct_nsStyleOutline(ptr: *mut nsStyleOutline,
                                              other: *const nsStyleOutline);
    pub fn Gecko_Destroy_nsStyleOutline(ptr: *mut nsStyleOutline);
    pub fn Gecko_Construct_nsStyleXUL(ptr: *mut nsStyleXUL);
    pub fn Gecko_CopyConstruct_nsStyleXUL(ptr: *mut nsStyleXUL,
                                          other: *const nsStyleXUL);
    pub fn Gecko_Destroy_nsStyleXUL(ptr: *mut nsStyleXUL);
    pub fn Gecko_Construct_nsStyleSVGReset(ptr: *mut nsStyleSVGReset);
    pub fn Gecko_CopyConstruct_nsStyleSVGReset(ptr: *mut nsStyleSVGReset,
                                               other: *const nsStyleSVGReset);
    pub fn Gecko_Destroy_nsStyleSVGReset(ptr: *mut nsStyleSVGReset);
    pub fn Gecko_Construct_nsStyleColumn(ptr: *mut nsStyleColumn);
    pub fn Gecko_CopyConstruct_nsStyleColumn(ptr: *mut nsStyleColumn,
                                             other: *const nsStyleColumn);
    pub fn Gecko_Destroy_nsStyleColumn(ptr: *mut nsStyleColumn);
    pub fn Gecko_Construct_nsStyleEffects(ptr: *mut nsStyleEffects);
    pub fn Gecko_CopyConstruct_nsStyleEffects(ptr: *mut nsStyleEffects,
                                              other: *const nsStyleEffects);
    pub fn Gecko_Destroy_nsStyleEffects(ptr: *mut nsStyleEffects);
}
