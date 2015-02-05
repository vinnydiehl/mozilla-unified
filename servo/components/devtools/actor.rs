/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/// General actor system infrastructure.

use std::any::Any;
use std::collections::HashMap;
use std::cell::{Cell, RefCell};
use std::intrinsics::TypeId;
use std::io::TcpStream;
use std::mem::{replace, transmute};
use std::raw::TraitObject;
use serialize::json;

/// A common trait for all devtools actors that encompasses an immutable name
/// and the ability to process messages that are directed to particular actors.
/// TODO: ensure the name is immutable
pub trait Actor: Any {
    fn handle_message(&self,
                      registry: &ActorRegistry,
                      msg_type: &String,
                      msg: &json::Object,
                      stream: &mut TcpStream) -> Result<bool, ()>;
    fn name(&self) -> String;
}

impl Actor {
    /// Returns true if the boxed type is the same as `T`
    #[inline]
    pub fn is<T: 'static>(&self) -> bool {
        // Get TypeId of the type this function is instantiated with
        let t = TypeId::of::<T>();

        // Get TypeId of the type in the trait object
        let boxed = self.get_type_id();

        // Compare both TypeIds on equality
        t == boxed
    }

    /// Returns some reference to the boxed value if it is of type `T`, or
    /// `None` if it isn't.
    #[inline]
    pub fn downcast_ref<T: 'static>(&self) -> Option<&T> {
        if self.is::<T>() {
            unsafe {
                // Get the raw representation of the trait object
                let to: TraitObject = transmute(self);

                // Extract the data pointer
                Some(transmute(to.data))
            }
        } else {
            None
        }
    }

    /// Returns some mutable reference to the boxed value if it is of type `T`, or
    /// `None` if it isn't.
    #[inline]
    pub fn downcast_mut<T: 'static>(&mut self) -> Option<&mut T> {
        if self.is::<T>() {
            unsafe {
                // Get the raw representation of the trait object
                let to: TraitObject = transmute(self);

                // Extract the data pointer
                Some(transmute(to.data))
            }
        } else {
            None
        }
    }
}

/// A list of known, owned actors.
pub struct ActorRegistry {
    actors: HashMap<String, Box<Actor+Send>>,
    new_actors: RefCell<Vec<Box<Actor+Send>>>,
    script_actors: RefCell<HashMap<String, String>>,
    next: Cell<u32>,
}

impl ActorRegistry {
    /// Create an empty registry.
    pub fn new() -> ActorRegistry {
        ActorRegistry {
            actors: HashMap::new(),
            new_actors: RefCell::new(vec!()),
            script_actors: RefCell::new(HashMap::new()),
            next: Cell::new(0),
        }
    }

    pub fn register_script_actor(&self, script_id: String, actor: String) {
        println!("registering {} ({})", actor.as_slice(), script_id.as_slice());
        let mut script_actors = self.script_actors.borrow_mut();
        script_actors.insert(script_id, actor);
    }

    pub fn script_to_actor(&self, script_id: String) -> String {
        if script_id.as_slice() == "" {
            return "".to_string();
        }
        self.script_actors.borrow().get(&script_id).unwrap().to_string()
    }

    pub fn script_actor_registered(&self, script_id: String) -> bool {
        self.script_actors.borrow().contains_key(&script_id)
    }

    pub fn actor_to_script(&self, actor: String) -> String {
        for (key, value) in self.script_actors.borrow().iter() {
            println!("checking {}", value.as_slice());
            if value.as_slice() == actor.as_slice() {
                return key.to_string();
            }
        }
        panic!("couldn't find actor named {}", actor)
    }

    /// Create a unique name based on a monotonically increasing suffix
    pub fn new_name(&self, prefix: &str) -> String {
        let suffix = self.next.get();
        self.next.set(suffix + 1);
        format!("{}{}", prefix, suffix)
    }

    /// Add an actor to the registry of known actors that can receive messages.
    pub fn register(&mut self, actor: Box<Actor+Send>) {
        self.actors.insert(actor.name().to_string(), actor);
    }

    pub fn register_later(&self, actor: Box<Actor+Send>) {
        let mut actors = self.new_actors.borrow_mut();
        actors.push(actor);
    }

    /// Find an actor by registered name
    pub fn find<'a, T: 'static>(&'a self, name: &str) -> &'a T {
        let actor = self.actors.get(&name.to_string()).unwrap();
        actor.downcast_ref::<T>().unwrap()
    }

    /// Find an actor by registered name
    pub fn find_mut<'a, T: 'static>(&'a mut self, name: &str) -> &'a mut T {
        let actor = self.actors.get_mut(&name.to_string()).unwrap();
        actor.downcast_mut::<T>().unwrap()
    }

    /// Attempt to process a message as directed by its `to` property. If the actor is not
    /// found or does not indicate that it knew how to process the message, ignore the failure.
    pub fn handle_message(&mut self,
                          msg: &json::Object,
                          stream: &mut TcpStream)
                          -> Result<(), ()> {
        let to = msg.get("to").unwrap().as_string().unwrap();
        match self.actors.get(&to.to_string()) {
            None => println!("message received for unknown actor \"{}\"", to),
            Some(actor) => {
                let msg_type = msg.get("type").unwrap().as_string().unwrap();
                if !try!(actor.handle_message(self, &msg_type.to_string(), msg, stream)) {
                    println!("unexpected message type \"{}\" found for actor \"{}\"",
                             msg_type, to);
                }
            }
        }
        let new_actors = replace(&mut *self.new_actors.borrow_mut(), vec!());
        for actor in new_actors.into_iter() {
            self.actors.insert(actor.name().to_string(), actor);
        }
        Ok(())
    }
}
