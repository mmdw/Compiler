/*
 * Token.cpp
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#include "Token.h"
#include <cstddef>

namespace Tokenizer {
Token::Token() : name(UNDETERMINED), id(NULL) {
}

void Token::setId(const char* id) {
	Token::id = id;
}

void Token::setName(TokenName name) {
	Token::name = name;
}

TokenName Token::getName() const {
	return name;
}

const char* Token::getId() const {
	return id;
}

}


