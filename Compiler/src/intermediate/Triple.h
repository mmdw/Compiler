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

	enum TripleOp {
		TRIPLE_ADD_FLOAT,
		TRIPLE_ADD_INT,
		TRIPLE_SUB,
		TRIPLE_MUL,
		TRIPLE_DIV,
		TRIPLE_COPY,
		TRIPLE_PUSH,
		TRIPLE_RETURN_FUNCTION,
		TRIPLE_RETURN_PROCEDURE,
		TRIPLE_CALL_FUNCTION,
		TRIPLE_CALL_PROCEDURE,
		TRIPLE_INT_TO_FLOAT,
		TRIPLE_INT_TO_DOUBLE_FLOAT,
		TRIPLE_FLOAT_TO_DOUBLE_FLOAT,
		TRIPLE_PRINTLN_INT,
		TRIPLE_PRINTLN_DOUBLE_FLOAT
	};

	std::string tripleOpToString(TripleOp op);

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
