#![allow(dead_code)]

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Token {
    Number(f64),
    Plus,
    Minus,
    Star,
    Slash,
    Caret,
    LeftParen,
    RightParen,
    Eof,
}

impl Token {
    pub fn preix_binding_power(&self) -> Option<((), u8)> {
        match self {
            Token::Minus => Some(((), 7)),
            Token::LeftParen => Some(((), 0)),
            _ => None,
        }
    }
    pub fn infix_binding_power(&self) -> Option<(u8, u8)> {
        match self {
            Token::Plus | Token::Minus => Some((1, 2)),
            Token::Star | Token::Slash => Some((3, 4)),
            Token::Caret => Some((5, 6)),
            _ => None,
        }
    }
}
