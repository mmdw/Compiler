/*
 * IRBuilder.h
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#ifndef IRBUILDER_H_
#define IRBUILDER_H_

#include <string>

#include "TreeNode.h"
#include "TreeBuilder.h"
#include "SymbolResolver.h"
#include "ConstTable.h"

namespace Compiler {

class IRBuilder {
	ASTBuilder::TreeNode* p_tree;
	SymbolResolver sr;
	ConstTable constTable;

	void fillTables();
	void walk(ASTBuilder::TreeNode* p_node);

public:
	IRBuilder(ASTBuilder::TreeNode* p_tree);
	virtual ~IRBuilder();
};

} /* namespace Syntax */
#endif /* IRBUILDER_H_ */
