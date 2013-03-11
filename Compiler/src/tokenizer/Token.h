/*
 * Token.h
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#include "TokenName.h"

namespace Tokenizer {

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
