/*
 * TreeNode.h
 *
 *  Created on: 10.03.2013
 *      Author: user
 */

#ifndef TREENODE_H_
#define TREENODE_H_
#include <string>
#include <ostream>
#include <list>
#include <cstdarg>

namespace Compiler {
	namespace ASTBuilder {
		const signed SYMBOL_UNDEFINED = -1;

		typedef signed SymbolId;

		enum TreeNodeType {
			NODE_SYMBOL,

			NODE_DEFINITION_SEQUENCE,
			NODE_FUNCTION_DEFINITION,
			NODE_FUNCTION_ARGUMENTS,

			NODE_VARIABLE_DEFINITION,
			NODE_STATEMENT_SEQUENCE,
			NODE_STATEMENT_BLOCK,
			NODE_STATEMENT,

			NODE_ASSIGN,
			NODE_RETURN,
			NODE_PRINTLN,

			NODE_IDENTIFIER,

			NODE_OR,
			NODE_AND,
			NODE_EQUAL,
			NODE_NOT_EQUAL,
			NODE_LESS,
			NODE_LESS_EQUAL,
			NODE_GREATER,
			NODE_GREATER_EQUAL,

			NODE_PLUS,
			NODE_MINUS,
			NODE_MUL,
			NODE_DIV,

			NODE_CONST_INT,
			NODE_CONST_FLOAT,
			NODE_CALL,

			NODE_KEYWORD_IF,
			NODE_KEYWORD_FLOAT,
			NODE_KEYWORD_STRUCT,
			NODE_KEYWORD_INT,
			NODE_KEYWORD_PRINTLN,
			NODE_KEYWORD_VOID,
			NODE_KEYWORD_CHAR,

			NODE_UNDEFINED
		};

		struct TreeNode {
			TreeNodeType nodeType;
			SymbolId symbolId;
			std::list<TreeNode*> childs;

			TreeNode(TreeNodeType type);

			TreeNode* append(TreeNode* p_node);
			TreeNode* at(unsigned n);

			static void printTree(std::ostream& os, TreeNode* root, int tabs);
		};

		std::string nodeTypeToString(TreeNodeType type);
	}
}


#endif /* TREENODE_H_ */
