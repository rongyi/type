use crate::{
    chunk::{Chunk, Instruction, Value},
    error::LoxError,
    scanner::{Scanner, Token, TokenType},
    strings::Strings,
};

use std::collections::HashMap;
use std::convert::TryFrom;

#[derive(Debug, Copy, Clone, PartialEq, PartialOrd)]
enum Precedence {
    None,
    Assignment, // =
    Or,         // or
    And,        // and
    Equality,   // == !=
    Comparison, // < > <= >=
    Term,       // + -
    Factor,     // * /
    Unary,      // ! -
    Call,       // . ()
    Primary,
}

impl Precedence {
    fn next(&self) -> Precedence {
        match self {
            Precedence::None => Precedence::Assignment,
            Precedence::Assignment => Precedence::Or,
            Precedence::Or => Precedence::And,
            Precedence::And => Precedence::Equality,
            Precedence::Equality => Precedence::Comparison,
            Precedence::Comparison => Precedence::Term,
            Precedence::Term => Precedence::Factor,
            Precedence::Factor => Precedence::Unary,
            Precedence::Unary => Precedence::Call,
            Precedence::Call => Precedence::Primary,
            Precedence::Primary => Precedence::None,
        }
    }
}

type ParseFn<'a> = fn(&mut Parser<'a>, can_assign: bool) -> ();

#[derive(Copy, Clone)]
struct ParseRule<'a> {
    prefix: Option<ParseFn<'a>>,
    infix: Option<ParseFn<'a>>,
    precedence: Precedence,
}

impl<'a> ParseRule<'a> {
    fn new(
        prefix: Option<ParseFn<'a>>,
        infix: Option<ParseFn<'a>>,
        precedence: Precedence,
    ) -> ParseRule<'a> {
        ParseRule {
            prefix,
            infix,
            precedence,
        }
    }
}

struct Local<'a> {
    name: Token<'a>,
    depth: i32,
}

impl<'a> Local<'a> {
    fn new(name: Token<'a>, depth: i32) -> Self {
        Self { name, depth }
    }
}

const LOCAL_COUNT: usize = std::u8::MAX as usize + 1;

struct Compiler<'a> {
    locals: Vec<Local<'a>>,
    scope_depth: i32,
}

impl<'a> Compiler<'a> {
    fn new() -> Self {
        Compiler {
            locals: Vec::with_capacity(LOCAL_COUNT),
            scope_depth: 0,
        }
    }
}

pub struct Parser<'a> {
    scanner: Scanner<'a>,
    compiler: Compiler<'a>,
    chunk: &'a mut Chunk,
    strings: &'a mut Strings,
    current: Token<'a>,
    previous: Token<'a>,
    had_error: bool,
    panic_mode: bool,
    rules: HashMap<TokenType, ParseRule<'a>>,
}

impl<'a> Parser<'a> {
    pub fn new(code: &'a str, chunk: &'a mut Chunk, strings: &'a mut Strings) -> Parser<'a> {
        let t1 = Token {
            kind: TokenType::Eof,
            lexeme: "",
            line: 1,
        };
        let t2 = Token {
            kind: TokenType::Eof,
            lexeme: "",
            line: 1,
        };
        let mut rules = HashMap::new();
        let mut rule = |kind, prefix, infix, precedence| {
            rules.insert(kind, ParseRule::new(prefix, infix, precedence));
        };

        rule(
            TokenType::LeftParen,
            Some(Parser::grouping),
            None,
            Precedence::None,
        );
        rule(TokenType::RightParen, None, None, Precedence::None);
        rule(TokenType::LeftBrace, None, None, Precedence::None);
        rule(TokenType::RightBrace, None, None, Precedence::None);
        rule(TokenType::Comma, None, None, Precedence::None);
        rule(TokenType::Dot, None, None, Precedence::None);
        rule(
            TokenType::Minus,
            Some(Parser::unary),
            Some(Parser::binary),
            Precedence::Term,
        );
        rule(
            TokenType::Plus,
            None,
            Some(Parser::binary),
            Precedence::Term,
        );
        rule(TokenType::Semicolon, None, None, Precedence::None);
        rule(
            TokenType::Slash,
            None,
            Some(Parser::binary),
            Precedence::Factor,
        );
        rule(
            TokenType::Star,
            None,
            Some(Parser::binary),
            Precedence::Factor,
        );
        rule(TokenType::Bang, Some(Parser::unary), None, Precedence::None);
        rule(
            TokenType::BangEqual,
            None,
            Some(Parser::binary),
            Precedence::Equality,
        );
        rule(TokenType::Equal, None, None, Precedence::None);
        rule(
            TokenType::EqualEqual,
            None,
            Some(Parser::binary),
            Precedence::Equality,
        );
        rule(
            TokenType::Greater,
            None,
            Some(Parser::binary),
            Precedence::Comparison,
        );
        rule(
            TokenType::GreaterEqual,
            None,
            Some(Parser::binary),
            Precedence::Comparison,
        );
        rule(
            TokenType::Less,
            None,
            Some(Parser::binary),
            Precedence::Comparison,
        );
        rule(
            TokenType::LessEqual,
            None,
            Some(Parser::binary),
            Precedence::Comparison,
        );
        rule(
            TokenType::Identifier,
            Some(Parser::variable),
            None,
            Precedence::None,
        );
        rule(
            TokenType::String,
            Some(Parser::string),
            Some(Parser::binary),
            Precedence::None,
        );
        rule(
            TokenType::Number,
            Some(Parser::number),
            None,
            Precedence::None,
        );
        rule(TokenType::And, None, None, Precedence::None);
        rule(TokenType::Class, None, None, Precedence::None);
        rule(TokenType::Else, None, None, Precedence::None);
        rule(
            TokenType::False,
            Some(Parser::literal),
            None,
            Precedence::None,
        );
        rule(TokenType::For, None, None, Precedence::None);
        rule(TokenType::Fun, None, None, Precedence::None);
        rule(TokenType::If, None, None, Precedence::None);
        rule(
            TokenType::Nil,
            Some(Parser::literal),
            None,
            Precedence::None,
        );
        rule(TokenType::Or, None, None, Precedence::None);
        rule(TokenType::Print, None, None, Precedence::None);
        rule(TokenType::Return, None, None, Precedence::None);
        rule(TokenType::Super, None, None, Precedence::None);
        rule(TokenType::This, None, None, Precedence::None);
        rule(
            TokenType::True,
            Some(Parser::literal),
            None,
            Precedence::None,
        );
        rule(TokenType::Var, None, None, Precedence::None);
        rule(TokenType::While, None, None, Precedence::None);
        rule(TokenType::Error, None, None, Precedence::None);
        rule(TokenType::Eof, None, None, Precedence::None);

        Parser {
            scanner: Scanner::new(code),
            compiler: Compiler::new(),
            chunk,
            strings,
            current: t1,
            previous: t2,
            had_error: false,
            panic_mode: false,
            rules,
        }
    }

    pub fn compile(&mut self) -> Result<(), LoxError> {
        self.advance();
        while !self.matches(TokenType::Eof) {
            self.declaration();
        }
        self.emit(Instruction::Return);

        #[cfg(debug_assertions)]
        if !self.had_error {
            self.chunk.disassemble("code");
        }

        if self.had_error {
            Err(LoxError::CompileError)
        } else {
            Ok(())
        }
    }

    fn expression(&mut self) {
        self.parse_precedence(Precedence::Assignment);
    }

    fn expression_statement(&mut self) {
        self.expression();
        self.consume(TokenType::Semicolon, "Expect ';' after expression.");
        self.emit(Instruction::Pop);
    }

    fn declaration(&mut self) {
        if self.matches(TokenType::Var) {
            self.var_declaration();
        } else {
            self.statement();
        }

        if self.panic_mode {
            self.synchronize();
        }
    }

    fn var_declaration(&mut self) {
        let index = self.parse_variable("Expect variable name.");
        if self.matches(TokenType::Equal) {
            self.expression();
        } else {
            self.emit(Instruction::Nil);
        }
        self.consume(
            TokenType::Semicolon,
            "Expect ';' after variable declaration.",
        );
        self.define_variable_global(index);
    }

    fn define_variable_global(&mut self, index: usize) {
        // no in global? ignore
        if self.compiler.scope_depth > 0 {
            // a side effect
            self.mark_initialized();
            return;
        }

        self.emit(Instruction::DefineGlobal(index));
    }

    fn mark_initialized(&mut self) {
        let last_local = self.compiler.locals.last_mut().unwrap();
        last_local.depth = self.compiler.scope_depth;
    }

    fn statement(&mut self) {
        if self.matches(TokenType::Print) {
            self.print_statement();
        } else if self.matches(TokenType::LeftBrace) {
            self.begin_scope();
            self.block();
            self.end_scope();
        } else {
            self.expression_statement();
        }
    }
    fn begin_scope(&mut self) {
        self.compiler.scope_depth += 1;
    }

    fn end_scope(&mut self) {
        self.compiler.scope_depth -= 1;

        for i in (0..self.compiler.locals.len()).rev() {
            if self.compiler.locals[i].depth > self.compiler.scope_depth {
                self.emit(Instruction::Pop);
                self.compiler.locals.pop();
            }
        }
    }

    fn block(&mut self) {
        while !self.check(TokenType::RightBrace) && !self.check(TokenType::Eof) {
            self.declaration();
        }

        self.consume(TokenType::RightBrace, "Expect '}' after block.");
    }

    fn print_statement(&mut self) {
        self.expression();
        self.consume(TokenType::Semicolon, "Expect ';' after value");
        self.emit(Instruction::Print);
    }

    fn number(&mut self, _can_assign: bool) {
        let value: f64 = self
            .previous
            .lexeme
            .parse()
            .expect("Parsed value is not a double");
        self.emit_constant(Value::Number(value));
    }

    fn matches(&mut self, kind: TokenType) -> bool {
        if !self.check(kind) {
            false
        } else {
            self.advance();
            true
        }
    }

    fn check(&self, kind: TokenType) -> bool {
        self.current.kind == kind
    }

    fn string(&mut self, _can_assign: bool) {
        let lexeme = self.previous.lexeme;
        let value = &lexeme[1..(lexeme.len() - 1)];
        let s = self.strings.intern(value);
        self.emit_constant(Value::String(s));
    }

    fn literal(&mut self, _can_assign: bool) {
        match self.previous.kind {
            TokenType::False => self.emit(Instruction::False),
            TokenType::True => self.emit(Instruction::True),
            TokenType::Nil => self.emit(Instruction::Nil),
            _ => panic!("Unreachable literal"),
        }
    }

    fn variable(&mut self, can_assign: bool) {
        self.named_variable(self.previous, can_assign);
    }

    fn named_variable(&mut self, name: Token, can_assign: bool) {
        let get_op;
        let set_op;
        if let Some(arg) = self.resolve_local(name) {
            get_op = Instruction::GetLocal(arg);
            set_op = Instruction::SetLocal(arg);
        } else {
            let index = self.identifier_constant(name);
            get_op = Instruction::GetGlobal(index);
            set_op = Instruction::SetGlobal(index);
        }

        if can_assign && self.matches(TokenType::Equal) {
            self.expression();
            self.emit(set_op)
        } else {
            self.emit(get_op);
        }
    }

    fn resolve_local(&mut self, name: Token) -> Option<usize> {
        for (i, local) in self.compiler.locals.iter().enumerate().rev() {
            if name.lexeme == local.name.lexeme {
                if local.depth == -1 {
                    self.error("Can not read local variable in its own initializer.");
                }
                return Option::from(i);
            }
        }
        None
    }

    fn grouping(&mut self, _can_assign: bool) {
        self.expression();
        self.consume(TokenType::RightParen, "Expect ')' after expression.")
    }

    fn unary(&mut self, _can_assign: bool) {
        let operator = self.previous.kind;
        self.parse_precedence(Precedence::Unary);
        match operator {
            TokenType::Bang => self.emit(Instruction::Not),
            TokenType::Minus => self.emit(Instruction::Negate),
            _ => panic!("Invalid unary operator"),
        }
    }

    fn binary(&mut self, _can_assign: bool) {
        let operator = self.previous.kind;
        let rule = self.get_rule(operator);
        self.parse_precedence(rule.precedence.next());
        match operator {
            TokenType::Plus => self.emit(Instruction::Add),
            TokenType::Minus => self.emit(Instruction::Substract),
            TokenType::Star => self.emit(Instruction::Multiply),
            TokenType::Slash => self.emit(Instruction::Divide),
            TokenType::BangEqual => self.emit_two(Instruction::Equal, Instruction::Not),
            TokenType::EqualEqual => self.emit(Instruction::Equal),
            TokenType::Greater => self.emit(Instruction::Greater),
            TokenType::GreaterEqual => self.emit_two(Instruction::Less, Instruction::Not),
            TokenType::Less => self.emit(Instruction::Less),
            TokenType::LessEqual => self.emit_two(Instruction::Greater, Instruction::Not),

            _ => panic!("Invalid binary operator"),
        }
    }

    fn parse_precedence(&mut self, precedence: Precedence) {
        self.advance();
        let prefix_rule = self.get_rule(self.previous.kind).prefix;

        let prefix_rule = match prefix_rule {
            Some(rule) => rule,
            None => {
                self.error("Expect expression.");
                return;
            }
        };
        let can_assign = precedence <= Precedence::Assignment;
        prefix_rule(self, can_assign);
        while self.is_lower_precedence(precedence) {
            self.advance();
            let infix_rule = self.get_rule(self.previous.kind).infix.unwrap();
            infix_rule(self, can_assign);
        }
        if can_assign && self.matches(TokenType::Equal) {
            self.error("Invalid assignment target.");
        }
    }

    fn parse_variable(&mut self, msg: &str) -> usize {
        self.consume(TokenType::Identifier, msg);
        self.declare_variable_local();

        if self.compiler.scope_depth > 0 {
            return 0;
        }

        self.identifier_constant(self.previous)
    }

    fn identifier_constant(&mut self, token: Token) -> usize {
        let identifier = self.strings.intern(token.lexeme);
        let value = Value::String(identifier);
        self.make_constant(value)
    }

    // declare means take a position first
    fn declare_variable_local(&mut self) {
        // glocal? quit
        if self.compiler.scope_depth == 0 {
            return;
        }
        let name = self.previous;
        if self.is_local_declared(name) {
            self.error("Variable with this name already delared in this scope.");
        }
        self.add_local(name);
    }

    fn is_local_declared(&self, name: Token) -> bool {
        for local in self.compiler.locals.iter().rev() {
            if local.depth != -1 && local.depth < self.compiler.scope_depth {
                return false;
            }
            if local.name.lexeme == name.lexeme {
                return true;
            }
        }
        false
    }
    fn add_local(&mut self, token: Token<'a>) {
        if self.compiler.locals.len() == LOCAL_COUNT {
            self.error("Too many local variable in function.");
            return;
        }
        let local = Local::new(token, -1);
        self.compiler.locals.push(local);
    }

    fn is_lower_precedence(&self, precedence: Precedence) -> bool {
        let current_precedence = self.get_rule(self.current.kind).precedence;
        precedence <= current_precedence
    }

    fn consume(&mut self, expected: TokenType, msg: &str) {
        if self.current.kind == expected {
            self.advance();
            return;
        }
        self.error_at_current(msg);
    }

    fn advance(&mut self) {
        self.previous = self.current;
        loop {
            self.current = self.scanner.scan_token();
            if self.current.kind == TokenType::Error {
                self.error_at_current(self.current.lexeme);
            } else {
                break;
            }
        }
    }

    fn error_at_current(&mut self, msg: &str) {
        self.error_at(self.current, msg);
    }

    fn error(&mut self, msg: &str) {
        self.error_at(self.previous, msg);
    }

    fn error_at(&mut self, token: Token, msg: &str) {
        if self.panic_mode {
            return;
        }
        self.had_error = true;
        self.panic_mode = true;
        eprint!("[line {}] Error", token.line);
        if token.kind == TokenType::Eof {
            eprint!(" at end");
        } else {
            eprint!(" at '{}'", token.lexeme);
        }
        eprintln!(": {}", msg);
    }

    fn synchronize(&mut self) {
        self.panic_mode = false;
        while self.previous.kind != TokenType::Eof {
            if self.previous.kind == TokenType::Semicolon {
                return;
            }
            match self.current.kind {
                TokenType::Class
                | TokenType::Fun
                | TokenType::Var
                | TokenType::For
                | TokenType::If
                | TokenType::While
                | TokenType::Print
                | TokenType::Return => return,
                _ => (),
            }

            self.advance();
        }
    }

    fn emit(&mut self, instruction: Instruction) {
        self.chunk.write(instruction, self.previous.line);
    }

    fn emit_two(&mut self, i1: Instruction, i2: Instruction) {
        self.chunk.write(i1, self.previous.line);
        self.chunk.write(i2, self.previous.line);
    }

    fn make_constant(&mut self, value: Value) -> usize {
        let index = self.chunk.add_constant(value);
        let index = match u8::try_from(index) {
            Ok(index) => index,
            _ => 0,
        };
        index as usize
    }

    fn emit_constant(&mut self, value: Value) {
        let index = self.make_constant(value);
        self.emit(Instruction::Constant(index));
    }

    fn get_rule(&self, kind: TokenType) -> ParseRule<'a> {
        self.rules.get(&kind).cloned().unwrap()
    }
}
