use std::env;
use std::fs;
use std::io::{self, Write};
use std::process;

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

    fn read_constant(&self, index: usize) -> Value {
        self.constants[index]
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

enum InterpreterResult {
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

    fn run(&mut self) -> InterpreterResult {
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
                    let val = self.chunk.read_constant(index);
                    self.stack.push(val);
                }
                Instruction::Negate => {
                    let val = self.pop();
                    self.push(-val);
                }
                Instruction::Return => {
                    println!("{}", self.stack.pop().expect("empty stack!"));
                    return InterpreterResult::Ok;
                }
                Instruction::Add => self.binary_op(|a, b| a + b),
                Instruction::Divide => self.binary_op(|a, b| a / b),
                Instruction::Multiply => self.binary_op(|a, b| a * b),
                Instruction::Substract => self.binary_op(|a, b| a - b),
            }
        }
    }

    fn next_instruction(&mut self) -> Instruction {
        let instruction = self.chunk.code[self.ip];
        self.ip += 1;
        instruction
    }
}

#[derive(Debug, Clone, Copy)]
enum TokenType {
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,

    // One or two character tokens.
    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    // Literals.
    Identifier,
    String,
    Number,

    // Keywords.
    And,
    Class,
    Else,
    False,
    For,
    Fun,
    If,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While,

    Error,
    Eof,
}

struct Token<'a> {
    kind: TokenType,
    line: usize,
    lexeme: &'a str,
}

struct Scanner<'a> {
    code: &'a str,
    start: usize,
    current: usize,
    line: usize,
}

impl<'a> Scanner<'a> {
    fn new(code: &'a str) -> Scanner {
        Scanner {
            code,
            start: 0,
            current: 0,
            line: 1, // start line 1
        }
    }
    fn scan_token(&mut self) -> Token {
        self.start = self.current;
        if self.is_at_end() {
            return self.make_token(TokenType::Eof);
        }
        let c = self.advance();
        match c {
            b'(' => self.make_token(TokenType::LeftParen),
            b')' => self.make_token(TokenType::RightParen),
            b'{' => self.make_token(TokenType::LeftBrace),
            b'}' => self.make_token(TokenType::RightBrace),
            b';' => self.make_token(TokenType::Semicolon),
            b',' => self.make_token(TokenType::Comma),
            b'.' => self.make_token(TokenType::Dot),
            b'-' => self.make_token(TokenType::Minus),
            b'+' => self.make_token(TokenType::Plus),
            b'/' => self.make_token(TokenType::Slash),
            b'*' => self.make_token(TokenType::Star),
            b'!' if self.matches(b'=') => self.make_token(TokenType::BangEqual),
            b'!' => self.make_token(TokenType::Bang),
            b'=' if self.matches(b'=') => self.make_token(TokenType::EqualEqual),
            b'=' => self.make_token(TokenType::Equal),
            b'<' if self.matches(b'=') => self.make_token(TokenType::LessEqual),
            b'<' => self.make_token(TokenType::Less),
            b'>' if self.matches(b'=') => self.make_token(TokenType::GreaterEqual),
            b'>' => self.make_token(TokenType::Greater),
            _ => self.error_token("Unexpected character."),
        }
    }
    fn is_at_end(&self) -> bool {
        self.current >= self.code.len()
    }
    fn make_token(&self, kind: TokenType) -> Token {
        Token {
            kind,
            lexeme: &self.code[self.start..self.current],
            line: self.line,
        }
    }
    fn error_token(&self, message: &'static str) -> Token {
        Token {
            kind: TokenType::Error,
            lexeme: message,
            line: self.line,
        }
    }

    fn advance(&mut self) -> u8 {
        let c = self.code.as_bytes()[self.current];
        self.current += 1;

        c
    }
    // if true, advance
    fn matches(&mut self, expected: u8) -> bool {
        if self.is_at_end() {
            false
        } else if self.code.as_bytes()[self.current] != expected {
            false
        } else {
            self.current += 1;
            true
        }
    }
}

fn interpret(chunk: Chunk) -> InterpreterResult {
    let mut vm = Vm::new(chunk);
    return vm.run();
}

fn compile(code: &str) {}

fn run_code(code: &str) -> InterpreterResult {
    compile(code);

    InterpreterResult::Ok
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
            process::exit(74);
        }
    };
    match run_code(&code) {
        InterpreterResult::Ok => process::exit(65),
        _ => process::exit(70),
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    match args.len() {
        1 => repl(),
        2 => run_file(&args[1]),
        _ => process::exit(64),
    }
}
