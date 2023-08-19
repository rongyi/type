use std::env;
use std::fs;
use std::io::{self, Write};

const DEBUG: bool = true;
type Value = f64;

#[derive(Debug, Clone, Copy)]
enum Instruction {
    Negate,
    Add,
    Substract,
    Multiply,
    Divide,
    Constant(usize),
    Return,
}

struct Chunk {
    code: Vec<Instruction>,
    constants: Vec<Value>,
    lines: Vec<usize>,
}

impl Chunk {
    pub fn new() -> Self {
        Self {
            code: Vec::new(),
            constants: Vec::new(),
            lines: Vec::new(),
        }
    }

    fn write(&mut self, instruction: Instruction, line: usize) {
        self.code.push(instruction);
        self.lines.push(line);
    }

    fn add_constant(&mut self, value: Value) -> usize {
        self.constants.push(value);
        self.constants.len() - 1
    }

    fn disassemble(&self, name: &str) {
        println!("=== {} ===", name);
        for (offset, instruction) in self.code.iter().enumerate() {
            self.disassemble_instruction(instruction, offset);
        }
    }

    fn disassemble_instruction(&self, instruction: &Instruction, offset: usize) {
        print!("{:04} ", offset);
        let line = self.lines[offset];
        if offset > 0 && line == self.lines[offset - 1] {
            print!("   | ");
        } else {
            print!("{:>4} ", line);
        }
        match instruction {
            Instruction::Negate => println!("OP_NEGATE"),
            Instruction::Add => println!("OP_ADD"),
            Instruction::Substract => println!("OP_SUBSTRACT"),
            Instruction::Multiply => println!("OP_MULTIPLY"),
            Instruction::Divide => println!("OP_DVIDE"),
            Instruction::Return => println!("OP_RETURN"),
            Instruction::Constant(index) => {
                let value = self.constants[*index];
                println!("{:<16} {:4} {}", "OP_CONSTANT", index, value);
            }
        }
    }
}

enum InterpreResult {
    Ok,
    CompileError,
    RuntimeError,
}

struct Vm {
    chunk: Chunk,
    ip: usize,
    stack: Vec<Value>,
}

impl Vm {
    pub fn new(chunk: Chunk) -> Self {
        Self {
            chunk,
            ip: 0,
            stack: Vec::with_capacity(256),
        }
    }
    fn push(&mut self, val: Value) {
        self.stack.push(val);
    }
    fn pop(&mut self) -> Value {
        self.stack.pop().expect("Empty stack")
    }
    fn binary_op(&mut self, f: fn(Value, Value) -> Value) {
        let b = self.pop();
        let a = self.pop();
        self.push(f(a, b));
    }

    fn next_instruction(&mut self) -> Instruction {
        let instruction = self.chunk.code[self.ip];
        self.ip += 1;
        instruction
    }

    fn run(&mut self) -> InterpreResult {
        loop {
            for val in self.stack.iter() {
                print!("[ {} ]", val);
            }
            println!("");
            let instruction = self.next_instruction();
            if DEBUG {
                self.chunk
                    .disassemble_instruction(&instruction, self.ip - 1);
            }
            match instruction {
                Instruction::Constant(index) => {
                    let val = self.chunk.constants[index];
                    self.stack.push(val);
                }
                Instruction::Negate => {
                    let val = self.pop();
                    self.push(-val);
                }
                Instruction::Return => {
                    println!("{}", self.stack.pop().expect("empty stack!"));
                    return InterpreResult::Ok;
                }
                Instruction::Add => self.binary_op(|a, b| a + b),
                Instruction::Substract => self.binary_op(|a, b| a - b),
                Instruction::Multiply => self.binary_op(|a, b| a * b),
                Instruction::Divide => self.binary_op(|a, b| a / b),
            }
        }
    }
}

fn interpret(chunk: Chunk) -> InterpreResult {
    let mut vm = Vm::new(chunk);
    return vm.run();
}

fn run_code(code: &str) {
    println!("Running {}", code);
}

fn repl() {
    let mut line = String::new();
    loop {
        print!("> ");
        io::stdout().flush().unwrap();
        io::stdin()
            .read_line(&mut line)
            .expect("Unable to read line from the REPL");
        if line.len() == 0 {
            break;
        }
        run_code(&line);
        line.clear();
    }
}

fn run_file(path: &str) {
    let code = match fs::read_to_string(path) {
        Ok(content) => content,
        Err(err) => {
            eprint!("Unable to read file {}: {}", path, err);
            std::process::exit(1);
        }
    };
    run_code(&code);
}

fn main() {
    let args: Vec<String> = env::args().collect();
    match args.len() {
        1 => repl(),
        2 => run_file(&args[1]),
        _ => std::process::exit(1),
    }
}
