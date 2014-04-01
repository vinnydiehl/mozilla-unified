/* -*- Mode: javascript; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * The ElementManager manages DOM references and interactions with elements.
 * According to the WebDriver spec (http://code.google.com/p/selenium/wiki/JsonWireProtocol), the
 * server sends the client an element reference, and maintains the map of reference to element.
 * The client uses this reference when querying/interacting with the element, and the
 * server uses maps this reference to the actual element when it executes the command.
 */

this.EXPORTED_SYMBOLS = [
  "ElementManager",
  "CLASS_NAME",
  "SELECTOR",
  "ID",
  "NAME",
  "LINK_TEXT",
  "PARTIAL_LINK_TEXT",
  "TAG",
  "XPATH"
];

const DOCUMENT_POSITION_DISCONNECTED = 1;

let uuidGen = Components.classes["@mozilla.org/uuid-generator;1"]
             .getService(Components.interfaces.nsIUUIDGenerator);

this.CLASS_NAME = "class name";
this.SELECTOR = "css selector";
this.ID = "id";
this.NAME = "name";
this.LINK_TEXT = "link text";
this.PARTIAL_LINK_TEXT = "partial link text";
this.TAG = "tag name";
this.XPATH = "xpath";

function ElementException(msg, num, stack) {
  this.message = msg;
  this.code = num;
  this.stack = stack;
}

this.ElementManager = function ElementManager(notSupported) {
  this.seenItems = {};
  this.timer = Components.classes["@mozilla.org/timer;1"].createInstance(Components.interfaces.nsITimer);
  this.elementStrategies = [CLASS_NAME, SELECTOR, ID, NAME, LINK_TEXT, PARTIAL_LINK_TEXT, TAG, XPATH];
  for (let i = 0; i < notSupported.length; i++) {
    this.elementStrategies.splice(this.elementStrategies.indexOf(notSupported[i]), 1);
  }
}

ElementManager.prototype = {
  /**
   * Reset values
   */
  reset: function EM_clear() {
    this.seenItems = {};
  },

  /**
  * Add element to list of seen elements
  *
  * @param nsIDOMElement element
  *        The element to add
  *
  * @return string
  *        Returns the server-assigned reference ID
  */
  addToKnownElements: function EM_addToKnownElements(element) {
    for (let i in this.seenItems) {
      let foundEl = null;
      try {
        foundEl = this.seenItems[i].get();
      }
      catch(e) {}
      if (foundEl) {
        if (XPCNativeWrapper(foundEl) == XPCNativeWrapper(element)) {
          return i;
        }
      }
      else {
        //cleanup reference to GC'd element
        delete this.seenItems[i];
      }
    }
    var id = uuidGen.generateUUID().toString();
    this.seenItems[id] = Components.utils.getWeakReference(element);
    return id;
  },

  /**
   * Retrieve element from its unique ID
   *
   * @param String id
   *        The DOM reference ID
   * @param nsIDOMWindow win
   *        The window that contains the element
   *
   * @returns nsIDOMElement
   *        Returns the element or throws Exception if not found
   */
  getKnownElement: function EM_getKnownElement(id, win) {
    let el = this.seenItems[id];
    if (!el) {
      throw new ElementException("Element has not been seen before. Id given was " + id, 17, null);
    }
    try {
      el = el.get();
    }
    catch(e) {
      el = null;
      delete this.seenItems[id];
    }
    // use XPCNativeWrapper to compare elements; see bug 834266
    let wrappedWin = XPCNativeWrapper(win);
    if (!el ||
        !(XPCNativeWrapper(el).ownerDocument == wrappedWin.document) ||
        (XPCNativeWrapper(el).compareDocumentPosition(wrappedWin.document.documentElement) &
         DOCUMENT_POSITION_DISCONNECTED)) {
      throw new ElementException("The element reference is stale. Either the element " +
                                 "is no longer attached to the DOM or the page has been refreshed.", 10, null);
    }
    return el;
  },

  /**
   * Convert values to primitives that can be transported over the
   * Marionette protocol.
   *
   * This function implements the marshaling algorithm defined in the
   * WebDriver specification:
   *
   *     https://dvcs.w3.org/hg/webdriver/raw-file/tip/webdriver-spec.html#synchronous-javascript-execution
   *
   * @param object val
   *        object to be marshaled
   *
   * @return object
   *         Returns a JSON primitive or Object
   */
  wrapValue: function EM_wrapValue(val) {
    let result = null;

    switch (typeof(val)) {
      case "undefined":
        result = null;
        break;

      case "string":
      case "number":
      case "boolean":
        result = val;
        break;

      case "object":
        let type = Object.prototype.toString.call(val);
        if (type == "[object Array]" ||
            type == "[object NodeList]") {
          result = [];
          for (let i = 0; i < val.length; ++i) {
            result.push(this.wrapValue(val[i]));
          }
        }
        else if (val == null) {
          result = null;
        }
        else if (val.nodeType == 1) {
          result = {'ELEMENT': this.addToKnownElements(val)};
        }
        else {
          result = {};
          for (let prop in val) {
            result[prop] = this.wrapValue(val[prop]);
          }
        }
        break;
    }

    return result;
  },

  /**
   * Convert any ELEMENT references in 'args' to the actual elements
   *
   * @param object args
   *        Arguments passed in by client
   * @param nsIDOMWindow win
   *        The window that contains the elements
   *
   * @returns object
   *        Returns the objects passed in by the client, with the
   *        reference IDs replaced by the actual elements.
   */
  convertWrappedArguments: function EM_convertWrappedArguments(args, win) {
    let converted;
    switch (typeof(args)) {
      case 'number':
      case 'string':
      case 'boolean':
        converted = args;
        break;
      case 'object':
        if (args == null) {
          converted = null;
        }
        else if (Object.prototype.toString.call(args) == '[object Array]') {
          converted = [];
          for (let i in args) {
            converted.push(this.convertWrappedArguments(args[i], win));
          }
        }
        else if (typeof(args['ELEMENT'] === 'string') &&
                 args.hasOwnProperty('ELEMENT')) {
          converted = this.getKnownElement(args['ELEMENT'],  win);
          if (converted == null)
            throw new ElementException("Unknown element: " + args['ELEMENT'], 500, null);
        }
        else {
          converted = {};
          for (let prop in args) {
            converted[prop] = this.convertWrappedArguments(args[prop], win);
          }
        }
        break;
    }
    return converted;
  },

  /*
   * Execute* helpers
   */

  /**
   * Return an object with any namedArgs applied to it. Used
   * to let clients use given names when refering to arguments
   * in execute calls, instead of using the arguments list.
   *
   * @param object args
   *        list of arguments being passed in
   *
   * @return object
   *        If '__marionetteArgs' is in args, then
   *        it will return an object with these arguments
   *        as its members.
   */
  applyNamedArgs: function EM_applyNamedArgs(args) {
    namedArgs = {};
    args.forEach(function(arg) {
      if (typeof(arg['__marionetteArgs']) === 'object') {
        for (let prop in arg['__marionetteArgs']) {
          namedArgs[prop] = arg['__marionetteArgs'][prop];
        }
      }
    });
    return namedArgs;
  },

  /**
   * Find an element or elements starting at the document root or
   * given node, using the given search strategy. Search
   * will continue until the search timelimit has been reached.
   *
   * @param nsIDOMWindow win
   *        The window to search in
   * @param object values
   *        The 'using' member of values will tell us which search
   *        method to use. The 'value' member tells us the value we
   *        are looking for.
   *        If this object has an 'element' member, this will be used
   *        as the start node instead of the document root
   *        If this object has a 'time' member, this number will be
   *        used to see if we have hit the search timelimit.
   * @param function on_success
   *        The notification callback used when we are returning successfully.
   * @param function on_error
            The callback to invoke when an error occurs.
   * @param boolean all
   *        If true, all found elements will be returned.
   *        If false, only the first element will be returned.
   *
   * @return nsIDOMElement or list of nsIDOMElements
   *        Returns the element(s) by calling the on_success function.
   */
  find: function EM_find(win, values, searchTimeout, on_success, on_error, all, command_id) {
    let startTime = values.time ? values.time : new Date().getTime();
    let startNode = (values.element != undefined) ?
                    this.getKnownElement(values.element, win) : win.document;
    if (this.elementStrategies.indexOf(values.using) < 0) {
      throw new ElementException("No such strategy.", 17, null);
    }
    let found = all ? this.findElements(values.using, values.value, win.document, startNode) :
                      this.findElement(values.using, values.value, win.document, startNode);
    if (found) {
      let type = Object.prototype.toString.call(found);
      if ((type == '[object Array]') || (type == '[object HTMLCollection]') || (type == '[object NodeList]')) {
        let ids = []
        for (let i = 0 ; i < found.length ; i++) {
          ids.push(this.addToKnownElements(found[i]));
        }
        on_success(ids, command_id);
      }
      else {
        let id = this.addToKnownElements(found);
        on_success({'ELEMENT':id}, command_id);
      }
      return;
    } else {
      if (!searchTimeout || new Date().getTime() - startTime > searchTimeout) {
        on_error("Unable to locate element: " + values.value, 7, null, command_id);
      } else {
        values.time = startTime;
        this.timer.initWithCallback(this.find.bind(this, win, values,
                                                   searchTimeout,
                                                   on_success, on_error, all,
                                                   command_id),
                                    100,
                                    Components.interfaces.nsITimer.TYPE_ONE_SHOT);
      }
    }
  },

  /**
   * Find a value by XPATH
   *
   * @param nsIDOMElement root
   *        Document root
   * @param string value
   *        XPATH search string
   * @param nsIDOMElement node
   *        start node
   *
   * @return nsIDOMElement
   *        returns the found element
   */
  findByXPath: function EM_findByXPath(root, value, node) {
    return root.evaluate(value, node, null,
            Components.interfaces.nsIDOMXPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
  },

  /**
   * Find values by XPATH
   *
   * @param nsIDOMElement root
   *        Document root
   * @param string value
   *        XPATH search string
   * @param nsIDOMElement node
   *        start node
   *
   * @return object
   *        returns a list of found nsIDOMElements
   */
  findByXPathAll: function EM_findByXPathAll(root, value, node) {
    let values = root.evaluate(value, node, null,
                      Components.interfaces.nsIDOMXPathResult.ORDERED_NODE_ITERATOR_TYPE, null);
    let elements = [];
    let element = values.iterateNext();
    while (element) {
      elements.push(element);
      element = values.iterateNext();
    }
    return elements;
  },

  /**
   * Helper method to find. Finds one element using find's criteria
   *
   * @param string using
   *        String identifying which search method to use
   * @param string value
   *        Value to look for
   * @param nsIDOMElement rootNode
   *        Document root
   * @param nsIDOMElement startNode
   *        Node from which we start searching
   *
   * @return nsIDOMElement
   *        Returns found element or throws Exception if not found
   */
  findElement: function EM_findElement(using, value, rootNode, startNode) {
    let element;
    switch (using) {
      case ID:
        element = startNode.getElementById ?
                  startNode.getElementById(value) :
                  this.findByXPath(rootNode, './/*[@id="' + value + '"]', startNode);
        break;
      case NAME:
        element = startNode.getElementsByName ?
                  startNode.getElementsByName(value)[0] :
                  this.findByXPath(rootNode, './/*[@name="' + value + '"]', startNode);
        break;
      case CLASS_NAME:
        element = startNode.getElementsByClassName(value)[0]; //works for >=FF3
        break;
      case TAG:
        element = startNode.getElementsByTagName(value)[0]; //works for all elements
        break;
      case XPATH:
        element = this.findByXPath(rootNode, value, startNode);
        break;
      case LINK_TEXT:
      case PARTIAL_LINK_TEXT:
        let allLinks = startNode.getElementsByTagName('A');
        for (let i = 0; i < allLinks.length && !element; i++) {
          let text = allLinks[i].text;
          if (PARTIAL_LINK_TEXT == using) {
            if (text.indexOf(value) != -1) {
              element = allLinks[i];
            }
          } else if (text == value) {
            element = allLinks[i];
          }
        }
        break;
      case SELECTOR:
        element = startNode.querySelector(value);
        break;
      default:
        throw new ElementException("No such strategy", 500, null);
    }
    return element;
  },

  /**
   * Helper method to find. Finds all element using find's criteria
   *
   * @param string using
   *        String identifying which search method to use
   * @param string value
   *        Value to look for
   * @param nsIDOMElement rootNode
   *        Document root
   * @param nsIDOMElement startNode
   *        Node from which we start searching
   *
   * @return nsIDOMElement
   *        Returns found elements or throws Exception if not found
   */
  findElements: function EM_findElements(using, value, rootNode, startNode) {
    let elements = [];
    switch (using) {
      case ID:
        value = './/*[@id="' + value + '"]';
      case XPATH:
        elements = this.findByXPathAll(rootNode, value, startNode);
        break;
      case NAME:
        elements = startNode.getElementsByName ?
                   startNode.getElementsByName(value) :
                   this.findByXPathAll(rootNode, './/*[@name="' + value + '"]', startNode);
        break;
      case CLASS_NAME:
        elements = startNode.getElementsByClassName(value);
        break;
      case TAG:
        elements = startNode.getElementsByTagName(value);
        break;
      case LINK_TEXT:
      case PARTIAL_LINK_TEXT:
        let allLinks = rootNode.getElementsByTagName('A');
        for (let i = 0; i < allLinks.length; i++) {
          let text = allLinks[i].text;
          if (PARTIAL_LINK_TEXT == using) {
            if (text.indexOf(value) != -1) {
              elements.push(allLinks[i]);
            }
          } else if (text == value) {
            elements.push(allLinks[i]);
          }
        }
        break;
      case SELECTOR:
        elements = Array.slice(startNode.querySelectorAll(value));
        break;
      default:
        throw new ElementException("No such strategy", 500, null);
    }
    return elements;
  },
}
