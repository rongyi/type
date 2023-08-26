use crate::{
    chunk::{Instruction, Value},
    error::LoxError,
    function::{FunctionID, FunctionType, Functions, LoxFunction},
    scanner::{Scanner, Token, TokenType},
    strings::Strings,
};

use std::collections::HashMap;
use std::{convert::TryFrom, mem};

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

#[derive(Copy, Clone)]
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
    // chunk move to function
    enclosing: Option<Box<Compiler<'a>>>,
    function: LoxFunction,
    function_type: FunctionType,
    locals: Vec<Local<'a>>,
    scope_depth: i32,
}

impl<'a> Compiler<'a> {
    fn new(enclosing: Option<Box<Compiler<'a>>>, kind: FunctionType) -> Box<Self> {
        let mut compiler = Compiler {
            enclosing,
            function: LoxFunction::new(),
            function_type: kind,
            locals: Vec::with_capacity(LOCAL_COUNT),
            scope_depth: 0,
        };

        let token = Token {
            kind: TokenType::Error,
            lexeme: "",
            line: 0,
        };
        compiler.locals.push(Local::new(token, 0));

        Box::new(compiler)
    }
}

pub struct Parser<'a> {
    scanner: Scanner<'a>,
    compiler: Box<Compiler<'a>>,
    strings: &'a mut Strings,
    functions: &'a mut Functions,
    current: Token<'a>,
    previous: Token<'a>,
    had_error: bool,
    panic_mode: bool,
    rules: HashMap<TokenType, ParseRule<'a>>,
}

impl<'a> Parser<'a> {
    pub fn new(
        code: &'a str,
        strings: &'a mut Strings,
        functions: &'a mut Functions,
    ) -> Parser<'a> {
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
            Some(Parser::call),
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
        rule(TokenType::And, None, Some(Parser::and_op), Precedence::And);
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
        rule(TokenType::Or, None, Some(Parser::or_op), Precedence::Or);
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
            compiler: Compiler::new(None, FunctionType::Script),
            strings,
            functions,
            current: t1,
            previous: t2,
            had_error: false,
            panic_mode: false,
            rules,
        }
    }

    // compile return a function which contain chunk
    pub fn compile(mut self) -> Result<FunctionID, LoxError> {
        self.advance();
        while !self.matches(TokenType::Eof) {
            self.declaration();
        }
        self.emit(Instruction::Return);

        #[cfg(debug_assertions)]
        if !self.had_error {
            self.compiler.function.chunk.disassemble("code");
        }

        if self.had_error {
            Err(LoxError::CompileError)
        } else {
            Ok(self.functions.store(self.compiler.function))
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
        if self.matches(TokenType::Fun) {
            self.fun_declaration();
        } else if self.matches(TokenType::Var) {
            self.var_declaration();
        } else {
            self.statement();
        }

        if self.panic_mode {
            self.synchronize();
        }
    }

    fn fun_declaration(&mut self) {
        let global = self.parse_variable("Expect function name.");
        self.mark_initialized();
        self.function(FunctionType::Function);
        self.define_variable_global(global);
    }

    fn push_compiler(&mut self, kind: FunctionType) {
        let new_compiler = Compiler::new(None, kind);
        let old_compiler = mem::replace(&mut self.compiler, new_compiler);
        self.compiler.enclosing = Some(old_compiler);
        let function_name = self.strings.intern(self.previous.lexeme);

        self.compiler.function.name = function_name;
    }

    fn pop_compiler(&mut self) -> LoxFunction {
        self.emit(Instruction::Nil);
        self.emit(Instruction::Return);
        match self.compiler.enclosing.take() {
            Some(enclosing) => {
                let compiler = mem::replace(&mut self.compiler, enclosing);
                compiler.function
            }
            None => panic!("Didn't find an enclosing compiler"),
        }
    }

    fn function(&mut self, kind: FunctionType) {
        self.push_compiler(kind);
        self.begin_scope();
        self.consume(TokenType::LeftParen, "Expect '(' after function name.");
        // fun args
        if !self.check(TokenType::RightParen) {
            loop {
                self.compiler.function.arity += 1;
                if self.compiler.function.arity > 255 {
                    self.error_at_current("Cannot have more than 255 parameters.");
                }
                let param = self.parse_variable("Expect paramter name.");
                self.define_variable_global(param);
                if !self.matches(TokenType::Comma) {
                    break;
                }
            }
        }
        // arg end
        self.consume(TokenType::RightParen, "Expect ')' after parameters.");
        self.consume(TokenType::LeftBrace, "Expect '{' before function body.");
        self.block();
        let function = self.pop_compiler();
        let fn_id = self.functions.store(function);
        self.emit_constant(Value::Function(fn_id));
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
        } else if self.matches(TokenType::If) {
            self.if_statement();
        } else if self.matches(TokenType::While) {
            self.while_statement();
        } else if self.matches(TokenType::For) {
            self.for_statement();
        } else if self.matches(TokenType::LeftBrace) {
            self.begin_scope();
            self.block();
            self.end_scope();
        } else {
            self.expression_statement();
        }
    }

    fn for_statement(&mut self) {
        self.begin_scope();
        self.consume(TokenType::LeftParen, "Expect '(' after 'for'.");

        // initializer
        if self.matches(TokenType::Semicolon) {
            // no initializer
        } else if self.matches(TokenType::Var) {
            self.var_declaration();
        } else {
            self.expression_statement();
        }
        // loop start after initializer
        let mut loop_start = self.start_loop();

        // condition
        let mut exit_jump = Option::None;
        if !self.matches(TokenType::Semicolon) {
            self.expression();
            self.consume(TokenType::Semicolon, "Expect ';' after loop condition.");
            let jump = self.emit(Instruction::JumpIfFalse(0xffff));
            exit_jump = Option::from(jump);
            self.emit(Instruction::Pop);
        }

        // increments
        if !self.matches(TokenType::RightParen) {
            let body_jump = self.emit(Instruction::Jump(0xffff));
            let increment_start = self.start_loop();
            self.expression();
            self.emit(Instruction::Pop);
            self.consume(TokenType::RightParen, "Expect ')' after for clauses.");
            self.emit_loop(loop_start);
            // hack loop start and let the jump in body first goto increment
            // and in increment we chain loop back to start
            loop_start = increment_start;
            self.patch_jump(body_jump);
        }

        self.statement();
        self.emit_loop(loop_start);
        if let Option::Some(exit_jump) = exit_jump {
            self.patch_jump(exit_jump);
            self.emit(Instruction::Pop);
        }
        self.end_scope();
    }

    fn while_statement(&mut self) {
        let loop_start = self.start_loop();
        self.consume(TokenType::LeftParen, "Expect '(' after 'while'.");
        self.expression();
        self.consume(TokenType::RightParen, "Expect ')' after condition");
        // guard
        let exit_jump = self.emit(Instruction::JumpIfFalse(0xffff));
        self.emit(Instruction::Pop);
        self.statement();
        self.emit_loop(loop_start);
        self.patch_jump(exit_jump);
        self.emit(Instruction::Pop);
    }

    fn if_statement(&mut self) {
        self.consume(TokenType::LeftParen, "Expect '(' after 'if'.");
        self.expression();
        self.consume(TokenType::RightParen, "Expect ')' after condition.");
        let then_jump = self.emit(Instruction::JumpIfFalse(0xffff));
        self.emit(Instruction::Pop);
        self.statement();
        let else_jump = self.emit(Instruction::Jump(0xffff));
        self.patch_jump(then_jump);
        self.emit(Instruction::Pop);
        if self.matches(TokenType::Else) {
            self.statement();
        }
        self.patch_jump(else_jump);
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
        };
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
            self.emit(set_op);
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

    fn call(&mut self, _can_assign: bool) {
        let arg_cnt = self.argument_list() as usize;
        self.emit(Instruction::Call(arg_cnt));
    }

    fn argument_list(&mut self) -> usize {
        let mut count = 0;
        if !self.check(TokenType::RightParen) {
            loop {
                self.expression();
                if count == 255 {
                    self.error("Cannot have more than 255 arguments.");
                }
                count += 1;

                if !self.matches(TokenType::Comma) {
                    break;
                }
            }
        }
        self.consume(TokenType::RightParen, "Expect ')' after arguments.");

        count
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
        };
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
        };
    }

    fn and_op(&mut self, _can_assign: bool) {
        let false_jump = self.emit(Instruction::JumpIfFalse(0xffff));
        // ok, if we execute this instruction, means previous bool value is true
        // so we pop the true value because it may not the final and value
        // and eval next
        self.emit(Instruction::Pop);
        self.parse_precedence(Precedence::And);
        self.patch_jump(false_jump);
        // leave the false value on the stack
    }
    fn or_op(&mut self, _can_assign: bool) {
        // it's a 'guard' for true jump, when the value is true
        // we bypass all the following or instruction and leave this
        // true value on stack
        // else we ignore the true jump and eval next
        let false_jump = self.emit(Instruction::JumpIfFalse(0xffff));
        let true_jump = self.emit(Instruction::Jump(0xffff));
        self.patch_jump(false_jump);
        self.emit(Instruction::Pop);
        self.parse_precedence(Precedence::Or);
        self.patch_jump(true_jump);
        // leave the true value on the stack
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

    fn emit(&mut self, instruction: Instruction) -> usize {
        self.compiler
            .function
            .chunk
            .write(instruction, self.previous.line)
    }

    fn emit_two(&mut self, i1: Instruction, i2: Instruction) -> usize {
        self.compiler.function.chunk.write(i1, self.previous.line);
        self.compiler.function.chunk.write(i2, self.previous.line)
    }
    fn start_loop(&self) -> usize {
        self.compiler.function.chunk.code.len()
    }

    fn emit_loop(&mut self, start_pos: usize) {
        // don't contain the loop instruction
        let offset = self.compiler.function.chunk.code.len() - start_pos;
        let offset = match u16::try_from(offset) {
            Ok(o) => o,
            Err(_) => {
                self.error("Loop body too large.");
                0xffff
            }
        };
        self.emit(Instruction::Loop(offset));
    }

    fn patch_jump(&mut self, pos: usize) {
        let offset = self.compiler.function.chunk.code.len() - 1 - pos;
        let offset = match u16::try_from(offset) {
            Ok(offset) => offset,
            Err(_) => {
                self.error("Too much code to jump over.");
                0xffff
            }
        };

        match self.compiler.function.chunk.code[pos] {
            Instruction::JumpIfFalse(ref mut o) => *o = offset,
            Instruction::Jump(ref mut o) => *o = offset,
            _ => panic!("Instruction at position is not jump"),
        }
    }

    fn make_constant(&mut self, value: Value) -> usize {
        let index = self.compiler.function.chunk.add_constant(value);
        let index = match u8::try_from(index) {
            Ok(index) => index,
            Err(_) => {
                self.error("Too many constants in one chunk.");
                0
            }
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
