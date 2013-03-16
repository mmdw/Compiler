/*
 * IRBuilder.h
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#ifndef IRBUILDER_H_
#define IRBUILDER_H_

#include <sstream>
#include <string>

#include "TreeNode.h"
#include "TreeBuilder.h"
#include "SymbolResolver.h"
#include "ConstTable.h"
#include "FuncTable.h"
#include "Triple.h"
#include "TripleTranslator.h"

namespace Compiler {

class IRBuilder {
	ASTBuilder::TreeNode* p_tree;

	SymbolResolver resolver;

	ConstTable constTable;
	FuncTable funcTable;
	TripleTranslator translator;

	std::ostringstream output;

	void fillTables				();
	void walk					(SymbolType returnType, ASTBuilder::TreeNode* p_node, TripleSequence& trineSequence);

	void walkGlobal				(ASTBuilder::TreeNode* p_node);

	SymbolId processVariable	(ASTBuilder::TreeNode* p_variableNode);
	SymbolId processFunction	(ASTBuilder::TreeNode* p_functionNode);
	Addr processExpression		(ASTBuilder::TreeNode* p_expression, 	TripleSequence& trineSequence);
	void processReturn			(ASTBuilder::TreeNode* p_return, 		TripleSequence& threeTupleSequence);
	Addr processCall			(ASTBuilder::TreeNode* p_expression, 	TripleSequence& tripleSequence);
	void processPrintln			(ASTBuilder::TreeNode* p_node, 			TripleSequence& tripleSequence);

	Addr constIdToAddr			(ConstId constId);
	Addr symbolIdToAddr			(SymbolId symbolId);

	SymbolType stringToType		(const std::string& val);

	bool hasMainFunc();
public:
	IRBuilder(ASTBuilder::TreeNode* p_tree);
	virtual ~IRBuilder();
};

} /* namespace Syntax */
#endif /* IRBUILDER_H_ */
