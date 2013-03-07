/*
 * Token.h
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#ifndef TOKEN_H_
#define TOKEN_H_

namespace Tokenizer {

enum TokenName {
	UNDETERMINED = -1,

	CONST_CHAR,			// 'a' 'b'
	CONST_STRING,		// "Watermelon"
	CONST_INT,			// 513
	CONST_FLOAT,		// 0.99
	IDENTIFIER,			// paramOne

	KEYWORD_IF,
	KEYWORD_THEN,
	KEYWORD_ELSE,

	KEYWORD_INT,
	KEYWORD_CHAR,
	KEYWORD_STRUCT,
	KEYWORD_FLOAT,
	KEYWORD_VOID,
	KEYWORD_WHILE,
	KEYWORD_FOR,
	KEYWORD_COMPLEX,
	KEYWORD_NEW,
	KEYWORD_DELETE,

	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV,

	OP_AND,
	OP_OR,
	OP_NOT,

	OP_BITWISE_AND,
	OP_BITWISE_OR,

	OP_ASSIGN,
	OP_ASSIGN_PLUS,
	OP_ASSIGN_MINUS,
	OP_ASSIGN_DIV,
	OP_ASSIGN_MUL,

	OP_INC,
	OP_DEC,

	OP_NARROW,
	OP_DOT,
	OP_SEMICOLON,
	OP_COMMA,

	LEFT_ROWND_BRACKET,
	RIGHT_ROWND_BRACKET,
	LEFT_SQUARE_BRACKET,
	RIGHT_SQUARE_BRACKET,
	BEGIN_BRACKET,
	END_BRACKET,

	CMP_LESS,
	CMP_GREATER,
	CMP_LESS_EQUAL,
	CMP_GREATER_EQUAL,
	CMP_EQUAL,
	CMP_NOT_EQUAL,
};

class Token {
	TokenName name;
	const char* id;

public:
	Token();

	void setId(const char* id);
	void setName(TokenName name);

	TokenName getName() const;
	const char * getId() const;
};

}
#endif /* TOKEN_H_ */
