#include "scanner.h"

#include <stdio.h>
#include <string.h>

#include "common.h"

typedef struct {
  const char *start_;
  const char *current_;
  int line_;
} Scanner;

Scanner scanner;

void initScanner(const char *source) {
  scanner.start_ = source;
  scanner.current_ = source;
  scanner.line_ = 1;
}
