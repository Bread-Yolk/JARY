#ifndef JAYVM_TOKEN_H
#define JAYVM_TOKEN_H

enum jy_tkn {
	TKN_NONE = -1,
	TKN_ERR,
	TKN_ERR_STR,

	TKN_LEFT_PAREN,
	TKN_RIGHT_PAREN,
	TKN_LEFT_BRACE,
	TKN_RIGHT_BRACE,
	TKN_DOT,
	TKN_COMMA,
	TKN_COLON,
	TKN_NEWLINE,
	TKN_SPACES,

	TKN_RULE,
	TKN_IMPORT,
	TKN_INCLUDE,
	TKN_INGRESS,

	TKN_JUMP,
	TKN_INPUT,
	TKN_MATCH,
	TKN_CONDITION,
	TKN_FIELD,

	TKN_LONG_TYPE,
	TKN_STRING_TYPE,

	// < OPERATOR SYMBOL
	TKN_TILDE,

	TKN_PLUS,
	TKN_MINUS,
	TKN_STAR,
	TKN_SLASH,

	TKN_EQUAL,
	TKN_LESSTHAN,
	TKN_GREATERTHAN,
	TKN_AND,
	TKN_OR,
	TKN_NOT,
	TKN_ANY,
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
	TKN_DOLLAR,
	TKN_ALIAS,

	TKN_CUSTOM,
	TKN_EOF,
};

#endif // JAYVM_TOKEN_H

