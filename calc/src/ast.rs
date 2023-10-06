#![allow(dead_code)]

#[derive(Debug, Clone, PartialEq)]
pub enum Node {
    Number(f64),
    Add(Box<Node>, Box<Node>),
    Sub(Box<Node>, Box<Node>),
    Multiply(Box<Node>, Box<Node>),
    Divide(Box<Node>, Box<Node>),
    Power(Box<Node>, Box<Node>),
    Negate(Box<Node>),
}

pub fn eval(node: Node) -> f64 {
    match node {
        Node::Number(i) => i,
        Node::Add(l, r) => eval(*l) + eval(*r),
        Node::Sub(l, r) => eval(*l) - eval(*r),
        Node::Multiply(l, r) => eval(*l) * eval(*r),
        Node::Divide(l, r) => eval(*l) / eval(*r),
        Node::Power(l, r) => eval(*l).powf(eval(*r)),
        Node::Negate(l) => -eval(*l),
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn eval_test() {
        let node = Node::Number(10.0);
        assert_eq!(eval(node), 10.0);
    }

    #[test]
    fn eval_test2() {
        let node = Node::Add(Box::new(Node::Number(10.0)), Box::new(Node::Number(20.0)));
        assert_eq!(eval(node), 30.0);
    }
}
