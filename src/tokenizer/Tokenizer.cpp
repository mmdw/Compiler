/*
 * Tokenizer.cpp
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#include "Tokenizer.h"
#include <cstdlib>
#include <string>
#include <sstream>

#include "Token.h"
#include "../util/util.h"

namespace Tokenizer {

using std::string;
using std::ostringstream;
using std::istringstream;
using Util::to_string;

Tokenizer::Tokenizer(istream& is) : is(is), line(1), pos(0) {
	current = is.get();
	lookahead = is.get();
	is.unget();

	initTables();
}

void Tokenizer::match(char ch) {
	if (current != ch) {
		error("Tokenizer: match failed");
	}

	if (current == '\n') {
		++line;
		pos = -1;
	}

	current = is.get();
	lookahead = is.get();
	is.unget();

	++pos;
}

void Tokenizer::error(const char* message) {
	cerr << "Error: line " << line << " pos: " << pos << " " << message;
	exit(-1);
}

void Tokenizer::skipCommentsAndWs() {
	while (isspace(current) || (current == '/' && (lookahead == '/' || lookahead == '*'))) {
		if (isspace(current)) {
			match(current);
		} else if (lookahead == '/') {
			while (current != '\n') {
				match(current);
			}

			match(current);
		} else {
			while (current != '*' || lookahead != '/') {
				match(current);
			}

			match(current);
			match(current);
		}
	}
}

char Tokenizer::processEscapeSequence() {
	char ch;

	match('\\');
	switch (current) {
	case '\'':
	case '\"':
	case '\\':
		ch = current;
		break;
	case 'n':
		ch = '\n';
		break;
	case 't':
		ch = '\t';
		break;
	default:
		error("expected: valid escape sequence");
	}

	match(current);
	return ch;
}

Token Tokenizer::getToken() {
	Token result;

	string word;
	string maybe2op = string() + current + lookahead;
	string maybe1op = string() + current;

	if (nondigit(current)) {
		while (nondigit(current) || isdigit(current)) {
			word += current;
			match(current);
		}

		if (keywords.find(word) != keywords.end()) {
			result.setName(keywords[word]);
		} else {
			result.setName(IDENTIFIER);
			result.setId(symbolTable.insert(word));
		}
	} else if (isdigit(current)) {
		while (isdigit(current)) {
			word += current;
			match(current);
		}

		if (current == '.') {
			word += current;
			match(current);

			while (isdigit(current)) {
				word += current;
				match(current);
			}


			result.setName(CONST_FLOAT);
			result.setId(symbolTable.insert(word));
		} else {
			result.setName(CONST_INT);
			result.setId(symbolTable.insert(word));
		}
	} else if (current == '\'') {
		match(current);

		if (current == '\\') {
			word += processEscapeSequence();
		} else {
			word += current;
			match(current);
		}

		match('\'');

		result.setName(CONST_CHAR);
		result.setId(symbolTable.insert(word));
	} else if (current == '\"') {
		match('\"');

		while (current != '\"') {
			if (current == '\n') {
				error("unexpected newline character");
			}

			if (current == '\\') {
				word += processEscapeSequence();
			} else {
				word += current;
				match(current);
			}
		}

		match(current);

		result.setName(CONST_STRING);
		result.setId(symbolTable.insert(word));
	} else if (twoCharPunctuators.find(maybe2op) != twoCharPunctuators.end()) {
		match(current);
		match(current);
		result.setName(twoCharPunctuators[maybe2op]);
	} else if (oneCharPunctuators.find(maybe1op) != oneCharPunctuators.end()) {
		match(current);
		result.setName(oneCharPunctuators[maybe1op]);
	}

	if (result.getName() == UNDETERMINED) {
		error("tokenizer failed");
	}

	skipCommentsAndWs();
	return result;
}

bool Tokenizer::hasNext() {
	return current != -1;
}

bool Tokenizer::nondigit(char ch) {
	return isalpha(ch) || ch == '_';
}

void Tokenizer::initTables() {
	keywords["if"] 		= KEYWORD_IF;
	keywords["else"]	= KEYWORD_ELSE;
	keywords["then"]	= KEYWORD_THEN;
	keywords["int"]		= KEYWORD_INT;
	keywords["char"]	= KEYWORD_CHAR;
	keywords["struct"]	= KEYWORD_STRUCT;
	keywords["float"]	= KEYWORD_FLOAT;
	keywords["void"]	= KEYWORD_VOID;
	keywords["while"]	= KEYWORD_WHILE;
	keywords["for"]		= KEYWORD_FOR;
	keywords["complex"]	= KEYWORD_COMPLEX;
	keywords["new"]		= KEYWORD_NEW;
	keywords["delete"]	= KEYWORD_DELETE;

	oneCharPunctuators["+"]	= OP_PLUS;
	oneCharPunctuators["-"]	= OP_MINUS;
	oneCharPunctuators["*"] = OP_MUL;
	oneCharPunctuators["/"] = OP_DIV;
	oneCharPunctuators["!"]	= OP_NOT;
	oneCharPunctuators["&"] = OP_BITWISE_AND;
	oneCharPunctuators["|"] = OP_BITWISE_OR;
	oneCharPunctuators["="] = OP_ASSIGN;
	oneCharPunctuators["."] = OP_DOT;
	oneCharPunctuators[";"] = OP_SEMICOLON;
	oneCharPunctuators[","] = OP_COMMA;

	oneCharPunctuators["("] = LEFT_ROWND_BRACKET;
	oneCharPunctuators[")"] = RIGHT_ROWND_BRACKET;
	oneCharPunctuators["["] = LEFT_SQUARE_BRACKET;
	oneCharPunctuators["]"] = RIGHT_SQUARE_BRACKET;
	oneCharPunctuators["{"] = BEGIN_BRACKET;
	oneCharPunctuators["}"] = END_BRACKET;

	oneCharPunctuators["<"] = CMP_LESS;
	oneCharPunctuators[">"] = CMP_GREATER;

	twoCharPunctuators["&&"] = OP_AND;
	twoCharPunctuators["||"] = OP_OR;
	twoCharPunctuators["+="] = OP_ASSIGN_PLUS;
	twoCharPunctuators["-="] = OP_ASSIGN_MINUS;
	twoCharPunctuators["*="] = OP_ASSIGN_MUL;
	twoCharPunctuators["++"] = OP_INC;
	twoCharPunctuators["--"] = OP_DEC;
	twoCharPunctuators["->"] = OP_NARROW;
	twoCharPunctuators["<="] = CMP_LESS_EQUAL;
	twoCharPunctuators[">="] = CMP_GREATER_EQUAL;
	twoCharPunctuators["=="] = CMP_EQUAL;
	twoCharPunctuators["!="] = CMP_NOT_EQUAL;
}

string Tokenizer::tokenToString(const Token& token) {
	for (map<string, TokenName>::const_iterator it = keywords.begin(); it != keywords.end(); ++it) {
		if (it->second == token.getName()) {
			return string("<KEYWORD: ") + it->first + string(">");
		}
	}

	for (map<string, TokenName>::const_iterator it = oneCharPunctuators.begin(); it != oneCharPunctuators.end();
			++it) {
		if (it->second == token.getName()) {
			return string("<") + it->first + string(">");
		}
	}

	for (map<string, TokenName>::const_iterator it = twoCharPunctuators.begin(); it != twoCharPunctuators.end();
			++it) {
		if (it->second == token.getName()) {
			return string("<") + it->first + string(">");
		}
	}

	string type;
	switch (token.getName()) {
	case CONST_CHAR:
		return string("<CHAR: ") + token.getId() + string(">");
	case CONST_STRING:
		return string("<STRING: ") + token.getId() + string(">");
	case CONST_FLOAT:
		return string("<FLOAT: ") + token.getId() + string(">");
	case CONST_INT:
		return string("<INT: ") + token.getId() + string(">");
	case IDENTIFIER:
		return string("<IDENTIFIER: ") + token.getId() + string(">");
	default:
		return string("tokenToString failed");
	}
}

Tokenizer::~Tokenizer() {
	// TODO Auto-generated destructor stub
}

} /* namespace Tokeniser */
