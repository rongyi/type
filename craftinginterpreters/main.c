#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  Chunk c;
  initChunk(&c);
  int idx = addConstants(&c, 3.14);
  writeChunk(&c, OP_CONSTANT, 2);
  writeChunk(&c, idx, 2);

  writeConstant(&c, 3.298, 2);

  writeChunk(&c, OP_RETURN, 2);
  disassembleChunk(&c, "test chunk");
  freeChunk(&c);

  return 0;
}
