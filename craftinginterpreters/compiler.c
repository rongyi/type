#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  Token current_;
  Token previous_;
  bool hadError_;
  bool panicMode_;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // > < >= <=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix_;
  ParseFn infix_;
  Precedence prec_;
} PraseRule;

Parser parser;
Chunk *compilingChunk;
static Chunk *currentChunk() { return compilingChunk; }

static void expression();
static PraseRule *getRule(TokenType t);
static void parsePrecedence(Precedence prec);

static void errorAt(Token *token, const char *message) {
  if (parser.panicMode_) {
    return;
  }
  parser.panicMode_ = true;
  fprintf(stderr, "[line %d] Error", token->line_);
  if (token->type_ == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type_ == TOKEN_ERROR) {
    // do nothing
  } else {
    fprintf(stderr, " at '%.*s'", token->length_, token->start_);
  }
  fprintf(stderr, ": %s\n", message);
  parser.hadError_ = true;
}

static void errorAtCurrent(const char *message) {
  errorAt(&parser.current_, message);
}

static void error(const char *message) { errorAt(&parser.previous_, message); }

static void advance() {
  parser.previous_ = parser.current_;

  while (true) {
    parser.current_ = scanToken();
    if (parser.current_.type_ != TOKEN_ERROR) {
      break;
    }

    errorAtCurrent(parser.current_.start_);
  }
}

static void consume(TokenType type, const char *message) {
  if (parser.current_.type_ == type) {
    advance();
    return;
  }
  errorAtCurrent(message);
}

static void emitByte(uint8_t byte) {
  writeChunk(currentChunk(), byte, parser.previous_.line_);
}
static void emitReturn() { emitByte(OP_RETURN); }

static void endCompiler() { emitReturn(); }
static void emitBytes(uint8_t b1, uint8_t b2) {
  emitByte(b1);
  emitByte(b2);
}

static uint8_t makeConstant(Value v) {
  int idx = addConstants(currentChunk(), v);
  if (idx > UINT8_MAX) {
    error("Too many constants in one chunk");
    return 0;
  }

  return (uint8_t)idx;
}

static void emitConstant(Value v) { emitBytes(OP_CONSTANT, makeConstant(v)); }

static void number() {
  double value = strtod(parser.previous_.start_, NULL);
  emitConstant(value);
}

static void parsePrecedence(Precedence prec) {
  advance();
  ParseFn prefixRule = getRule(parser.previous_.type_)->prefix_;
  if (prefixRule == NULL) {
    error("Expect expression");
    return;
  }
  prefixRule();

  while (prec <= getRule(parser.current_.type_)->prec_) {
    advance();
    ParseFn infixRule = getRule(parser.previous_.type_)->infix_;
    infixRule();
  }
}

static void expression() { parsePrecedence(PREC_ASSIGNMENT); }

static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ) after expression");
}

static void unary() {
  TokenType op = parser.previous_.type_;

  parsePrecedence(PREC_UNARY);

  switch (op) {
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      return;
  }
}

static void binary() {
  TokenType op = parser.previous_.type_;
  PraseRule *rule = getRule(op);
  parsePrecedence((Precedence)(rule->prec_ + 1));

  switch (op) {
    case TOKEN_PLUS:
      emitByte(OP_ADD);
      break;
    case TOKEN_MINUS:
      emitByte(OP_SUBSTRACT);
      break;
    case TOKEN_STAR:
      emitByte(OP_MULTIPLY);
      break;
    case TOKEN_SLASH:
      emitByte(OP_DIVIDE);
      break;
    default:
      return;
  }
}

PraseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {NULL, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {NULL, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static PraseRule *getRule(TokenType t) { return &rules[t]; }

bool compile(const char *source, Chunk *c) {
  initScanner(source);
  compilingChunk = c;

  parser.panicMode_ = false;
  parser.hadError_ = false;

  advance();
  expression();
  consume(TOKEN_EOF, "expect end of expression");
  endCompiler();

  // for testing scanner
#if 0
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
#endif

  return !parser.hadError_;
}
