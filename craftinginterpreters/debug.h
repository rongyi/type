#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

void disassembleChunk(Chunk *c, const char *name);
int disassembleInstruction(Chunk *c, int offset);

int simpleInstruction(const char *name, int offset);

#endif
