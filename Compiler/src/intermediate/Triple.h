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

namespace Compiler {
	enum TripleOp {
		TRIPLE_ADD,
		TRIPLE_SUB,
		TRIPLE_MUL,
		TRIPLE_DIV,
		TRIPLE_COPY,
		TRIPLE_PUSH,
		TRIPLE_RETURN_FUNCTION,
		TRIPLE_RETURN_PROCEDURE,
		TRIPLE_CALL_FUNCTION,
		TRIPLE_CALL_PROCEDURE,
		TRIPLE_PRINTLN
	};

	std::string tripleOpToString(TripleOp op);

	typedef std::string Addr;

	struct Triple {
		TripleOp op;

		Addr result;
		Addr arg1;
		Addr arg2;

		Triple(TripleOp op)
		: op(op), result(""), arg1(""), arg2("") { }

		Triple(TripleOp op, const Addr& arg1)
			: op(op), result(""), arg1(arg1), arg2("") { }

		Triple(TripleOp op, const Addr& result, const Addr& arg1)
			: op(op), result(result), arg1(arg1), arg2("") { }

		Triple(TripleOp op, const Addr& result, const Addr& arg1, const Addr& arg2)
			: op(op), result(result), arg1(arg1), arg2(arg2) { }
	};

	typedef std::list<Triple> TripleSequence;

	void printTripleSequence(std::ostream& os, TripleSequence& seq);
}

#endif /* TRIPLE_H_ */
