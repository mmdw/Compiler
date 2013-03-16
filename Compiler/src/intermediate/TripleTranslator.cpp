/*
 * TripleTranslator.cpp
 *
 *  Created on: 16.03.2013
 *      Author: user
 */

#include <algorithm>
#include "TripleTranslator.h"


namespace Compiler {

static const std::string EAX 		= "eax";
static const std::string RET 		= "ret";
static const std::string PUSH 		= "push";
static const std::string CALL		= "call";

TripleTranslator::TripleTranslator() {
}

static std::string mov(const std::string& arg1, const std::string& arg2) {
	return "mov\t" + arg1 + ", " + arg2;
}

static std::string add(const std::string& arg1, const std::string& arg2) {
	return "add\t" + arg1 + ", " + arg2;
}

static void append(std::ostream& os, const std::string& line) {
	os << '\t' << line << std::endl;
}

static std::string b(const std::string& val) {
	return "[" + val + "]";
}

static std::string push(const std::string& arg) {
	return PUSH + "\tdword " + arg;
}

static std::string call(const std::string& arg) {
	return CALL + '\t' + arg;
}

static std::string println(const std::string& arg) {
	return "ccall\t[printf], __format_int, dword " + arg;
}

void TripleTranslator::translate(std::ostream& os,
		std::list<Triple>& tripleSequence) {

	for (std::list<Triple>::iterator it = tripleSequence.begin(); it != tripleSequence.end(); ++it) {
		Triple triple = *it;

		switch(triple.op) {
		case TRIPLE_ADD:
			append(os, mov(EAX, b(triple.arg1)));
			append(os, add(EAX, b(triple.arg2)));
			append(os, mov(b(triple.result), EAX));

			break;
		case TRIPLE_RETURN_PROCEDURE:
			append(os, RET);
			break;
		case TRIPLE_RETURN_FUNCTION:
			append(os, mov(EAX, b(triple.arg1)));
			append(os, RET);
			break;
		case TRIPLE_COPY:
			append(os, mov(EAX, b(triple.arg1)));
			append(os, mov(b(triple.result), EAX));
			break;
		case TRIPLE_PUSH:
			append(os, push(b(triple.arg1)));
			break;
		case TRIPLE_CALL_FUNCTION:
			append(os, call(triple.arg1));
			append(os, mov(b(triple.result), EAX));
			break;
		case TRIPLE_PRINTLN:
			append(os, println(b(triple.arg1)));
			break;
		default:
			throw std::string("translate failed: ") + tripleOpToString(triple.op);
		}
	}
}

TripleTranslator::~TripleTranslator() {
}

}
