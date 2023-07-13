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

static bool isAtEnd() { return *scanner.current_ == '\0'; }

static Token makeToken(TokenType type) {
  Token ret;
  ret.type_ = type;
  ret.start_ = scanner.start_;
  ret.length_ = (int)(scanner.current_ - scanner.start_);
  ret.line_ = scanner.line_;

  return ret;
}

// side effect!
static char advance() {
  scanner.current_++;

  return scanner.current_[-1];
}
// have side effect!
static bool match(char expected) {
  if (isAtEnd()) {
    return false;
  }
  if (*scanner.current_ != expected) {
    return false;
  }
  scanner.current_++;

  return true;
}

static Token errorToken(const char *message) {
  Token ret;
  ret.type_ = TOKEN_ERROR;
  // point to here?
  ret.start_ = message;
  ret.length_ = (int)strlen(message);
  ret.line_ = scanner.line_;

  return ret;
}

static char peekNext() {
  if (isAtEnd()) {
    return '\0';
  }

  return scanner.current_[1];
}

static char peek() { return *scanner.current_; }
static void skipWhitespace() {
  while (true) {
    char c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      case '\n':
        scanner.line_++;
        advance();
        break;
      case '/':
        // a comment
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) {
            advance();
          }
        } else {
          return;
        }
        break;
      default:
        // do nothing
        return;
    }
  }
}

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      scanner.line_++;
    }
    advance();
  }
  if (isAtEnd()) {
    return errorToken("Unterminated string.");
  }
  // skip the closing '"'
  advance();

  return makeToken(TOKEN_STRING);
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static Token number() {
  while (isDigit(peek())) {
    advance();
  }
  // fraction part
  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) {
      advance();
    }
  }

  return makeToken(TOKEN_NUMBER);
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static TokenType identifierType() { return TOKEN_IDENTIFIER; }

static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek())) {
    advance();
  }
  return makeToken(identifierType());
}

Token scanToken() {
  skipWhitespace();
  scanner.start_ = scanner.current_;
  if (isAtEnd()) {
    return makeToken(TOKEN_EOF);
  }
  char c = advance();
  if (isAlpha(c)) {
    return identifier();
  }
  if (isDigit(c)) {
    return number();
  }
  switch (c) {
    case '(':
      return makeToken(TOKEN_LEFT_PAREN);
    case ')':
      return makeToken(TOKEN_RIGHT_PAREN);
    case '{':
      return makeToken(TOKEN_LEFT_BRACE);
    case '}':
      return makeToken(TOKEN_RIGHT_BRACE);
    case ';':
      return makeToken(TOKEN_SEMICOLON);
    case ',':
      return makeToken(TOKEN_COMMA);
    case '.':
      return makeToken(TOKEN_DOT);
    case '-':
      return makeToken(TOKEN_MINUS);
    case '+':
      return makeToken(TOKEN_PLUS);
    case '/':
      return makeToken(TOKEN_SLASH);
    case '*':
      return makeToken(TOKEN_STAR);
    case '!':
      if (match('=')) {
        return makeToken(TOKEN_BANG_EQUAL);
      }
      return makeToken(TOKEN_BANG);
    case '=':
      if (match('=')) {
        return makeToken(TOKEN_EQUAL_EQUAL);
      }
      return makeToken(TOKEN_EQUAL);
    case '<':
      if (match('=')) {
        return makeToken(TOKEN_LESS_EQUAL);
      }
      return makeToken(TOKEN_LESS);
    case '>':
      if (match('=')) {
        return makeToken(TOKEN_GREATER_EQUAL);
      }
      return makeToken(TOKEN_GREATER);
    case '"':
      return string();
  }

  return errorToken("Unexpected character.");
}

