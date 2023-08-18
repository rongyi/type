use std::env;

type Value = f64;

#[derive(Debug, Clone, Copy)]
enum Instruction {
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
            Instruction::Return => println!("OP_RETURN"),
            Instruction::Constant(index) => {
                let value = self.constants[*index];
                println!("{:<16} {:4} {}", "OP_CONSTANT", index, value);
            }
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().map(|s| format!(r#""{}""#, s)).collect();
    println!("{}", args.join(", "));

    let mut c = Chunk::new();
    c.write(Instruction::Return, 12);
    let idx = c.add_constant(3.14);
    c.write(Instruction::Constant(idx), 119);
    let idx = c.add_constant(8.8);
    c.write(Instruction::Constant(idx), 119);
    c.disassemble("test chunk");
}
