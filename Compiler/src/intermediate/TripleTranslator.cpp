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

static const std::string EAX 		= "eax";
static const std::string EDX		= "edx";
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

static std::string printlnInt(const std::string& arg) {
	return "ccall\t[printf], __format_int, dword " + arg;
}

static std::string printlnDoubleFloat(const std::string& dword1, const std::string& dword2) {
	return "ccall\t[printf], __format_float, dword " + dword1 + ", dword " + dword2;
}

static std::string fild(const std::string& arg) {
	return "fild\tdword\t" + arg;
}

static std::string fstp(const std::string& arg) {
	return "fstp\tdword\t" + arg;
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

static std::string imul(const std::string& arg1, const std::string& arg2) {
	return "imul\t" + arg1 + ", " + arg2;
}

void TripleTranslator::translate(ASTBuilder::SymbolTable* p_table, std::ostream& os,
		 std::list<Triple>& tripleSequence) {

	for (std::list<Triple>::iterator it = tripleSequence.begin(); it != tripleSequence.end(); ++it) {
		Triple triple = *it;

		switch(triple.op) {
		case TRIPLE_ADD_INT:
			append(os, mov(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, add(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, mov(b(CodeGenerator::symbolToAddr(p_table, triple.result)), EAX));
			break;
		case TRIPLE_ADD_FLOAT:
			append(os, fld(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, fadd(b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, fstp(b(CodeGenerator::symbolToAddr(p_table, triple.result))));
			break;
		case TRIPLE_SUB_INT:
			append(os, mov(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, sub(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, mov(b(CodeGenerator::symbolToAddr(p_table, triple.result)), EAX));
			break;
		case TRIPLE_SUB_FLOAT:
			append(os, fld(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, fsub(b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, fstp(b(CodeGenerator::symbolToAddr(p_table, triple.result))));
			break;
		case TRIPLE_MUL_INT:
			append(os, mov(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, imul(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, mov(b(CodeGenerator::symbolToAddr(p_table, triple.result)), EAX));
			break;
		case TRIPLE_MUL_FLOAT:
			append(os, fld(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, fmul(b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, fstp(b(CodeGenerator::symbolToAddr(p_table, triple.result))));
			break;
		case TRIPLE_DIV_INT:
			append(os, mov(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, _xor(EDX, EDX));
			append(os, idiv(b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, mov(b(CodeGenerator::symbolToAddr(p_table, triple.result)), EAX));
			break;
		case TRIPLE_DIV_FLOAT:
			append(os, fld(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, fdiv(b(CodeGenerator::symbolToAddr(p_table, triple.arg2))));
			append(os, fstp(b(CodeGenerator::symbolToAddr(p_table, triple.result))));
			break;
		case TRIPLE_RETURN_PROCEDURE:
			append(os, RET);
			break;
		case TRIPLE_RETURN_FUNCTION:
			append(os, mov(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, RET);
			break;
		case TRIPLE_COPY:
			append(os, mov(EAX, b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, mov(b(CodeGenerator::symbolToAddr(p_table, triple.result)), EAX));
			break;
		case TRIPLE_PUSH:
			append(os, push(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			break;
		case TRIPLE_CALL_FUNCTION:
			append(os, call(CodeGenerator::symbolToAddr(p_table, triple.arg1)));
			append(os, mov(b(CodeGenerator::symbolToAddr(p_table, triple.result)), EAX));
			break;
		case TRIPLE_PRINTLN_INT:
			append(os, printlnInt(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			break;
		case TRIPLE_PRINTLN_DOUBLE_FLOAT:
			append(os, printlnDoubleFloat(
					b(CodeGenerator::symbolToAddr(p_table, triple.arg1)),
					b(CodeGenerator::symbolToAddr(p_table, triple.arg1) + " + 4")
				));
			break;
		case TRIPLE_INT_TO_FLOAT:
			append(os, fild(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, fstp(b(CodeGenerator::symbolToAddr(p_table, triple.result))));
			break;
		case TRIPLE_FLOAT_TO_DOUBLE_FLOAT:
			append(os, fld_dword(b(CodeGenerator::symbolToAddr(p_table, triple.arg1))));
			append(os, fstp_qword(b(CodeGenerator::symbolToAddr(p_table, triple.result))));
			break;
		default:
			throw std::string("translate failed: ") + tripleOpToString(triple.op);
		}
	}
}

TripleTranslator::~TripleTranslator() {
}

}
