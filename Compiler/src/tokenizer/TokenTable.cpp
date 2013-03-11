/*
 * TableHolder.cpp
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#include "TokenTable.h"

namespace Tokenizer {

const char* TokenTable::insert(const string& value) {
	return table.insert(value).first->c_str();
}

Tokenizer::TokenTable::~TokenTable() {
}

} /* namespace Tokenizer */
