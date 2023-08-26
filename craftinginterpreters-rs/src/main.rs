mod chunk;
mod error;
mod compiler;
mod scanner;
mod strings;
mod function;
mod vm;

use std::env;
use std::fs;
use std::io::{self, Write};
use std::process;
use vm::Vm;

fn repl(vm: &mut Vm) {
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
        let _ = vm.interpret(&line);
        line.clear();
    }
}

fn run_file(vm: &mut Vm, path: &str) {
    let code = match fs::read_to_string(path) {
        Ok(content) => content,
        Err(err) => {
            eprint!("Unable to read file {}: {}", path, err);
            process::exit(74);
        }
    };
    match vm.interpret(&code) {
        Ok(_) => process::exit(65),
        _ => process::exit(70),
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut vm = Vm::new();
    match args.len() {
        1 => repl(&mut vm),
        2 => run_file(&mut vm, &args[1]),
        _ => process::exit(64),
    }
}
