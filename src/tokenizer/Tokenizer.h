/*
 * Tokenizer.h
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <iostream>
#include <map>

#include "Token.h"
#include "TokenTable.h"

namespace Tokenizer {

using std::istream;
using std::cerr;
using std::map;
using std::string;

class Tokenizer {
	TokenTable symbolTable;

	map<string, TokenName> keywords;
	map<string, TokenName> oneCharPunctuators;
	map<string, TokenName> twoCharPunctuators;

	istream& is;

	char current;
	char lookahead;
	unsigned line;
	unsigned pos;

	void match(char ch);
	void error(const char* message);

	bool nondigit(char ch);
	void skipCommentsAndWs();

	void initTables();
	char processEscapeSequence();

public:
	Tokenizer(istream& is);
	Token getToken();

	bool hasNext();

	string tokenToString(const Token& token);

	virtual ~Tokenizer();
};

} /* namespace Tokenizer */
#endif /* TOKENIZER_H_ */
