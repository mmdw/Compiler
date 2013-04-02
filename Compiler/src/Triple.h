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
#include <vector>

#include <cassert>

#include "definitions.h"
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
		TRIPLE_FLOAT_TO_INT,
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

	class TripleArg {
	public:
		int getPos() const {
			assert(!_isNull);
			assert(!_isSymbol);
			return position;
		}

		SymbolId getSymbolId() const {
			assert(!_isNull);
			assert(_isSymbol);
			return symbol;
		}

		void incPos() {
			assert(!_isNull);
			assert(!_isSymbol);

			++position;
		}

		static TripleArg pos(int position) {
			TripleArg result;
			result._isNull = false;
			result._isSymbol = false;
			result.position = position;

			return result;
		}

		static TripleArg sym(SymbolId symbolId) {
			TripleArg result;
			result._isNull = false;
			result._isSymbol = true;
			result.symbol = symbolId;

			return result;
		}

		static TripleArg null() {
			TripleArg result;
			result._isNull = true;

			return result;
		}

		bool isSymbol() const {
			return _isSymbol;
		}

		bool isNull() const {
			return _isNull;
		}

		TripleArg() : _isNull(true), _isSymbol(false) {}

	private:
		bool _isNull;
		bool _isSymbol;
		union {
			SymbolId symbol;
			int 	 position;
		};

	};

	struct Triple {
		TripleOp op;

		TripleArg arg1;
		TripleArg arg2;

		TypeId returnType;

		Triple(TripleOp op, TypeId returnType)
		: op(op), arg1(TripleArg::null()), arg2(TripleArg::null()), returnType(returnType) { }

		Triple(TripleOp op, const TripleArg& arg1, TypeId returnType)
			: op(op), arg1(arg1), arg2(TripleArg::null()), returnType(returnType) { }

		Triple(TripleOp op, const TripleArg& arg1, const TripleArg& arg2, TypeId returnType)
			: op(op), arg1(arg1), arg2(arg2), returnType(returnType) { }
	};


	class NewTripleSequence {
		std::vector<Triple> seq;

	public:
		typedef std::vector<Triple>::iterator iterator;

		std::vector<Triple>::iterator begin() {
			return seq.begin();
		}

		std::vector<Triple>::iterator end() {
			return seq.end();
		}

		void append(Triple triple) {
			seq.push_back(triple);
		}

		void append_front(Triple triple) {
			seq.insert(seq.begin(), triple);

			std::vector<Triple>::iterator it = seq.begin();
			++it;
			for (/* */; it != seq.end(); ++it) {
				if (!it->arg1.isNull() && !it->arg1.isSymbol()) {
					it->arg1.incPos();
				}

				if (!it->arg2.isNull() && !it->arg2.isSymbol()) {
					it->arg2.incPos();
				}
			}
		}

		int lastIndex() {
//			assert(!seq.empty());
			return seq.size() - 1;
		}

		const Triple last() {
			return seq.at(lastIndex());
		}

		bool isEmpty() {
			return seq.empty();
		}

		TripleArg argBack() {
			return TripleArg::pos(lastIndex());
		}

		const Triple& at(int pos) {
			return seq.at(pos);
		}
	};

	typedef NewTripleSequence TripleSequence;
	void printTripleSequence(std::ostream& os, TripleSequence& seq);
}

#endif /* TRIPLE_H_ */
