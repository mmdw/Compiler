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

	std::ostream& 				output;

	TripleTranslator			tripleTranslator;

	SymbolId castSymbolToPrimaryType(SymbolType targetType, SymbolId symbol, TripleSequence& tripleSequence);
	SymbolId castSymbolToSymbol(SymbolId targetId, SymbolId symbol, TripleSequence& tripleSequence);

	void generateHeader();
	void generateProcedures();
	void generateConstSection();
	void generateFooter();
	void generateGlobalVariableSection();
	void generateInitializationGlobalsCode();

	void validate();

	void generateTripleSequence(SymbolId targetFuncId, ASTBuilder::TreeNode* p_node, TripleSequence& tripleSequence);
	void generateLocalVariables(TripleSequence& tripleSequence);

	SymbolId maybeEval(SymbolId targetSymbolId, TreeNode* p_node, TripleSequence& tripleSequence);
	SymbolType maybeUpcastFormCustomType(SymbolId id);

public:
	static std::string symbolToAddr(ASTBuilder::SymbolTable* p_table, ASTBuilder::SymbolId symbolId);

	CodeGenerator(ASTBuilder::TreeNode* p_root, ASTBuilder::SymbolTable* p_table);
	virtual ~CodeGenerator();
};

} /* namespace Compiler */
#endif /* CODEGENERATOR_H_ */
