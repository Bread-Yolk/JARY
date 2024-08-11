#ifndef JAYVM_TOKEN_H
#define JAYVM_TOKEN_H

#include <stddef.h>
#include <stdint.h>

#define TKN Tkn
#define AS_PTKN(token) (Tkn*)&token

typedef enum {
    TKN_ERR,

    TKN_LEFT_PAREN,
    TKN_RIGHT_PAREN,
    TKN_LEFT_BRACE,
    TKN_RIGHT_BRACE,
    TKN_COMMA,
    TKN_COLON,
    TKN_NEWLINE,

    TKN_TARGET,
    TKN_INPUT,
    TKN_MATCH,
    TKN_CONDITION,

    TKN_RULE,
    TKN_IMPORT,
    TKN_INGRESS,

// < OPERATOR SYMBOL
    TKN_EQUAL,
    TKN_LESSTHAN,
    TKN_GREATERTHAN,
    TKN_ANY,
    TKN_AND,
    TKN_OR,
    TKN_ALL,
// > OPERATOR SYMBOL

// < LITERAL 
    TKN_REGEXP,
    TKN_STRING,
    TKN_NUMBER,
    TKN_FALSE,
    TKN_TRUE,
// > LITERAL

    TKN_IDENTIFIER,
    TKN_PVAR,

    TKN_CUSTOM,
    TKN_EOF,
} TknType;

typedef struct {
    TknType type;
    size_t length; // Does not count the quotes ("")
    size_t line;
    char* start;
    uint32_t hash;
} Tkn;

size_t tkn_lexeme_size(TKN* token);

// set a cstr representation of the token lexeme
bool tkn_lexeme(TKN* token, char *lexeme, size_t lexeme_size);


#endif // JAYVM_TOKEN_H