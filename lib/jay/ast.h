#ifndef JAYVM_AST_H
#define JAYVM_AST_H

enum jy_ast {
	AST_NONE = -1,
	AST_ROOT,

	AST_RULE,
	AST_IMPORT,
	AST_INCLUDE,
	AST_INGRESS,

	AST_TARGET,
	AST_INPUT,
	AST_MATCH,
	AST_CONDITION,
	AST_FIELDS,

	AST_ALIAS,
	AST_EVENT,
	AST_MEMBER,
	AST_CALL,

	AST_REGMATCH,
	AST_EQUALITY,
	AST_LESSER,
	AST_GREATER,

	AST_NOT,

	AST_ADDITION,
	AST_SUBTRACT,
	AST_MULTIPLY,
	AST_DIVIDE,

	AST_NAME,
	AST_PATH,

	AST_REGEXP,
	AST_LONG,
	AST_STRING,
	AST_FALSE,
	AST_TRUE,
};

#endif // JAYVM_AST_H