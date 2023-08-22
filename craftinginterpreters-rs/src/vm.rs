use crate::{
    chunk::{Chunk, Instruction, Value},
    error::LoxError,
    strings::LoxString,
};
use std::collections::HashMap;

pub struct Vm {
    chunk: Chunk,
    ip: usize,
    stack: Vec<Value>,
    globals: HashMap<LoxString, Value>,
}

impl Vm {
    pub fn new(chunk: Chunk) -> Self {
        Self {
            chunk,
            ip: 0,
            stack: Vec::with_capacity(256),
            globals: HashMap::new(),
        }
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

    pub fn run(&mut self) -> Result<(), LoxError> {
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
                            let s_a = self.chunk.strings.lookup(*a);
                            let s_b = self.chunk.strings.lookup(*b);
                            let result = format!("{}{}", s_a, s_b);
                            let s = self.chunk.strings.intern_owned(result);
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
                    if let Value::String(s) = self.chunk.read_constant(index) {
                        let value = self.pop();
                        self.globals.insert(s, value);
                    }
                }
                Instruction::Divide => self.binary_op(|a, b| a / b, |n| Value::Number(n))?,
                Instruction::Equal => {
                    let a = self.pop();
                    let b = self.pop();
                    self.push(Value::Bool(a == b));
                }
                Instruction::False => self.push(Value::Bool(false)),
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
                        println!("{}", self.chunk.strings.lookup(idx));
                    } else {
                        println!("{}", value);
                    }
                }
                Instruction::Return => {
                    return Ok(());
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
