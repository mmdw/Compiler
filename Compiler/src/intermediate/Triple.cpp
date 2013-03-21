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
			case TRIPLE_PUSH_INT:				return "PUSH_INT";
			case TRIPLE_PUSH_FLOAT:				return "PUSH_FLOAT";
			case TRIPLE_PUSH_BOOL:				return "PUSH_BOOL";
			case TRIPLE_PUSH_PTR:				return "PUSH_PTR";
			case TRIPLE_CALL_FUNCTION:			return "CALL_FUNCTION";
			case TRIPLE_CALL_PROCEDURE: 		return "CALL_PROCEDURE";
			case TRIPLE_INT_TO_FLOAT:			return "INT_TO_FLOAT";
			case TRIPLE_PRINTLN_DOUBLE_FLOAT:	return "PRINTLN_DOUBLE_FLOAT";
			case TRIPLE_PRINTLN_INT:			return "PRINTLN_INT";
			case TRIPLE_PRINTLN_BOOL:			return "PRINTLN_BOOL";
			case TRIPLE_OR:						return "OR";
			case TRIPLE_AND:					return "AND";
			case TRIPLE_NEG_INT:				return "NEG_INT";
			case TRIPLE_NEG_FLOAT:				return "NEG_FLOAT";
			case TRIPLE_NOT:					return "NOT";
			case TRIPLE_LESS_INT:				return "LESS_INT";
			case TRIPLE_LESS_EQUAL_INT:			return "LESS_EQUAL_INT";
			case TRIPLE_GREATER_INT:			return "GREATER_INT";
			case TRIPLE_GREATER_EQUAL_INT:		return "GREATER_EQUAL_INT";
			case TRIPLE_LESS_FLOAT:				return "LESS_FLOAT";
			case TRIPLE_LESS_EQUAL_FLOAT:		return "LESS_EQUAL_FLOAT";
			case TRIPLE_GREATER_FLOAT:			return "GREATER_FLOAT";
			case TRIPLE_GREATER_EQUAL_FLOAT:	return "GREATER_EQUAL_FLOAT";
			case TRIPLE_JZ:						return "JZ";
			case TRIPLE_JMP:					return "JMP";

			default:
				throw std::string("tripleOpToString: not implemented yet");
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
