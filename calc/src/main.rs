use std::io::{self, Write, Read};

use crate::parser::expr;
use crate::ast::eval;

mod token;
mod lexer;
mod ast;
mod parser;

fn main() {
    let mut input : String = String::new();

    loop {
        print!("> ");
        io::stdout().flush().unwrap();
        io::stdin().read_line(&mut input).expect("fail to read stdin");
        let s: String = input.split_whitespace().collect();
        if s.len() == 0 {
            break;
        }
        match expr(&s) {
            Ok(node) => println!("{}", eval(node)),
            Err(s) => eprintln!("{}", s),
        }
        input.clear();
    }
}
