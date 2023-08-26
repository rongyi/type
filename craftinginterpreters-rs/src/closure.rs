use crate::function::FunctionID;

pub struct Closure {
    pub function: FunctionID,
}

impl Closure {
    pub fn new(function: FunctionID) -> Self {
        Closure { function }
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
