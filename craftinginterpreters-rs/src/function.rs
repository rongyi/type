use crate::{chunk::Chunk, strings::LoxString};

pub enum FunctionType {
    Function,
    Script,
}

pub struct LoxFunction {
    pub arity: usize,
    pub chunk: Chunk,
    pub name: LoxString,
}

impl LoxFunction {
    pub fn new() -> Self {
        Self {
            arity: 0,
            chunk: Chunk::new(),
            name: 0,
        }
    }
}

#[derive(Default)]
pub struct Functions {
    functions: Vec<LoxFunction>,
}

pub type FunctionID = usize;

impl Functions {
    pub fn lookup(&self, id: FunctionID) -> &LoxFunction {
        &self.functions[id]
    }
    pub fn store(&mut self, function: LoxFunction) -> FunctionID {
        self.functions.push(function);
        self.functions.len() - 1
    }
}
