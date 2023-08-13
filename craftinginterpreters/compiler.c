#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef void (*ParseFn)(bool can_assign);

typedef struct {
  ParseFn prefix_;
  ParseFn infix_;
  Precedence prec_;
} PraseRule;

typedef struct {
  Token name_;
  int depth_;
} Local;

// indicate function is global or local
typedef enum {
  TYPE_FUNCTION,
  TYPE_SCRIPT,
} FunctionType;

typedef struct Compiler {
  struct Compiler *enclosing_;  // outlayer compiler
  ObjFunction *function_;
  FunctionType fun_type_;
  // locals_ save variable name
  Local locals_[UINT8_COUNT];
  int local_count_;
  int scope_depth_;
} Compiler;

Parser parser;
Compiler *currentCompiler = NULL;
static Chunk *currentChunk() { return &currentCompiler->function_->chunk_; }

static void expression();
static PraseRule *getRule(TokenType t);
static void parsePrecedence(Precedence prec);
static void literal(bool can_assign);
static void statement();
static void declaration();
static void variable(bool can_assign);
static bool match(TokenType type);
static int emitJump(OpCode code);
static void patchJump(int jump_pos);
static void varDeclaration();
static bool check(TokenType type);

static void initCompiler(Compiler *c, FunctionType type) {
  c->enclosing_ = currentCompiler;
  c->function_ = NULL;
  c->fun_type_ = type;
  c->local_count_ = 0;
  c->scope_depth_ = 0;

  c->function_ = newFuction();

  currentCompiler = c;
  // need a name
  if (type != TYPE_SCRIPT) {
    currentCompiler->function_->name_ =
        copyString(parser.previous_.start_, parser.previous_.length_);
  }

  // why?
  // to take the first index as function name
  Local *local = &currentCompiler->locals_[currentCompiler->local_count_++];
  local->depth_ = 0;
  local->name_.start_ = "";
  local->name_.length_ = 0;
}

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

static void emitReturn() {
  emitByte(OP_NIL);
  emitByte(OP_RETURN);
}

static ObjFunction *endCompiler() {
  emitReturn();
  ObjFunction *ret = currentCompiler->function_;
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError_) {
    disassembleChunk(currentChunk(),
                     ret->name_ != NULL ? ret->name_->chars_ : "<script>");
  }
#endif
  // base line
  currentCompiler = currentCompiler->enclosing_;
  return ret;
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

static void number(bool can_assign) {
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
  bool can_assign = prec <= PREC_ASSIGNMENT;
  prefixRule(can_assign);

  // not consumed =, shit this is not right
  if (can_assign && match(TOKEN_EQUAL)) {
    error("Invalid assignment target");
  }

  while (prec <= getRule(parser.current_.type_)->prec_) {
    advance();
    ParseFn infixRule = getRule(parser.previous_.type_)->infix_;
    infixRule(can_assign);
  }
}

static void expression() { parsePrecedence(PREC_ASSIGNMENT); }

static void grouping(bool can_assign) {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ) after expression");
}

static void unary(bool can_assign) {
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

static void binary(bool can_assign) {
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

static void string(bool can_assign) {
  emitConstant(OBJ_VAL(
      copyString(parser.previous_.start_ + 1, parser.previous_.length_ - 2)));
}

static void and_(bool can_assign) {
  // short cut, false, jump to end;
  // when we meet false, no execution for the lest expression
  int endJump = emitJump(OP_JUMP_IF_FALSE);
  // the current true value, pop it
  emitByte(OP_POP);
  parsePrecedence(PREC_AND);
  patchJump(endJump);
}

static void or_(bool can_assign) {
  // true will jump to end
  // else don't jump, but how todo
  // we put a guard before this jump
  // that is false jump
  int else_jump = emitJump(OP_JUMP_IF_FALSE);
  int end_jump = emitJump(OP_JUMP);
  patchJump(else_jump);
  // the true value in stack top
  emitByte(OP_POP);

  parsePrecedence(PREC_OR);
  patchJump(end_jump);
}

static uint8_t argumentList() {
  uint8_t arg_cnt = 0;
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      expression();
      if (arg_cnt == 255) {
        error("Can't have more than 255 arguments.");
      }
      arg_cnt++;
    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return arg_cnt;
}

static void call(bool can_assign) {
  uint8_t arg_cnt = argumentList();
  emitBytes(OP_CALL, arg_cnt);
}

PraseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, call, PREC_CALL},
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
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, and_, PREC_AND},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, or_, PREC_OR},
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

static void literal(bool can_assign) {
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

static void block() {
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    declaration();
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}
static void beginScope() { currentCompiler->scope_depth_++; }
static void endScope() {
  // current scope_depth_ minus one
  currentCompiler->scope_depth_--;

  // pop current sccope
  while (currentCompiler->local_count_ > 0 &&
         currentCompiler->locals_[currentCompiler->local_count_ - 1].depth_ >
             currentCompiler->scope_depth_) {
    emitByte(OP_POP);
    currentCompiler->local_count_--;
  }
}

// return the jump code start pos
// |jmp|  |  |
//   ^
static int emitJump(OpCode code) {
  emitByte(code);
  emitByte(0xff);
  emitByte(0xff);

  return currentChunk()->count - 2;
}

static void patchJump(int jump_pos) {
  //  current - (jump_pos + 2)
  //  the instruction need to jump
  int real_step = currentChunk()->count - jump_pos - 2;
  if (real_step > UINT16_MAX) {
    error("Too much code to jump over");
  }
  // this is different with book, I like bigendian
  currentChunk()->code[jump_pos] = (real_step & 0xff);
  currentChunk()->code[jump_pos + 1] = ((real_step >> 8) & 0xff);
}

static void ifStatement() {
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'if'");

  int then_jump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();
  int else_jump = emitJump(OP_JUMP);
  patchJump(then_jump);

  emitByte(OP_POP);
  if (match(TOKEN_ELSE)) {
    statement();
  }
  patchJump(else_jump);
}
static void emitLoop(int loop_start) {
  emitByte(OP_LOOP);
  // and there are two uint8_t of operand
  // we dont pushed in, but we count them before
  int offset = currentChunk()->count + 2 - loop_start;
  if (offset > UINT16_MAX) {
    error("Loop body too large");
  }
  // big endian
  emitByte(offset & 0xff);
  emitByte((offset >> 8) & 0xff);
}

static void whileStatement() {
  int loop_start = currentChunk()->count;
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'while'.");

  int exit_jump = emitJump(OP_JUMP_IF_FALSE);
  // true action, 1. pop expression value
  emitByte(OP_POP);
  statement();
  emitLoop(loop_start);

  patchJump(exit_jump);
  emitByte(OP_POP);
}

static void forStatement() {
  // a simple dead loop: for(;;)
  beginScope();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'");
  // initializer part
  if (match(TOKEN_SEMICOLON)) {
    // no initializer
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    expressionStatement();
  }
  /*consume(TOKEN_SEMICOLON, "Expect ';'.");*/
  int loop_start = currentChunk()->count;

  // condition part
  int exit_jump = -1;
  if (!match(TOKEN_SEMICOLON)) {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    exit_jump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);  // condition val in stack pop
  }

  if (!match(TOKEN_RIGHT_PAREN)) {
    // we compile increment part, but we don't execute this part
    // we need first go to body part,
    // so here is a unconditional jump to body
    int body_jump = emitJump(OP_JUMP);
    int increment_start = currentChunk()->count;
    expression();
    emitByte(OP_POP);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

    // go to the condition part after increment execution
    emitLoop(loop_start);
    // this value is hacked,
    // after the body, we goto increment part
    loop_start = increment_start;
    // when patchJump shows, means we need to jump all the instruction above
    patchJump(body_jump);
  }

  /*consume(TOKEN_SEMICOLON, "Expect ';'.");*/
  /*consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clause.");*/

  // body
  statement();
  // so after the body, we go to loop_start
  // but this value is hacked in increment part
  // let's see
  emitLoop(loop_start);
  // after the body and loop, means we jump to end
  if (exit_jump != -1) {
    patchJump(exit_jump);
    emitByte(OP_POP);  // condition val
  }
  endScope();
}

static void returnStatement() {
  if (currentCompiler->fun_type_ == TYPE_SCRIPT) {
    error("Can't return from top level code.");
  }
  if (match(TOKEN_SEMICOLON)) {
    emitReturn();
  } else {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    emitByte(OP_RETURN);
  }
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
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_RETURN)) {
    returnStatement();
  } else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
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

static void addLocal(Token name) {
  if (currentCompiler->local_count_ == UINT8_COUNT) {
    error("Too many local variable in function");
    return;
  }
  Local *local = &currentCompiler->locals_[currentCompiler->local_count_++];
  // only record their token name, no binding yet;
  // so where is the binding take place?
  local->name_ = name;
  /*local->depth_ = currentCompiler->scope_depth_;*/
  // mark as uninitialized
  local->depth_ = -1;
}

static bool identifierEqual(Token *a, Token *b) {
  if (a->length_ != b->length_) {
    return false;
  }

  return memcmp(a->start_, b->start_, a->length_) == 0;
}

static void declareVariable() {
  // global stat, this function is to service local variable
  if (currentCompiler->scope_depth_ == 0) {
    return;
  }
  Token *name = &parser.previous_;
  for (int i = currentCompiler->local_count_ - 1; i >= 0; i--) {
    Local *cur = &currentCompiler->locals_[i];
    // -1 means?
    // to another level, we are done, break it
    if (cur->depth_ != -1 && cur->depth_ < currentCompiler->scope_depth_) {
      break;
    }
    if (identifierEqual(name, &cur->name_)) {
      error("Already a variable with this name in this sccope");
    }
  }
  addLocal(*name);
}

static uint8_t parseVariable(const char *errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);

  declareVariable();
  // return a dummy index;
  if (currentCompiler->scope_depth_ > 0) {
    return 0;
  }

  return identifierConstant(&parser.previous_);
}

static void markInitialized() {
  if (currentCompiler->scope_depth_ == 0) {
    return;
  }
  currentCompiler->locals_[currentCompiler->local_count_ - 1].depth_ =
      currentCompiler->scope_depth_;
}

static void defineVariable(uint8_t global_idx) {
  // don't emit global when in local stats
  if (currentCompiler->scope_depth_ > 0) {
    markInitialized();
    return;
  }
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

/* Whenever a variable is declared, we append it to the locals array in
 * Compiler. That means the first local variable is at index zero, the next one
 * is at index one, and so on. In other words, the locals array in the compiler
 * has the exact same layout as the VM’s stack will have at runtime. The
 * variable’s index in the locals array is the same as its stack slot. How
 * convenient!*/
static int resolveLocal(Compiler *c, Token *name) {
  for (int i = c->local_count_ - 1; i >= 0; i--) {
    Local *cur = &c->locals_[i];
    if (identifierEqual(name, &cur->name_)) {
      if (cur->depth_ == -1) {
        error("Can't read local variable in its own initializer.");
      }
      return i;
    }
  }

  return -1;
}

static void namedVariable(Token name, bool can_assign) {
  uint8_t get_op;
  uint8_t set_op;
  int idx = resolveLocal(currentCompiler, &name);
  if (idx != -1) {
    get_op = OP_GET_LOCAL;
    set_op = OP_SET_LOCAL;
  } else {
    idx = identifierConstant(&name);
    get_op = OP_GET_GLOBAL;
    set_op = OP_SET_GLOBAL;
  }

  if (can_assign && match(TOKEN_EQUAL)) {
    // the value part
    expression();
    emitBytes(set_op, (uint8_t)idx);
  } else {
    emitBytes(get_op, (uint8_t)idx);
  }
}

static void variable(bool can_assign) {
  namedVariable(parser.previous_, can_assign);
}

static void function(FunctionType type) {
  Compiler c;
  initCompiler(&c, type);
  beginScope();

  // no function args yet
  consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
  if (!check(TOKEN_RIGHT_PAREN)) {
    // so you understand do while?
    // function args
    do {
      currentCompiler->function_->arity_++;  // arguments
      if (currentCompiler->function_->arity_ > 255) {
        errorAtCurrent("Can't have more than 255 parameters.");
      }
      uint8_t idx = parseVariable("Expect parameter name.");
      defineVariable(idx);

    } while (match(TOKEN_COMMA));
  }
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' before function body");

  block();

  ObjFunction *ret = endCompiler();
  /*emitBytes(OP_CONSTANT, makeConstant(OBJ_VAL(ret)));*/
  emitBytes(OP_CLOSURE, makeConstant(OBJ_VAL(ret)));
}

static void funDeclaration() {
  uint8_t idx = parseVariable("Expect function name.");
  markInitialized();
  function(TYPE_FUNCTION);
  defineVariable(idx);
}

static void declaration() {
  if (match(TOKEN_VAR)) {
    varDeclaration();
  } else if (match(TOKEN_FUN)) {
    funDeclaration();
  } else {
    statement();
  }
  if (parser.panicMode_) {
    synchronize();
  }
}

ObjFunction *compile(const char *source) {
  initScanner(source);
  Compiler compiler;
  initCompiler(&compiler, TYPE_SCRIPT);

  parser.panicMode_ = false;
  parser.hadError_ = false;

  advance();
  /*expression();*/
  while (!match(TOKEN_EOF)) {
    declaration();
  }
  consume(TOKEN_EOF, "expect end of expression");
  ObjFunction *ret = endCompiler();

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

  if (parser.hadError_) {
    return NULL;
  }

  return ret;
}
