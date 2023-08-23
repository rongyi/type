use crate::strings::LoxString;
use std::fmt;

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Value {
    Nil,
    Bool(bool),
    Number(f64),
    String(LoxString),
}

impl Value {
    pub fn is_falsy(&self) -> bool {
        match self {
            Value::Nil | Value::Number(_) | Value::String(_) => true,
            Value::Bool(value) => !value,
        }
    }
}

impl fmt::Display for Value {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Value::Nil => write!(f, "nil"),
            Value::Bool(value) => write!(f, "{}", value),
            Value::Number(value) => write!(f, "{}", value),
            Value::String(value) => write!(f, "{}", value),
        }
    }
}

#[derive(Debug, Copy, Clone)]
pub enum Instruction {
    Add,
    Constant(usize),
    DefineGlobal(usize),
    Divide,
    Equal,
    False,
    GetGlobal(usize),
    Greater,
    Less,
    Multiply,
    Negate,
    Nil,
    Not,
    Pop,
    Print,
    Return,
    Substract,
    True,
}

pub struct Chunk {
    pub code: Vec<Instruction>,
    pub constants: Vec<Value>,
    pub lines: Vec<usize>,
}

impl Chunk {
    pub fn new() -> Self {
        Self {
            code: Vec::new(),
            constants: Vec::new(),
            lines: Vec::new(),
        }
    }

    pub fn write(&mut self, instruction: Instruction, line: usize) {
        self.code.push(instruction);
        self.lines.push(line);
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
            Instruction::Constant(index) => self.disassemble_constant("OP_CONSTANT", *index),
            Instruction::Add => println!("OP_ADD"),
            Instruction::DefineGlobal(i) => self.disassemble_constant("OP_DEFINE_GLOBAL", *i),
            Instruction::Divide => println!("OP_DIVIDE"),
            Instruction::Equal => println!("OP_EQUAL"),
            Instruction::False => println!("OP_FALSE"),
            Instruction::GetGlobal(i) => self.disassemble_constant("OP_GET_GLOBAL", *i),
            Instruction::Greater => println!("OP_GREATER"),
            Instruction::Less => println!("OP_LESS"),
            Instruction::Multiply => println!("OP_MULTIPLY"),
            Instruction::Negate => println!("OP_NEGATE"),
            Instruction::Not => println!("OP_NOT"),
            Instruction::Nil => println!("OP_NIL"),
            Instruction::Pop => println!("OP_POP"),
            Instruction::Print => println!("OP_PRINT"),
            Instruction::Return => println!("OP_RETURN"),
            Instruction::Substract => println!("OP_SUBSTRACT"),
            Instruction::True => println!("OP_TRUE"),
        }
    }

    fn disassemble_constant(&self, name: &str, index: usize) {
        let value = self.constants[index].clone();
        println!("{:<16} {:4} {}", name, index, value);
    }
}
