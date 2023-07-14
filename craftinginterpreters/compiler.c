#include "compiler.h"

#include <stdio.h>

#include "common.h"
#include "scanner.h"

bool compile(const char *source, Chunk *c) {
  initScanner(source);

  int line = -1;
  while (true) {
    Token token = scanToken();
    if (token.line_ != line) {
      printf("%4d ", token.line_);
      line = token.line_;
    } else {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.type_, token.length_, token.start_);
    if (token.type_ == TOKEN_EOF) {
      break;
    }
  }

  return true;
}
