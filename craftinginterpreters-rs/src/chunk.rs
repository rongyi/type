use crate::closure::ClosureID;
use crate::function::FunctionID;
use crate::function::NativeFn;
use crate::strings::LoxString;
use std::fmt;

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Value {
    Nil,
    Bool(bool),
    Number(f64),
    String(LoxString),
    Function(FunctionID),
    Closure(ClosureID),
    NativeFunction(NativeFn),
}

impl Value {
    pub fn is_falsy(&self) -> bool {
        match self {
            Value::Nil => true,
            Value::Bool(value) => !value,
            _ => false,
        }
    }
}

impl fmt::Display for Value {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Value::Nil => write!(f, "nil"),
            Value::Bool(value) => write!(f, "{}", value),
            Value::Number(value) => write!(f, "{}", value),
            Value::String(value) => write!(f, "<str {}>", value),
            Value::Function(value) => write!(f, "<fn {}>", value),
            Value::Closure(value) => write!(f, "<fn {}>", value),
            Value::NativeFunction(_) => write!(f, "<native fn>"),
        }
    }
}

#[derive(Debug, Copy, Clone)]
pub enum Instruction {
    Add,
    Call(usize),
    CloseUpvalue,
    Closure(usize),
    Constant(usize),
    DefineGlobal(usize),
    Divide,
    Equal,
    False,
    GetLocal(usize),
    GetUpvalue(usize),
    GetGlobal(usize),
    Greater,
    Jump(u16),
    JumpIfFalse(u16),
    Less,
    Loop(u16),
    Multiply,
    Negate,
    Nil,
    Not,
    Pop,
    Print,
    Return,
    SetLocal(usize),
    SetUpvalue(usize),
    SetGlobal(usize),
    Substract,
    True,
}

#[derive(Default)]
pub struct Chunk {
    pub code: Vec<Instruction>,
    pub constants: Vec<Value>,
    pub lines: Vec<usize>,
}

impl Chunk {
    pub fn write(&mut self, instruction: Instruction, line: usize) -> usize {
        self.code.push(instruction);
        self.lines.push(line);

        self.code.len() - 1
    }

    pub fn add_constant(&mut self, value: Value) -> usize {
        self.constants.push(value);
        self.constants.len() - 1
    }

    pub fn read_constant(&self, index: usize) -> Value {
        self.constants[index].clone()
    }
    pub fn read_string(&self, index: usize) -> LoxString {
        if let Value::String(s) = self.read_constant(index) {
            s
        } else {
            panic!("Constant is not String!")
        }
    }

    #[cfg(debug_assertions)]
    pub fn disassemble(&self, name: &str) {
        println!("=== {} ===", name);
        for (offset, instruction) in self.code.iter().enumerate() {
            self.disassemble_instruction(instruction, offset);
        }
    }

    #[cfg(debug_assertions)]
    pub fn disassemble_instruction(&self, instruction: &Instruction, offset: usize) {
        print!("{:04} ", offset);
        let line = self.lines[offset];
        if offset > 0 && line == self.lines[offset - 1] {
            print!("   | ");
        } else {
            print!("{:>4} ", line);
        }
        match instruction {
            Instruction::Add => println!("OP_ADD"),
            Instruction::CloseUpvalue => println!("OP_CLOSE_UPVALUE"), // TODO: implement
            Instruction::Closure(i) => println!("OP_CLOSURE {}", i),   // TODO: implement
            Instruction::Constant(i) => self.disassemble_constant("OP_CONSTANT", *i),
            Instruction::Call(i) => println!("OP_CALL {}", i), // TODO: implement
            Instruction::DefineGlobal(i) => self.disassemble_constant("OP_DEFINE_GLOBAL", *i),
            Instruction::Divide => println!("OP_DIVIDE"),
            Instruction::Equal => println!("OP_EQUAL"),
            Instruction::False => println!("OP_FALSE"),
            Instruction::GetGlobal(i) => self.disassemble_constant("OP_GET_GLOBAL", *i),
            Instruction::GetLocal(i) => println!("OP_GET_LOCAL {}", i),
            Instruction::GetUpvalue(i) => println!("OP_GET_UPVALUE {}", i),
            Instruction::Greater => println!("OP_GREATER"),
            Instruction::Jump(offset) => println!("OP_JUMP {}", offset), // TODO:
            Instruction::JumpIfFalse(offset) => println!("OP_JUMP_IF_FALSE {}", offset), // TODO:
            Instruction::Less => println!("OP_LESS"),
            Instruction::Loop(offset) => println!("OP_LOOP {}", offset), // TODO:
            Instruction::Multiply => println!("OP_MULTIPLY"),
            Instruction::Negate => println!("OP_NEGATE"),
            Instruction::Not => println!("OP_NOT"),
            Instruction::Nil => println!("OP_NIL"),
            Instruction::Pop => println!("OP_POP"),
            Instruction::Print => println!("OP_PRINT"),
            Instruction::Return => println!("OP_RETURN"),
            Instruction::SetGlobal(i) => self.disassemble_constant("OP_SET_GLOBAL", *i),
            Instruction::SetLocal(i) => println!("OP_SET_LOCAL {}", i), // TODO: implement
            Instruction::SetUpvalue(i) => println!("OP_SET_UPVALUE {}", i),
            Instruction::Substract => println!("OP_SUBSTRACT"),
            Instruction::True => println!("OP_TRUE"),
        }
    }

    #[cfg(debug_assertions)]
    fn disassemble_constant(&self, name: &str, index: usize) {
        let value = self.constants[index].clone();
        println!("{:<16} {:4} {}", name, index, value);
    }
}
