use crate::{
    chunk::{Chunk, Instruction, Value},
    compiler::Parser,
    error::LoxError,
    strings::{LoxString, Strings},
};
use std::collections::HashMap;

pub struct Vm {
    chunk: Chunk,
    ip: usize,
    stack: Vec<Value>,
    globals: HashMap<LoxString, Value>,
    strings: Strings,
}

impl Vm {
    pub fn new() -> Self {
        Self {
            chunk: Chunk::new(),
            ip: 0,
            stack: Vec::with_capacity(256),
            globals: HashMap::new(),
            strings: Strings::default(),
        }
    }
    pub fn interpret(&mut self, code: &str) -> Result<(), LoxError> {
        self.chunk = Chunk::new();
        let mut parser = Parser::new(code, &mut self.chunk, &mut self.strings);
        parser.compile()?;
        self.ip = 0;
        self.run()
    }

    fn push(&mut self, val: Value) {
        self.stack.push(val);
    }
    fn pop(&mut self) -> Value {
        self.stack.pop().expect("Empty stack")
    }

    fn peek(&self, n: usize) -> Value {
        let sz = self.stack.len();
        self.stack[sz - 1 - n].clone()
    }

    fn binary_op<T>(&mut self, f: fn(f64, f64) -> T, r: fn(T) -> Value) -> Result<(), LoxError> {
        let operands = (self.pop(), self.pop());
        match operands {
            (Value::Number(b), Value::Number(a)) => {
                self.push(r(f(a, b)));
                Ok(())
            }
            _ => {
                self.runtime_error("Operands must be numbers.");
                Err(LoxError::RuntimeError)
            }
        }
    }

    fn run(&mut self) -> Result<(), LoxError> {
        loop {
            let instruction = self.next_instruction();
            #[cfg(debug_assertions)]
            {
                for val in self.stack.iter() {
                    print!("[{}]", val);
                }
                println!("");
                #[cfg(debug_assertions)]
                self.chunk
                    .disassemble_instruction(&instruction, self.ip - 1);
            }
            match instruction {
                Instruction::Add => {
                    let (b, a) = (self.pop(), self.pop());
                    match (&a, &b) {
                        (Value::Number(a), Value::Number(b)) => {
                            self.push(Value::Number(a + b));
                        }
                        (Value::String(a), Value::String(b)) => {
                            let s_a = self.strings.lookup(*a);
                            let s_b = self.strings.lookup(*b);
                            let result = format!("{}{}", s_a, s_b);
                            let s = self.strings.intern_owned(result);
                            let value = Value::String(s);
                            self.push(value);
                        }
                        _ => {
                            self.push(a);
                            self.push(b);
                            self.runtime_error(
                                "Operands must be numbers or string at the same time.",
                            );
                            return Err(LoxError::RuntimeError);
                        }
                    }
                }
                Instruction::Constant(index) => {
                    let val = self.chunk.read_constant(index);
                    self.stack.push(val);
                }
                Instruction::DefineGlobal(index) => {
                    let s = self.chunk.read_string(index);
                    let value = self.pop();
                    // no string, only the intern index
                    self.globals.insert(s, value);
                }
                Instruction::Divide => self.binary_op(|a, b| a / b, |n| Value::Number(n))?,
                Instruction::Equal => {
                    let a = self.pop();
                    let b = self.pop();
                    self.push(Value::Bool(a == b));
                }
                Instruction::False => self.push(Value::Bool(false)),
                Instruction::GetGlobal(index) => {
                    // just get the intern index
                    let s = self.chunk.read_string(index);
                    match self.globals.get(&s) {
                        Some(&value) => self.push(value),
                        None => {
                            let name = self.strings.lookup(s);
                            let msg = format!("Undefined variable '{}'.", name);
                            self.runtime_error(&msg);
                            return Err(LoxError::RuntimeError);
                        }
                    }
                }
                Instruction::GetLocal(slot) => {
                    let value = self.stack[slot];
                    self.push(value);
                }
                Instruction::Greater => self.binary_op(|a, b| a > b, |n| Value::Bool(n))?,
                Instruction::Less => self.binary_op(|a, b| a < b, |n| Value::Bool(n))?,
                Instruction::Multiply => self.binary_op(|a, b| a * b, |n| Value::Number(n))?,
                Instruction::Negate => {
                    if let Value::Number(value) = self.peek(0) {
                        self.pop();
                        self.push(Value::Number(-value));
                    } else {
                        self.runtime_error("Operand must be a number");
                        return Err(LoxError::RuntimeError);
                    }
                }
                Instruction::Nil => self.push(Value::Nil),
                Instruction::Not => {
                    let value = self.pop();
                    self.push(Value::Bool(value.is_falsy()));
                }
                Instruction::Pop => {
                    self.pop();
                }
                Instruction::Print => {
                    let value = self.pop();
                    if let Value::String(idx) = value {
                        println!("{}", self.strings.lookup(idx));
                    } else {
                        println!("{}", value);
                    }
                }
                Instruction::SetGlobal(index) => {
                    let name = self.chunk.read_string(index);
                    let value = self.peek(0);
                    // means not exist name, so this is an error
                    if let None = self.globals.insert(name, value) {
                        // delete
                        self.globals.remove(&name);
                        let s = self.strings.lookup(name);
                        let msg = format!("Undefined variable '{}'.", s);
                        self.runtime_error(&msg);
                        return Err(LoxError::RuntimeError);
                    }
                }
                Instruction::Return => {
                    return Ok(());
                }
                Instruction::SetLocal(slot) => {
                    let value = self.peek(0);
                    self.stack[slot] = value;
                }
                Instruction::Substract => self.binary_op(|a, b| a - b, |n| Value::Number(n))?,
                Instruction::True => self.push(Value::Bool(true)),
            }
        }
    }

    fn next_instruction(&mut self) -> Instruction {
        let instruction = self.chunk.code[self.ip];
        self.ip += 1;
        instruction
    }

    fn runtime_error(&mut self, msg: &str) {
        eprintln!("{}", msg);
        let line = self.chunk.lines[self.ip - 1];
        eprintln!("[line {}] in script", line);
    }
}
