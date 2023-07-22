#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "object.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

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
static void literal();
static void statement();
static void declaration();

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

static void endCompiler() {
  emitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError_) {
    disassembleChunk(currentChunk(), "code");
  }
#endif
}
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
  emitConstant(NUMBER_VAL(value));
}

// core part
// prefix + recursive + loop
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
    case TOKEN_BANG:
      emitByte(OP_NOT);
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
    case TOKEN_BANG_EQUAL:
      emitBytes(OP_EQUAL, OP_NOT);
      break;
    case TOKEN_EQUAL_EQUAL:
      emitByte(OP_EQUAL);
      break;
    case TOKEN_GREATER:
      emitByte(OP_GREATER);
      break;
    case TOKEN_GREATER_EQUAL:
      emitBytes(OP_LESS, OP_NOT);
      break;
    case TOKEN_LESS:
      emitByte(OP_LESS);
      break;
    case TOKEN_LESS_EQUAL:
      emitBytes(OP_GREATER, OP_NOT);
      break;
    default:
      return;
  }
}

static void string() {
  emitConstant(OBJ_VAL(
      copyString(parser.previous_.start_ + 1, parser.previous_.length_ - 2)));
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
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static PraseRule *getRule(TokenType t) { return &rules[t]; }

static void literal() {
  switch (parser.previous_.type_) {
    case TOKEN_FALSE:
      emitByte(OP_FALSE);
      break;
    case TOKEN_TRUE:
      emitByte(OP_TRUE);
      break;
    case TOKEN_NIL:
      emitByte(OP_NIL);
      break;
    default:
      return;
  }
}

static bool check(TokenType type) { return parser.current_.type_ == type; }

static bool match(TokenType type) {
  if (!check(type)) {
    return false;
  }
  advance();
  return true;
}

static void printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ; after value.");
  emitByte(OP_PRINT);
}

static void expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression");
  emitByte(OP_POP);
}

// Every bytecode instruction has a stack effect that describes how the
// instruction modifies the stack. For example, OP_ADD pops two values and
// pushes one, leaving the stack one element smaller than before.
//
// The bytecode for an entire statement has a total stack effect of zero. Since
// a statement produces no values, it ultimately leaves the stack unchanged,
// though it of course uses the stack while it’s doing its thing.
static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else {
    expressionStatement();
  }
}

static void synchronize() {
  parser.panicMode_ = false;
  while (parser.current_.type_ != TOKEN_EOF) {
    if (parser.previous_.type_ == TOKEN_SEMICOLON) {
      return;
    }
    switch (parser.current_.type_) {
      case TOKEN_CLASS:
      case TOKEN_FUN:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;
      default: {
        ;  // do nothing
      }
    }
  }
}

static uint8_t identifierConstant(Token *name) {
  return makeConstant(OBJ_VAL(copyString(name->start_, name->length_)));
}

static uint8_t parseVariable(const char *errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  return identifierConstant(&parser.previous_);
}

static void defineVariable(uint8_t global_idx) {
  emitBytes(OP_DEFINE_GLOBAL, global_idx);
}

static void varDeclaration() {
  uint8_t global_idx = parseVariable("Expect variable name.");
  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }
  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration");

  defineVariable(global_idx);
}

static void declaration() {
  if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    statement();
  }
  if (parser.panicMode_) {
    synchronize();
  }
}

bool compile(const char *source, Chunk *c) {
  initScanner(source);
  compilingChunk = c;

  parser.panicMode_ = false;
  parser.hadError_ = false;

  advance();
  /*expression();*/
  while (!match(TOKEN_EOF)) {
    declaration();
  }
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
