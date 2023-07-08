#include "common.h"
#include "chunk.h"
#include "debug.h"


int main(int argc, const char *argv[]) {
  Chunk c;
  initChunk(&c);
  int idx = addConstants(&c, 3.14);
  writeChunk(&c, OP_CONSTANT);
  writeChunk(&c, idx);

  writeChunk(&c, OP_RETURN);
  disassembleChunk(&c, "test chunk");
  freeChunk(&c);

  return 0;
}
