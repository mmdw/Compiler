/*
 * Trine.cpp
 *
 *  Created on: 15.03.2013
 *      Author: user
 */

#include "Triple.h"

namespace Compiler {

std::string tripleOpToString(TripleOp op) {
			switch(op) {
			case TRIPLE_ADD: 				return "ADD";
			case TRIPLE_COPY: 				return "COPY";
			case TRIPLE_RETURN_FUNCTION:	return "RETURN_FUNCTION";
			case TRIPLE_RETURN_PROCEDURE:	return "RETURN_PROCEDURE";
			case TRIPLE_PUSH:				return "PUSH";
			case TRIPLE_CALL_FUNCTION:		return "CALL_FUNCTION";
			case TRIPLE_CALL_PROCEDURE: 	return "CALL_PROCEDURE";

			default:
				throw std::string("not implemented yet");
			}
		}

void printTripleSequence(std::ostream& os, TripleSequence& seq) {
	os << "triple sequence" << std::endl;

	for (TripleSequence::iterator it = seq.begin(); it != seq.end(); ++it) {
		os << tripleOpToString(it->op) << '\t' << it->result << '\t' <<  it->arg1 << '\t' << it->arg2 << std::endl;
	}
}

}
