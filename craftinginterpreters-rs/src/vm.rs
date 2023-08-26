use crate::{
    chunk::{Instruction, Value},
    compiler::Parser,
    error::LoxError,
    function::{Functions, LoxFunction},
    strings::{LoxString, Strings},
};
use std::collections::HashMap;

struct CallFrame {
    function: LoxFunction,
    ip: usize,
    slot: usize, // what's this? base?
}

impl CallFrame {
    fn new(function: LoxFunction) -> Self {
        CallFrame {
            function,
            ip: 0,
            slot: 0,
        }
    }
}

const MAX_FRAMES: usize = 64;
const STACK_SIZE: usize = MAX_FRAMES * (std::u8::MAX as usize) + 1;

pub struct Vm {
    //chunk: Chunk,
    frames: Vec<CallFrame>,
    stack: Vec<Value>,
    globals: HashMap<LoxString, Value>,
    strings: Strings,
    functions: Functions,
}

impl Vm {
    pub fn new() -> Self {
        Self {
            frames: Vec::with_capacity(MAX_FRAMES),
            stack: Vec::with_capacity(256),
            globals: HashMap::new(),
            strings: Strings::default(),
            functions: Functions::default(),
        }
    }

    pub fn interpret(&mut self, code: &str) -> Result<(), LoxError> {
        let parser = Parser::new(code, &mut self.strings, &mut self.functions);
        let function = parser.compile()?;
        self.frames.push(CallFrame::new(function));
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
                let frame = match self.frames.last() {
                    Some(f) => f,
                    None => panic!("No frame available"),
                };
                Vm::runtime_error(frame, "Operands must be numbers.");
                Err(LoxError::RuntimeError)
            }
        }
    }

    fn run(&mut self) -> Result<(), LoxError> {
        let mut frame = self.frames.pop().unwrap();
        loop {
            let instruction = frame.function.chunk.code[frame.ip];
            #[cfg(debug_assertions)]
            {
                for val in self.stack.iter() {
                    print!("[{}]", val);
                }
                println!("");
                #[cfg(debug_assertions)]
                frame
                    .function
                    .chunk
                    .disassemble_instruction(&instruction, frame.ip);
            }
            frame.ip += 1;

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
                            Vm::runtime_error(
                                &frame,
                                "Operands must be numbers or string at the same time.",
                            );
                            return Err(LoxError::RuntimeError);
                        }
                    }
                }
                Instruction::Constant(index) => {
                    let val = frame.function.chunk.read_constant(index);
                    self.stack.push(val);
                }
                Instruction::DefineGlobal(index) => {
                    let s = frame.function.chunk.read_string(index);
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
                    let s = frame.function.chunk.read_string(index);
                    match self.globals.get(&s) {
                        Some(&value) => self.push(value),
                        None => {
                            let name = self.strings.lookup(s);
                            let msg = format!("Undefined variable '{}'.", name);
                            Vm::runtime_error(&frame, &msg);
                            return Err(LoxError::RuntimeError);
                        }
                    }
                }
                Instruction::GetLocal(slot) => {
                    let i = slot + frame.slot;
                    let value = self.stack[i];
                    self.push(value);
                }
                Instruction::Greater => self.binary_op(|a, b| a > b, |n| Value::Bool(n))?,
                Instruction::Jump(offset) => {
                    frame.ip += offset as usize;
                }
                Instruction::JumpIfFalse(offset) => {
                    if self.peek(0).is_falsy() {
                        frame.ip += offset as usize;
                    }
                }
                Instruction::Less => self.binary_op(|a, b| a < b, |n| Value::Bool(n))?,
                Instruction::Loop(offset) => {
                    frame.ip -= offset as usize + 1;
                }
                Instruction::Multiply => self.binary_op(|a, b| a * b, |n| Value::Number(n))?,
                Instruction::Negate => {
                    if let Value::Number(value) = self.peek(0) {
                        self.pop();
                        self.push(Value::Number(-value));
                    } else {
                        Vm::runtime_error(&frame, "Operand must be a number");
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
                    let name = frame.function.chunk.read_string(index);
                    let value = self.peek(0);
                    // means not exist name, so this is an error
                    if let None = self.globals.insert(name, value) {
                        // delete
                        self.globals.remove(&name);
                        let s = self.strings.lookup(name);
                        let msg = format!("Undefined variable '{}'.", s);
                        Vm::runtime_error(&frame, &msg);
                        return Err(LoxError::RuntimeError);
                    }
                }
                Instruction::Return => {
                    return Ok(());
                }
                Instruction::SetLocal(slot) => {
                    let value = self.peek(0);
                    let i = slot + frame.slot;
                    self.stack[i] = value;
                }
                Instruction::Substract => self.binary_op(|a, b| a - b, |n| Value::Number(n))?,
                Instruction::True => self.push(Value::Bool(true)),
            }
        }
    }

    fn runtime_error(frame: &CallFrame, msg: &str) {
        eprintln!("{}", msg);
        let line = frame.function.chunk.lines[frame.ip - 1];
        eprintln!("[line {}] in script", line);
    }
}
