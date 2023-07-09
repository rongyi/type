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
  int vidx = (c->code[offset + 3] << 16) | (c->code[offset + 2] << 8) | (c->code[offset + 1]);
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
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", c, offset);
    default:
      printf("Unkown opcode %d\n", instruction);
      return offset + 1;
  }
}

int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);

  return offset + 1;
}
