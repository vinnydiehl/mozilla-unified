/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#![allow(unsafe_code)]

//! Implementation of `iterable<...>` and `iterable<..., ...>` WebIDL declarations.

use core::nonzero::NonZero;
use dom::bindings::codegen::Bindings::IterableIteratorBinding::IterableKeyAndValueResult;
use dom::bindings::codegen::Bindings::IterableIteratorBinding::IterableKeyOrValueResult;
use dom::bindings::error::Fallible;
use dom::bindings::global::GlobalRef;
use dom::bindings::js::{JS, Root};
use dom::bindings::reflector::{Reflector, Reflectable, reflect_dom_object};
use dom::bindings::trace::JSTraceable;
use js::conversions::ToJSValConvertible;
use js::jsapi::{JSContext, JSObject, MutableHandleValue, MutableHandleObject, HandleValue};
use js::jsval::UndefinedValue;
use std::cell::Cell;
use std::ptr;

/// The values that an iterator will iterate over.
#[derive(JSTraceable, HeapSizeOf)]
pub enum IteratorType {
    /// The keys of the iterable object.
    Keys,
    /// The values of the iterable object.
    Values,
    /// The keys and values of the iterable object combined.
    Entries,
}

/// A DOM object that can be iterated over using a pair value iterator.
pub trait Iterable {
    /// The type of the key of the iterator pair.
    type Key: ToJSValConvertible;
    /// The type of the value of the iterator pair.
    type Value: ToJSValConvertible;
    /// Return the number of entries that can be iterated over.
    fn get_iterable_length(&self) -> u32;
    /// Return the value at the provided index.
    fn get_value_at_index(&self, index: u32) -> Self::Value;
    /// Return the key at the provided index.
    fn get_key_at_index(&self, index: u32) -> Self::Key;
}

/// An iterator over the iterable entries of a given DOM interface.
//FIXME: #12811 prevents dom_struct with type parameters
//#[dom_struct]
#[must_root]
#[privatize]
#[derive(JSTraceable)]
#[derive(HeapSizeOf)]
pub struct IterableIterator<T: Reflectable + JSTraceable + Iterable> {
    reflector: Reflector,
    iterable: JS<T>,
    type_: IteratorType,
    index: Cell<u32>,
}

impl<T: Reflectable + JSTraceable + Iterable> Reflectable for IterableIterator<T> {
    fn reflector<'a>(&'a self) -> &'a Reflector {
        &self.reflector
    }
    fn init_reflector(&mut self, obj: *mut JSObject) {
        self.reflector.set_jsobject(obj);
    }
}

impl<T: Reflectable + JSTraceable + Iterable> ToJSValConvertible for IterableIterator<T> {
    #[allow(unsafe_code)]
    unsafe fn to_jsval(&self,
                       cx: *mut JSContext,
                       rval: MutableHandleValue) {
        let object = Reflectable::reflector(self).get_jsobject();
        object.to_jsval(cx, rval)
    }
}

impl<T: Reflectable + JSTraceable + Iterable> IterableIterator<T> {
    /// Create a new iterator instance for the provided iterable DOM interface.
    pub fn new(iterable: &T,
               type_: IteratorType,
               wrap: fn(*mut JSContext, GlobalRef, Box<IterableIterator<T>>)
                     -> Root<Self>) -> Root<Self> {
        let iterator = box IterableIterator {
            reflector: Reflector::new(),
            type_: type_,
            iterable: JS::from_ref(iterable),
            index: Cell::new(0),
        };
        let global = iterable.global();
        reflect_dom_object(iterator, global.r(), wrap)
    }

    /// Return the next value from the iterable object.
    #[allow(non_snake_case)]
    pub fn Next(&self, cx: *mut JSContext) -> Fallible<NonZero<*mut JSObject>> {
        let index = self.index.get();
        rooted!(in(cx) let mut value = UndefinedValue());
        rooted!(in(cx) let mut rval = ptr::null_mut());
        let result = if index >= self.iterable.get_iterable_length() {
            dict_return(cx, rval.handle_mut(), true, value.handle())
        } else {
            match self.type_ {
                IteratorType::Keys => {
                    unsafe {
                        self.iterable.get_key_at_index(index).to_jsval(cx, value.handle_mut());
                    }
                    dict_return(cx, rval.handle_mut(), false, value.handle())
                }
                IteratorType::Values => {
                    unsafe {
                        self.iterable.get_value_at_index(index).to_jsval(cx, value.handle_mut());
                    }
                    dict_return(cx, rval.handle_mut(), false, value.handle())
                }
                IteratorType::Entries => {
                    rooted!(in(cx) let mut key = UndefinedValue());
                    unsafe {
                        self.iterable.get_key_at_index(index).to_jsval(cx, key.handle_mut());
                        self.iterable.get_value_at_index(index).to_jsval(cx, value.handle_mut());
                    }
                    key_and_value_return(cx, rval.handle_mut(), key.handle(), value.handle())
                }
            }
        };
        self.index.set(index + 1);
        result.map(|_| {
            assert!(!rval.is_null());
            unsafe { NonZero::new(rval.get()) }
        })
    }
}

fn dict_return(cx: *mut JSContext,
               result: MutableHandleObject,
               done: bool,
               value: HandleValue) -> Fallible<()> {
    let mut dict = unsafe { IterableKeyOrValueResult::empty(cx) };
    dict.done = done;
    dict.value = value.get();
    rooted!(in(cx) let mut dict_value = UndefinedValue());
    unsafe {
        dict.to_jsval(cx, dict_value.handle_mut());
    }
    result.set(dict_value.to_object());
    Ok(())
}

fn key_and_value_return(cx: *mut JSContext,
                        result: MutableHandleObject,
                        key: HandleValue,
                        value: HandleValue) -> Fallible<()> {
    let mut dict = unsafe { IterableKeyAndValueResult::empty(cx) };
    dict.done = false;
    dict.value = Some(vec![key.get(), value.get()]);
    rooted!(in(cx) let mut dict_value = UndefinedValue());
    unsafe {
        dict.to_jsval(cx, dict_value.handle_mut());
    }
    result.set(dict_value.to_object());
    Ok(())
}
