/*
 * StringHolder.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef STRINGHOLDER_H_
#define STRINGHOLDER_H_

#include <string>
#include <list>

namespace Compiler {
namespace ASTBuilder {

class StringHolder {
	StringHolder();
	virtual ~StringHolder();
	static std::list<std::string*> strings;

public:
	static std::string* store(std::string* p_str) {
		strings.push_back(p_str);

		return p_str;
	}

	static void clear();
};

} /* namespace ASTBuilder */
} /* namespace Compiler */
#endif /* STRINGHOLDER_H_ */
