/*
 * Trine.h
 *
 *  Created on: 15.03.2013
 *      Author: user
 */

#ifndef TRIPLE_H_
#define TRIPLE_H_

#include <ostream>
#include <string>
#include <list>

#include "TreeNode.h"

namespace Compiler {
	using namespace ASTBuilder;
	typedef int LabelId;
	const LabelId LABEL_UNDEFINED = -1;

	enum TripleOp {
		TRIPLE_ADD_FLOAT,
		TRIPLE_ADD_INT,
		TRIPLE_SUB_FLOAT,
		TRIPLE_SUB_INT,
		TRIPLE_MUL_FLOAT,
		TRIPLE_MUL_INT,
		TRIPLE_DIV_FLOAT,
		TRIPLE_DIV_INT,
		TRIPLE_COPY,
		TRIPLE_PUSH_INT,
		TRIPLE_PUSH_FLOAT,
		TRIPLE_PUSH_PTR,
		TRIPLE_PUSH_BOOL,
		TRIPLE_RETURN_FUNCTION,
		TRIPLE_RETURN_PROCEDURE,
		TRIPLE_CALL_FUNCTION,
		TRIPLE_CALL_PROCEDURE,
		TRIPLE_INT_TO_FLOAT,
		TRIPLE_INT_TO_DOUBLE_FLOAT,
		TRIPLE_FLOAT_TO_DOUBLE_FLOAT,

		TRIPLE_PRINTLN_INT,
		TRIPLE_PRINTLN_BOOL,
		TRIPLE_PRINTLN_DOUBLE_FLOAT,
		TRIPLE_PRINT_INT,
		TRIPLE_PRINT_BOOL,
		TRIPLE_PRINT_DOUBLE_FLOAT,
		TRIPLE_READLN_INT,
		TRIPLE_READLN_FLOAT,
		TRIPLE_READLN_BOOL,

		TRIPLE_OR,
		TRIPLE_AND,
		TRIPLE_NEG_INT,
		TRIPLE_NEG_FLOAT,
		TRIPLE_NOT,

		TRIPLE_LESS_INT,
		TRIPLE_LESS_EQUAL_INT,
		TRIPLE_GREATER_INT,
		TRIPLE_GREATER_EQUAL_INT,
		TRIPLE_EQUAL_INT,
		TRIPLE_NOT_EQUAL_INT,

		TRIPLE_LESS_FLOAT,
		TRIPLE_LESS_EQUAL_FLOAT,
		TRIPLE_GREATER_FLOAT,
		TRIPLE_GREATER_EQUAL_FLOAT,
		TRIPLE_EQUAL_FLOAT,
		TRIPLE_NOT_EQUAL_FLOAT,

		TRIPLE_LABEL,
		TRIPLE_JZ,
		TRIPLE_JMP,

		TRIPLE_PQUEUE_INIT,
		TRIPLE_PQUEUE_PUSH,
		TRIPLE_PQUEUE_POP,
		TRIPLE_PQUEUE_SIZE,
		TRIPLE_PQUEUE_TOP,
		TRIPLE_PQUEUE_TOP_PRIORITY,
	};

	std::string tripleOpToString(TripleOp op);

	struct TripleArg {
		bool isAddr;
		union {
			SymbolId sym;
			int 	 pos;
		};
	};

	struct Triple {
		TripleOp op;

		SymbolId result;
		SymbolId arg1;
		SymbolId arg2;

		Triple(TripleOp op)
		: op(op), result(SYMBOL_UNDEFINED), arg1(SYMBOL_UNDEFINED), arg2(SYMBOL_UNDEFINED) { }

		Triple(TripleOp op, const SymbolId& arg1)
			: op(op), result(SYMBOL_UNDEFINED), arg1(arg1), arg2(SYMBOL_UNDEFINED) { }

		Triple(TripleOp op, const SymbolId& result, const SymbolId& arg1)
			: op(op), result(result), arg1(arg1), arg2(SYMBOL_UNDEFINED) { }

		Triple(TripleOp op, const SymbolId& result, const SymbolId& arg1, const SymbolId& arg2)
			: op(op), result(result), arg1(arg1), arg2(arg2) { }
	};

	typedef std::list<Triple> TripleSequence;

	void printTripleSequence(std::ostream& os, TripleSequence& seq);
}

#endif /* TRIPLE_H_ */
