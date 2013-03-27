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

	SymbolId castSymbolToType(TypeId targetType, SymbolId symbol, TripleSequence& tripleSequence, bool force = false);

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
	TypeId maybeGetReferencedType(SymbolId id);

public:
	std::string symbolToAddr(ASTBuilder::SymbolId symbolId) const;

	CodeGenerator(ASTBuilder::TreeNode* p_root, ASTBuilder::SymbolTable* p_table, ASTBuilder::TypeTable* p_type);
	virtual ~CodeGenerator();
};

} /* namespace Compiler */
#endif /* CODEGENERATOR_H_ */
