mod chunk;
mod error;
mod parser;
mod scanner;
mod strings;
mod vm;

use parser::Parser;
use error::LoxError;
use vm::Vm;
use std::env;
use std::fs;
use std::io::{self, Write};
use std::process;


fn interpret(code: &str) -> Result<(), LoxError> {
    let mut parser = Parser::new(code);
    parser.compile()?;
    let mut vm = Vm::new(parser.chunk);

    return vm.run();
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
        let _ = interpret(&line);
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
    match interpret(&code) {
        Ok(_) => process::exit(65),
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
