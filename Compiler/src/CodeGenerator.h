/*
 * CodeGenerator.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef CODEGENERATOR_H_
#define CODEGENERATOR_H_

#include <ostream>

#include "TripleTranslator.h"
#include "TreeNode.h"
#include "SymbolTable.h"
#include "Triple.h"

namespace Compiler {

class CodeGenerator {
	ASTBuilder::TreeNode* 		p_root;
	ASTBuilder::SymbolTable*	p_table;
	ASTBuilder::TypeTable* 		p_type;

	std::ostream& 				output;

	TripleTranslator			tripleTranslator;

	TripleArg castSymbolToType(TypeId targetType, TripleArg tripleArg, TripleSequence& tripleSequence, bool force = false);
	TypeId getTripleType(const TripleArg& ta, TripleSequence& tripleSequence);

	void generateHeader();
	void generateProcedures();
	void generateConstSection();
	void generateFooter();
	void generateGlobalVariableSection();
	void generateInitializationGlobalsCode();

	void validate();

	void generateTripleSequence(SymbolId targetFuncId, ASTBuilder::TreeNode* p_node, TripleSequence& tripleSequence);
	void generateLocalVariables(TripleSequence& tripleSequence);

	TripleArg maybeEval(SymbolId targetSymbolId, TreeNode* p_node, TripleSequence& tripleSequence);
	TypeId maybeGetReferencedType(TripleArg ta, TripleSequence& ts);

public:
	std::string symbolToAddr(SymbolId symbolId) const;
	std::string tripleArgToAddr(TripleArg ta) const;

	CodeGenerator(ASTBuilder::TreeNode* p_root, ASTBuilder::SymbolTable* p_table, ASTBuilder::TypeTable* p_type, std::ostream& os);
	virtual ~CodeGenerator();
};

} /* namespace Compiler */
#endif /* CODEGENERATOR_H_ */
