#![allow(dead_code)]
use std::iter::Peekable;
use std::str::Chars;

use crate::token::Token;

pub struct Lexer<'a> {
    pub expr: Peekable<Chars<'a>>,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        Lexer {
            expr: input.chars().peekable(),
        }
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Token;
    fn next(&mut self) -> Option<Self::Item> {
        let c = self.expr.next()?;
        match c {
            '0'..='9' => {
                let mut nums = c.to_string();
                let mut have_dot = false;
                while let Some(c) = self.expr.peek() {
                    if c.is_numeric() {
                        nums.push(self.expr.next()?);
                    } else if c == &'.' {
                        if have_dot {
                            return None;
                        }
                        have_dot = true;
                        nums.push(self.expr.next()?);
                    } else {
                        break;
                    }
                }
                if have_dot && nums.chars().last().unwrap() == '.' {
                    return None;
                }

                Some(Token::Number(nums.parse().unwrap()))
            }
            '+' => Some(Token::Plus),
            '-' => Some(Token::Minus),
            '*' => Some(Token::Star),
            '/' => Some(Token::Slash),
            '^' => Some(Token::Caret),
            '(' => Some(Token::LeftParen),
            ')' => Some(Token::RightParen),
            _ => None,
        }
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn number() {
        let mut l = Lexer::new("1.0");
        assert_eq!(l.next(), Some(Token::Number(1.0)));
        let mut l = Lexer::new("1.0.1");
        assert_eq!(l.next(), None);
        let mut l = Lexer::new("1.");
        assert_eq!(l.next(), None);
        let mut l = Lexer::new("300.1230");
        assert_eq!(l.next(), Some(Token::Number(300.1230)));
    }
    #[test]
    fn op() {
        let mut l = Lexer::new("+-*/^()");
        assert_eq!(l.next(), Some(Token::Plus));
        assert_eq!(l.next(), Some(Token::Minus));
        assert_eq!(l.next(), Some(Token::Star));
        assert_eq!(l.next(), Some(Token::Slash));
        assert_eq!(l.next(), Some(Token::Caret));
        assert_eq!(l.next(), Some(Token::LeftParen));
        assert_eq!(l.next(), Some(Token::RightParen));
    }
}
