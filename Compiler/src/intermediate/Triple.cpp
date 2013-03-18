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
			case TRIPLE_ADD_FLOAT: 				return "ADD_FLOAT";
			case TRIPLE_ADD_INT:				return "ADD_INT";
			case TRIPLE_COPY: 					return "COPY";
			case TRIPLE_SUB_FLOAT:				return "SUB_FLOAT";
			case TRIPLE_SUB_INT:				return "SUB_INT";
			case TRIPLE_MUL_FLOAT:				return "MUL_FLOAT";
			case TRIPLE_MUL_INT:				return "MUL_INT";
			case TRIPLE_DIV_FLOAT:				return "DIV_FLOAT";
			case TRIPLE_DIV_INT:				return "DIV_INT";
			case TRIPLE_RETURN_FUNCTION:		return "RETURN_FUNCTION";
			case TRIPLE_RETURN_PROCEDURE:		return "RETURN_PROCEDURE";
			case TRIPLE_PUSH:					return "PUSH";
			case TRIPLE_CALL_FUNCTION:			return "CALL_FUNCTION";
			case TRIPLE_CALL_PROCEDURE: 		return "CALL_PROCEDURE";
			case TRIPLE_INT_TO_FLOAT:			return "INT_TO_FLOAT";
			case TRIPLE_PRINTLN_DOUBLE_FLOAT:	return "PRINTLN_DOUBLE_FLOAT";
			case TRIPLE_PRINTLN_INT:			return "PRINTLN_INT";

			default:
				throw std::string("not implemented yet");
			}
		}

void printTripleSequence(std::ostream& os, TripleSequence& seq) {
	os << "triple sequence" << std::endl;

	for (TripleSequence::iterator it = seq.begin(); it != seq.end(); ++it) {
		os << tripleOpToString(it->op);
		if (it->result != SYMBOL_UNDEFINED) {
			os << '\t' << it->result;
		}

		if (it->arg1 != SYMBOL_UNDEFINED) {
		 os << '\t' <<  it->arg1;
		}

		if (it->arg2 != SYMBOL_UNDEFINED) {
			os << '\t' << it->arg2;
		}

		os << std::endl;
	}
}

}
