/*
 * util.h
 *
 *  Created on: 07.03.2013
 *      Author: user
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <sstream>

namespace Util {
using std::string;
using std::ostringstream;

template<typename T>
std::string to_string(const T& value);

template<typename T>
inline std::string to_string(const T& value) {
	ostringstream oss;
	oss << value;
	return oss.str();
}
}
#endif /* UTIL_H_ */
