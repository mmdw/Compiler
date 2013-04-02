/*
 * TripleTranslator.cpp
 *
 *  Created on: 16.03.2013
 *      Author: user
 */

#include <algorithm>
#include "TripleTranslator.h"
#include "CodeGenerator.h"

namespace Compiler {

static const std::string ST0		= "st0";
static const std::string EAX 		= "eax";
static const std::string EBX 		= "ebx";
static const std::string EDX		= "edx";
static const std::string RET 		= "ret";
static const std::string PUSH 		= "push";
static const std::string CALL		= "call";
static const std::string ESI		= "esi";
static const std::string EDI		= "edi";
static const std::string CLD		= "cld";
static const std::string ECX		= "ecx";
static const std::string REP_MOVSD  = "rep\tmovsd";

static const std::string CCALL_SCANF_INT   = "ccall [scanf], __format_int";
static const std::string CCALL_SCANF_FLOAT = "ccall [scanf], __format_float";
static const std::string CCALL_GETCHAR 	   = "ccall [getchar]";

static const std::string PRINT_NEWLINE = "ccall\t[printf], __newline";

TripleTranslator::TripleTranslator(const CodeGenerator& cg) : cg(cg), labelCount(0) {

}

LabelId TripleTranslator::newLabel() {
	return labelCount++;
}

static std::string inst(const std::string& name, const std::string& arg1, const std::string& arg2) {
	return name + "\t" + arg1 +",\t" + arg2;
}

static std::string mov(const std::string& arg1, const std::string& arg2) {
	return "mov\t" + arg1 + ", " + arg2;
}

static std::string add(const std::string& arg1, const std::string& arg2) {
	return "add\t" + arg1 + ", " + arg2;
}

static std::string sub(const std::string& arg1, const std::string& arg2) {
	return "sub\t" + arg1 + ", " + arg2;
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

static std::string printInt(const std::string& arg) {
	return "ccall\t[printf], __format_print_int, dword " + arg;
}

static std::string printDoubleFloat(const std::string& dword1, const std::string& dword2) {
	return "ccall\t[printf], __format_print_float, dword " + dword1 + ", dword " + dword2;
}

static std::string fild(const std::string& arg) {
	return "fild\tdword\t" + arg;
}

static std::string fstp(const std::string& arg) {
	return "fstp\tdword\t" + arg;
}

static std::string fistp(const std::string& arg) {
	return "fistp\tdword\t" + arg;
}

static std::string fld_dword(const std::string& arg) {
	return "fld\tdword\t" + arg;
}

static std::string fstp_qword(const std::string& arg) {
	return "fstp\tqword\t" + arg;
}

static std::string fld(const std::string& arg) {
	return "fld\tdword\t" + arg;
}

static std::string fadd(const std::string& arg) {
	return "fadd\tdword\t" + arg;
}

static std::string fsub(const std::string& arg) {
	return "fsub\tdword\t" + arg;
}

static std::string idiv(const std::string& arg1) {
	return "idiv\t" + arg1;
}

static std::string fdiv(const std::string& arg) {
	return "fdiv\tdword\t" + arg;
}

static std::string fmul(const std::string& arg) {
	return "fmul\tdword\t" + arg;
}

static std::string _xor(const std::string& arg1, const std::string& arg2) {
	return "xor\t " + arg1 + ", " + arg2;
}

static std::string _or(const std::string& arg1, const std::string& arg2) {
	return "or\t " + arg1 + ", " + arg2;
}

static std::string _and(const std::string& arg1, const std::string& arg2) {
	return "and\t " + arg1 + ", " + arg2;
}

static std::string imul(const std::string& arg1, const std::string& arg2) {
	return "imul\t" + arg1 + ", " + arg2;
}

static std::string lea(const std::string& arg1, const std::string& arg2) {
	return "lea\t" + arg1 + ", "+ arg2;
}

static std::string neg(const std::string& arg1) {
	return "neg\t" + arg1;
}

static std::string _not(const std::string& arg1) {
	return "not\t" + arg1;
}

static std::string cmp(const std::string& arg1, const std::string& arg2) {
	return "cmp\t " + arg1 + ", " + arg2;
}

static std::string fcomip(const std::string& arg1) {
	return "fcomip\t" + arg1;
}

static std::string jz(const std::string& arg1) {
	return "jz\t" + arg1;
}

static std::string jmp(const std::string& arg1) {
	return "jmp\t" + arg1;
}

static std::string ffree(const std::string& arg1) {
	return "ffree\t" + arg1;
}

void TripleTranslator::translate(ASTBuilder::SymbolTable* p_table, TypeTable* p_type, std::ostream& os,
		 TripleSequence& tripleSequence) {

	int i = 0;
	for (std::vector<Triple>::iterator it = tripleSequence.begin(); it != tripleSequence.end(); ++it, ++i) {
		Triple triple = *it;

		std::string tripleRes = cg.tripleArgToAddr(TripleArg::pos(i));

		if (triple.returnType != -1) {
			std::string size(triple.returnType == p_type->BASIC_DOUBLE_FLOAT ? ":QWORD" : ":DWORD");

			append(os,  "local\t" + tripleRes + size);
		}

		switch(triple.op) {
		case TRIPLE_OR:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, _or(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_AND:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, _and(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_ADD_INT:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, add(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_ADD_FLOAT:
			append(os, fld(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fadd(b(cg.tripleArgToAddr(triple.arg2))));
			append(os, fstp(b(tripleRes)));
			break;
		case TRIPLE_SUB_INT:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, sub(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_SUB_FLOAT:
			append(os, fld(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fsub(b(cg.tripleArgToAddr(triple.arg2))));
			append(os, fstp(b(tripleRes)));
			break;
		case TRIPLE_MUL_INT:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, imul(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_MUL_FLOAT:
			append(os, fld(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fmul(b(cg.tripleArgToAddr(triple.arg2))));
			append(os, fstp(b(tripleRes)));
			break;
		case TRIPLE_DIV_INT:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, _xor(EDX, EDX));
			append(os, idiv(b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_DIV_FLOAT:
			append(os, fld(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fdiv(b(cg.tripleArgToAddr(triple.arg2))));
			append(os, fstp(b(tripleRes)));
			break;
		case TRIPLE_RETURN_PROCEDURE:
			append(os, RET);
			break;
		case TRIPLE_RETURN_FUNCTION:
			append(os, lea(ESI, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, mov(EDI, b("__ret_ref")));
			append(os, mov(ECX, "1")); // FIXME
			append(os, CLD);
			append(os, REP_MOVSD);
			append(os, RET);
			break;
		case TRIPLE_COPY:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, mov(b(cg.tripleArgToAddr(triple.arg1)), EAX));
			append(os, mov(b(tripleRes), EAX));
			break;

		case TRIPLE_PUSH_BOOL:
		case TRIPLE_PUSH_FLOAT:
		case TRIPLE_PUSH_INT:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			break;
		case TRIPLE_CALL_FUNCTION:
			append(os, lea(EAX, b(tripleRes)));
			append(os, push(EAX));
			append(os, call(cg.tripleArgToAddr(triple.arg1)));
			break;
		case TRIPLE_CALL_PROCEDURE:
			append(os, call(cg.tripleArgToAddr(triple.arg1)));
			break;
		case TRIPLE_PRINT_INT:
		case TRIPLE_PRINTLN_INT:
			append(os, printInt(b(cg.tripleArgToAddr(triple.arg1))));
			if (triple.op == TRIPLE_PRINTLN_INT) {
				append(os, PRINT_NEWLINE);
			}
			break;
		case TRIPLE_PRINT_DOUBLE_FLOAT:
		case TRIPLE_PRINTLN_DOUBLE_FLOAT:
			append(os, printDoubleFloat(
					b(cg.tripleArgToAddr(triple.arg1)),
					b(cg.tripleArgToAddr(triple.arg1) + " + 4")
				));
			if (triple.op == TRIPLE_PRINTLN_DOUBLE_FLOAT) {
				append(os, PRINT_NEWLINE);
			}
			break;
		case TRIPLE_PRINT_BOOL:
		case TRIPLE_PRINTLN_BOOL:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, call("__print_bool"));
			if (triple.op == TRIPLE_PRINTLN_BOOL) {
				append(os, PRINT_NEWLINE);
			}
			break;
		case TRIPLE_READLN_FLOAT:
		case TRIPLE_READLN_INT: {
			append(os, lea(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, push(EAX));
			if (triple.op == TRIPLE_READLN_INT) {
				append(os, CCALL_SCANF_INT);
			} else if (triple.op == TRIPLE_READLN_FLOAT){
				append(os, CCALL_SCANF_FLOAT);
			}
			append(os, CCALL_GETCHAR);
		}
			break;
		case TRIPLE_INT_TO_FLOAT:
			append(os, fild(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fstp(b(tripleRes)));
			break;
		case TRIPLE_FLOAT_TO_INT:
			append(os, fld(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fistp(b(tripleRes)));
			break;
		case TRIPLE_FLOAT_TO_DOUBLE_FLOAT:
			append(os, fld_dword(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fstp_qword(b(tripleRes)));
			break;
		case TRIPLE_PUSH_PTR:
			append(os, lea(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, push(EAX));
			break;
		case TRIPLE_NEG_INT:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, neg(EAX));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_NEG_FLOAT:
			append(os, "fldz");
			append(os, fsub(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, fstp(b(tripleRes)));
			break;
		case TRIPLE_NOT:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, _not(EAX));
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_EQUAL_INT:
		case TRIPLE_NOT_EQUAL_INT:
		case TRIPLE_GREATER_INT:
		case TRIPLE_GREATER_EQUAL_INT:
		case TRIPLE_LESS_INT:
		case TRIPLE_LESS_EQUAL_INT:
			append(os, _xor(ECX, ECX));
			append(os, mov(EBX, "-1"));
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg1))));
			append(os, cmp(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, inst(cmpInstruction(triple.op) + '\t', ECX, EBX));
			append(os, mov(b(tripleRes), ECX));
			break;
		case TRIPLE_EQUAL_FLOAT:
		case TRIPLE_NOT_EQUAL_FLOAT:
		case TRIPLE_GREATER_FLOAT:
		case TRIPLE_LESS_FLOAT:
			append(os, _xor(ECX, ECX));
			append(os, mov(EBX, "-1"));

			append(os, fld(b(cg.tripleArgToAddr(triple.arg2))));
			append(os, fld(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, inst("fcomip\t", "st0", "st1"));
			append(os, inst(cmpInstruction(triple.op) + '\t', ECX, EBX));
			append(os, ffree(ST0));
			append(os, mov(b(tripleRes), ECX));
			break;
		case TRIPLE_LABEL:
			append(os, cg.tripleArgToAddr(triple.arg1) + ':');
			break;
		case TRIPLE_JZ:
			append(os, mov(EAX, b(cg.tripleArgToAddr(triple.arg2))));
			append(os, cmp(EAX, "0"));
			append(os, jz(cg.tripleArgToAddr(triple.arg1)));
			break;
		case TRIPLE_JMP:
			append(os, jmp(cg.tripleArgToAddr(triple.arg1)));
			break;
		case TRIPLE_PQUEUE_INIT:
			append(os, "ccall [pqueue_alloc]");
			append(os, mov(b(cg.tripleArgToAddr(triple.arg1)), EAX));
			break;
		case TRIPLE_PQUEUE_SIZE:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, "ccall [pqueue_size]");
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_PQUEUE_PUSH:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, "ccall [pqueue_push]");
			break;
		case TRIPLE_PQUEUE_POP:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, "ccall [pqueue_pop]");
			break;
		case TRIPLE_PQUEUE_TOP:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, "ccall [pqueue_top_value]");
			append(os, mov(b(tripleRes), EAX));
			break;
		case TRIPLE_PQUEUE_TOP_PRIORITY:
			append(os, push(b(cg.tripleArgToAddr(triple.arg1))));
			append(os, "ccall [pqueue_top_priority]");
			append(os, mov(b(tripleRes), EAX));
			break;
		default:
			throw std::string("translate failed: ") + tripleOpToString(triple.op);
		}
	}
}

std::string TripleTranslator::cmpInstruction(TripleOp op) {
	switch (op) {
	case TRIPLE_LESS_INT:
		return "cmovl";
	case TRIPLE_LESS_FLOAT:
		return "cmovc";

	case TRIPLE_LESS_EQUAL_INT:
		return "cmovle";

	case TRIPLE_GREATER_EQUAL_INT:
		return "cmovge";

	case TRIPLE_GREATER_INT:
		return "cmovg";
	case TRIPLE_GREATER_FLOAT:
		return "cmovnc";

	case TRIPLE_EQUAL_INT:
		return "cmove";
	case TRIPLE_EQUAL_FLOAT:
		return "cmovz";

	case TRIPLE_NOT_EQUAL_INT:
		return "cmovne";
	case TRIPLE_NOT_EQUAL_FLOAT:
		return "cmovnz";
	default:
		throw std::string("cmpInstruction");
	}
}

TripleTranslator::~TripleTranslator() {
}

}
