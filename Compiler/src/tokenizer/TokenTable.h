/*
 * TableHolder.h
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#ifndef TABLEHOLDER_H_
#define TABLEHOLDER_H_

#include <map>
#include <string>
#include <set>
#include "Token.h"

namespace Tokenizer {

using std::map;
using std::string;
using std::set;

class TokenTable {
	set<string> table;

public:
	const char* insert(const string& value);
	~TokenTable();
};

} /* namespace Tokenizer */
#endif /* TABLEHOLDER_H_ */
