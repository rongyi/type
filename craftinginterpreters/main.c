#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
  initVM();
  Chunk c;
  initChunk(&c);
  int idx = addConstants(&c, 3.14);
  // a code array to store instruction
  // a global array to store constant
  // a same size array to store line number, that't it
  writeChunk(&c, OP_CONSTANT, 2);
  writeChunk(&c, idx, 2);

  writeConstant(&c, 3.298, 2);
  writeConstant(&c, 3.398, 2);

  writeChunk(&c, OP_RETURN, 2);
  disassembleChunk(&c, "test chunk");
  interpret(&c);
  freeVM();
  freeChunk(&c);

  return 0;
}
