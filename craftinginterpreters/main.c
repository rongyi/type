#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

static void repl() {
  char line[1024];
  while (true) {
    printf("> ");
    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }
    interpret(line);
  }
}

static char *readFile(const char *path) {
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    exit(74);
  }
  fseek(f, 0L, SEEK_END);
  size_t fsz = ftell(f);
  rewind(f);

  char *buffer = (char *)malloc(fsz + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read: %s\n", path);
    exit(74);
  }
  size_t readsz = fread(buffer, sizeof(char), fsz, f);
  if (readsz < fsz) {
    fprintf(stderr, "Could not read file: %s\n", path);
    exit(74);
  }
  buffer[readsz] = '\0';

  fclose(f);

  return buffer;
}

static void runFile(const char *path) {
  char *source = readFile(path);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) {
    exit(65);
  }
  if (result == INTERPRET_RUNTIME_ERROR) {
    exit(70);
  }
}

// new for compiler
int main(int argc, const char *argv[]) {
  initVM();
  /*interpret("var a = 10;");*/
  /*interpret("var b = 10;");*/
  /*interpret("a + b = 10;");*/

#if 1
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(1);
  }
#endif
  freeVM();
}

#if 0
int main1(int argc, const char *argv[]) {
  initVM();
  Chunk c;
  initChunk(&c);
  int idx = addConstants(&c, 1.2);
  // a code array to store instruction
  // a global array to store constant
  // a same size array to store line number, that't it
  writeChunk(&c, OP_CONSTANT, 2);
  writeChunk(&c, idx, 2);

  writeConstant(&c, 3.4, 2);

  /*writeChunk(&c, OP_NEGATE, 2);*/
  writeChunk(&c, OP_ADD, 2);

  writeConstant(&c, 5.6, 2);
  writeChunk(&c, OP_DIVIDE, 2);
  writeChunk(&c, OP_NEGATE, 2);

  writeChunk(&c, OP_RETURN, 2);
  disassembleChunk(&c, "test chunk");
  interpret(&c);
  freeVM();
  freeChunk(&c);

  return 0;
}
#endif
