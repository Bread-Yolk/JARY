#include <gtest/gtest.h>

extern "C" {
#include "scanner.h"
}

TEST(ScannerTest, ScanEOF)
{
	char str[] = "\0\0";

	enum jy_tkn type;
	uint32_t    len	  = sizeof(str);
	const char *start = str, *end = NULL;
	end = jry_scan(str, len, &type);

	ASSERT_EQ(type, TKN_EOF);
	ASSERT_EQ(1, end - start);
}

TEST(ScannerTest, ScanNewline)
{
	char str[] = "\n\n\n\n\n";

	enum jy_tkn type;
	uint32_t    len	  = sizeof(str);
	const char *start = str, *end = NULL;
	end = jry_scan(str, len, &type);

	ASSERT_EQ(type, TKN_NEWLINE);
	ASSERT_EQ(memcmp(str, start, end - start), 0);
}

TEST(ScannerTest, ScanWhitespace)
{
	enum jy_tkn type;
	char	    str[] = "    \t\t\r\t  \r";
	int32_t	    len	  = sizeof(str);
	const char *start = str, *end = NULL;
	end = jry_scan(str, len, &type);

	ASSERT_EQ(type, TKN_SPACES);
	ASSERT_EQ(memcmp(str, start, end - start), 0);
}

TEST(ScannerTest, ScanSymbol)
{
	char str[]	    = "(){}==~<>:,$";

	enum jy_tkn types[] = {
		// group
		TKN_LEFT_PAREN,
		TKN_RIGHT_PAREN,
		// block
		TKN_LEFT_BRACE,
		TKN_RIGHT_BRACE,
		// operator
		TKN_EQUAL,
		TKN_TILDE,
		TKN_LESSTHAN,
		TKN_GREATERTHAN,
		TKN_COLON,
		TKN_COMMA,
		TKN_DOLLAR,
	};

	uint32_t length	  = sizeof(types) / sizeof(types[0]);

	const char *start = str;
	for (uint32_t i = 0; i < length; ++i) {
		enum jy_tkn type;
		start = jry_scan(start, sizeof(str), &type);

		ASSERT_EQ(type, types[i]) << "i: " << i << " lexeme: " << start;
	}
}

TEST(ScannerTest, ScanKeyword)
{
	const char *str[] = {
		"all",	 "and",	   "any",	"false",  "true",    "or",
		"not",	 "input",  "rule",	"import", "ingress", "include",
		"match", "target", "condition", "field",
	};

	enum jy_tkn types[] {
		TKN_ALL,
		TKN_AND,
		TKN_ANY,
		TKN_FALSE,
		TKN_TRUE,
		TKN_OR,
		TKN_NOT,
		TKN_INPUT,
		TKN_RULE,
		TKN_IMPORT,
		TKN_INGRESS,
		TKN_INCLUDE,
		TKN_MATCH,
		TKN_JUMP,
		TKN_CONDITION,
		TKN_FIELD,
	};

	ASSERT_EQ(sizeof(str) / sizeof(str[0]),
		  sizeof(types) / sizeof(types[0]));

	for (uint32_t i = 0; i < sizeof(str) / sizeof(str[0]); ++i) {
		enum jy_tkn type;
		const char *start = str[i], *end = NULL;
		// + 1 include '\0'
		size_t	    strsz = strlen(str[i]) + 1;
		end		  = jry_scan(start, strsz, &type);

		ASSERT_EQ(type, types[i]) << "i: " << i;
		ASSERT_EQ(memcmp(str[i], start, end - start), 0);
		ASSERT_EQ(strsz - 1, end - start) << "i: " << i;
	}
}

TEST(ScannerTest, ScanString)
{
	enum jy_tkn type;
	char	    str[] = "\"hello world\"";
	const char *start = str, *end = NULL;

	end = jry_scan(start, sizeof(str), &type);
	ASSERT_EQ(type, TKN_STRING);
	ASSERT_EQ(strlen(start), end - start);
	ASSERT_EQ(memcmp(str, start, end - start), 0);
}

TEST(ScannerTest, ScanRegexp)
{
	char	    str[] = "/Hello world\\//";
	enum jy_tkn type;
	const char *start = str, *end = NULL;
	end = jry_scan(start, sizeof(str), &type);

	ASSERT_EQ(type, TKN_REGEXP);
	ASSERT_EQ(strlen(str), end - start);
	ASSERT_EQ(memcmp(str, start, end - start), 0);
}

TEST(ScannerTest, ScanIdentifier)
{
	const char *names[] = {
		"hello",
		"_my_name"
		"identifier123",
	};

	for (uint32_t i = 0; i < sizeof(names) / sizeof(names[0]) - 1; ++i) {
		enum jy_tkn type;
		const char *start = names[i], *end = NULL;
		// + 1 to include '\0'
		uint32_t    len = strlen(start) + 1;
		end		= jry_scan(start, len, &type);
		ASSERT_EQ(type, TKN_IDENTIFIER);
		ASSERT_EQ(memcmp(start, start, end - start), 0);
		ASSERT_EQ(len - 1, end - start);
	}
}
