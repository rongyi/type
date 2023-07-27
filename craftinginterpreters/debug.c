#include "debug.h"

#include "stdio.h"
#include "value.h"

static int constantInstruction(const char *name, Chunk *c, int offset) {
  uint8_t vidx = c->code[offset + 1];
  printf("%-16s %4d '", name, vidx);
  printValue(c->constants.values[vidx]);
  printf("'\n");

  return offset + 2;
}

static int constantLongInstruction(const char *name, Chunk *c, int offset) {
  int vidx = (c->code[offset + 3] << 16) | (c->code[offset + 2] << 8) |
             (c->code[offset + 1]);
  printf("%-16s %4d '", name, vidx);
  printValue(c->constants.values[vidx]);
  printf("'\n");

  return offset + 4;
}

void disassembleChunk(Chunk *c, const char *name) {
  printf("== %s ==\n", name);
  for (int i = 0; i < c->count;) {
    i = disassembleInstruction(c, i);
  }
}
static int byteInstruction(const char *name, Chunk *c, int offset) {
  uint8_t slot = c->code[offset + 1];
  printf("%-16s %4d\n", name, slot);
  return offset + 2;
}

static int jumpInstruction(const char *name, int sign, Chunk *c, int offset) {
  uint16_t jump = (uint16_t)(c->code[offset + 1] | (c->code[offset + 2] << 8));
  printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);

  return offset + 3;
}

int disassembleInstruction(Chunk *c, int offset) {
  printf("%04d ", offset);
  if (offset > 0 && c->lines[offset] == c->lines[offset - 1]) {
    printf("   | ");
  } else {
    printf("%4d ", c->lines[offset]);
  }
  uint8_t instruction = c->code[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", c, offset);
    case OP_NIL:
      return simpleInstruction("OP_NIL", offset);
    case OP_TRUE:
      return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
      return simpleInstruction("OP_FALSE", offset);
    case OP_ADD:
      return simpleInstruction("OP_ADD", offset);
    case OP_SUBSTRACT:
      return simpleInstruction("OP_SUBSTRACT", offset);
    case OP_MULTIPLY:
      return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
      return simpleInstruction("OP_DIVIDE", offset);
    case OP_NOT:
      return simpleInstruction("OP_NOT", offset);
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", c, offset);
    case OP_NEGATE:
      return simpleInstruction("OP_NEGATE", offset);
    case OP_EQUAL:
      return simpleInstruction("OP_EQUAL", offset);
    case OP_GREATER:
      return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
      return simpleInstruction("OP_LESS", offset);
    case OP_PRINT:
      return simpleInstruction("OP_PRINT", offset);
    case OP_POP:
      return simpleInstruction("OP_POP", offset);
    case OP_DEFINE_GLOBAL:
      return constantInstruction("OP_DEFINE_GLOBAL", c, offset);
    case OP_GET_GLOBAL:
      return constantInstruction("OP_GET_GLOBAL", c, offset);
    case OP_SET_GLOBAL:
      return constantInstruction("OP_SET_GLOBAL", c, offset);
    case OP_GET_LOCAL:
      return byteInstruction("OP_GET_LOCAL", c, offset);
    case OP_SET_LOCAL:
      return byteInstruction("OP_SET_LOCAL", c, offset);
    case OP_JUMP_IF_FALSE:
      return jumpInstruction("OP_JUMP_IF_FALSE", 1, c, offset);
    case OP_JUMP:
      return jumpInstruction("OP_JUMP", 1, c, offset);
    case OP_LOOP:
      return jumpInstruction("OP_LOOP", -1, c, offset);
    default:
      printf("Unkown opcode %d\n", instruction);
      return offset + 1;
  }
}

int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);

  return offset + 1;
}
