use std::cell::RefCell;
use std::rc::Rc;

use crate::chunk::Value;
use crate::function::FunctionID;

pub struct Closure {
    pub function: FunctionID,
    pub upvalues: Vec<Rc<RefCell<ObjUpvalue>>>,
}

pub struct ObjUpvalue {
    pub location: usize,
    pub closed: Option<Value>,
}

impl ObjUpvalue {
    pub fn new(location: usize) -> Self {
        ObjUpvalue {
            location,
            closed: None,
        }
    }
}

impl Closure {
    pub fn new(function: FunctionID) -> Self {
        Closure {
            function,
            upvalues: Vec::new(),
        }
    }
}

pub type ClosureID = usize;

#[derive(Default)]
pub struct Closures {
    closures: Vec<Closure>,
}

impl Closures {
    pub fn lookup(&self, id: ClosureID) -> &Closure {
        &self.closures[id]
    }

    pub fn store(&mut self, closure: Closure) -> ClosureID {
        self.closures.push(closure);
        self.closures.len() - 1
    }
}
