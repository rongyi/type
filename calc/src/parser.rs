use crate::ast::Node;
use crate::lexer::Lexer;
use crate::token::Token;

pub fn expr(input: &str) -> Result<Node, String> {
    let mut lexer = Lexer::new(input);
    expr_bp(&mut lexer, 0)
}

fn expr_bp(lexer: &mut Lexer, min_bp: u8) -> Result<Node, String> {
    let mut lhs = match lexer.next() {
        Some(Token::Number(i)) => Ok(Node::Number(i)),
        Some(op @ Token::Minus) => {
            let (_, r_bp) = op.preix_binding_power().unwrap();
            let lhs = expr_bp(lexer, r_bp)?;
            Ok(Node::Negate(Box::new(lhs)))
        }
        Some(op @ Token::LeftParen) => {
            let (_, r_bp) = op.preix_binding_power().unwrap();
            let lhs = expr_bp(lexer, r_bp);
            assert_eq!(lexer.next().unwrap(), Token::RightParen);
            lhs
        }
        _ => Err("bad prefix op".to_string()),
    };

    loop {
        let op = match lexer.expr.peek() {
            Some('+') => Token::Plus,
            Some('-') => Token::Minus,
            Some('*') => Token::Star,
            Some('/') => Token::Slash,
            Some('^') => Token::Caret,

            _ => Token::Eof,
        };

        if let Some((l_bp, r_bp)) = op.infix_binding_power() {
            if l_bp < min_bp {
                break;
            }
            // consume operator
            lexer.next();
            let rhs = expr_bp(lexer, r_bp)?;
            lhs = match op {
                Token::Plus => Ok(Node::Add(Box::new(lhs?), Box::new(rhs))),
                Token::Minus => Ok(Node::Sub(Box::new(lhs?), Box::new(rhs))),
                Token::Star => Ok(Node::Multiply(Box::new(lhs?), Box::new(rhs))),
                Token::Slash => Ok(Node::Divide(Box::new(lhs?), Box::new(rhs))),
                Token::Caret => Ok(Node::Power(Box::new(lhs?), Box::new(rhs))),
                _ => Err("bad infix operator".to_string()),
            }
        } else {
            break;
        }
    }

    lhs
}
