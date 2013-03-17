/*
 * StringHolder.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <algorithm>
#include "StringHolder.h"

namespace Compiler {
namespace ASTBuilder {

std::list<std::string*> StringHolder::strings;

StringHolder::StringHolder() {
	// TODO Auto-generated constructor stub

}

StringHolder::~StringHolder() {
	clear();
}

void StringHolder::clear() {
	for (std::list<std::string*>::iterator it = strings.begin(); it != strings.end(); ++it) {
		delete *it;
	}
}

} /* namespace ASTBuilder */
} /* namespace Compiler */
