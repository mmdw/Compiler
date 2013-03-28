#include <string>
#include <cassert>
#include "../headers/TreeNode.h"

namespace Compiler {
	namespace ASTBuilder {

		std::string nodeTypeToString(TreeNodeType type) {
			switch(type) {
			case NODE_SYMBOL: 				return "NODE_SYMBOL";
			case NODE_DEFINITION_SEQUENCE:	return "NODE_DEFINITION_SEQUENCE";
			case NODE_FUNCTION_DEFINITION:	return "NODE_FUNCTION_DEFINITION";
			case NODE_FUNCTION_ARGUMENTS:	return "NODE_FUNCTION_ARGUMENTS";

			case NODE_VARIABLE_DEFINITION:  return "NODE_VARIABLE_DEFINITION";
			case NODE_STATEMENT_BLOCK:		return "NODE_STATEMENT_BLOCK";
			case NODE_STATEMENT:			return "NODE_STATEMENT";
			case NODE_STATEMENT_SEQUENCE:	return "NODE_STATEMENT_SEQUENCE";

			case NODE_ASSIGN:				return "NODE_ASSIGN";
			case NODE_RETURN:				return "NODE_RETURN";
			case NODE_PRINTLN:				return "NODE_PRINTLN";
			case NODE_PRINT:				return "NODE_PRINT";
			case NODE_READLN:				return "NODE_READLN";

			case NODE_IDENTIFIER:			return "NODE_IDENTIFIER";

			case NODE_OR:					return "NODE_OR";
			case NODE_AND:					return "NODE_AND";
			case NODE_EQUAL:				return "NODE_EQUAL";
			case NODE_NOT_EQUAL:			return "NODE_NOT_EQUAL";
			case NODE_LESS:					return "NODE_LESS";
			case NODE_LESS_EQUAL:			return "NODE_LESS_EQUAL";
			case NODE_GREATER:				return "NODE_GREATER";
			case NODE_GREATER_EQUAL:		return "NODE_GREATER_EQUAL";

			case NODE_ADD:					return "NODE_ADD";
			case NODE_SUB:					return "NODE_SUB";
			case NODE_MUL:					return "NODE_MUL";
			case NODE_DIV:					return "NODE_DIV";
			case NODE_UMINUS:				return "NODE_UMINUS";

			case NODE_CONST_INT:			return "NODE_CONST_INT";
			case NODE_CONST_FLOAT:			return "NODE_CONST_FLOAT";
			case NODE_CALL:					return "NODE_CALL";

			case NODE_CAST:					return "NODE_CAST";

			case NODE_IF:					return "NODE_IF";
			case NODE_IF_ELSE:				return "NODE_IF_ELSE";
			case NODE_KEYWORD_VOID:			return "NODE_KEYWORD_VOID";
			case NODE_KEYWORD_INT:			return "NODE_KEYWORD_INT";
			case NODE_KEYWORD_FLOAT:		return "NODE_KEYWORD_FLOAT";
			case NODE_KEYWORD_PRINTLN:		return "NODE_KEYWORD_PRINTLN";
			case NODE_WHILE_STATEMENT:		return "NODE_WHILE_STATEMENT";

			case NODE_PQUEUE_PUSH:			return "NODE_PQUEUE_PUSH";
			case NODE_PQUEUE_POP:			return "NODE_PQUEUE_POP";
			case NODE_PQUEUE_SIZE:			return "NODE_PQUEUE_SIZE";
			case NODE_PQUEUE_TOP:			return "NODE_PQUEUE_TOP";
			case NODE_PQUEUE_TOP_PRIORITY:	return "NODE_PQUEUE_TOP_PRIORITY";

			case NODE_NOT:					return "NODE_NOT";

			case NODE_UNDEFINED:			return "NODE_UNDEFINED";
			default:
				throw std::string("nodeTypeToString: not implemented yet");
			}
		}

	 	static void newline(std::ostream& os, int tabs);

		TreeNode::TreeNode(TreeNodeType type) : nodeType(type), symbolId(SYMBOL_UNDEFINED) {
		}

		TreeNode* TreeNode::append(TreeNode* p_node) {
			childs.push_back(p_node);
			return this;
		}

		 void TreeNode::printTree(std::ostream& os, TreeNode* root, int tabs) {
			if (root->childs.empty()) {
				os << nodeTypeToString(root->nodeType);

				if (root->nodeType == NODE_SYMBOL) {
					os << ' ' << root->symbolId;
				}
			} else {
				os << nodeTypeToString(root->nodeType) <<" {";
				++tabs;

				for (std::list<TreeNode*>::const_iterator it = root->childs.begin();
						it != root->childs.end(); ++it) {

					if (it != root->childs.begin()) {
						os << ",";
					}
					newline(os, tabs);
					printTree(os, *it, tabs);
				}

				--tabs;
				if (!root->childs.empty()) {
					newline(os, tabs);
				}

				os << "}";
			}
		}

		 static void newline(std::ostream& os, int tabs) {
			os << std::endl;

			for (int i = 0; i < 2 * tabs; ++i) {
				os << " ";
			}
		}

		 TreeNode* Compiler::ASTBuilder::TreeNode::at(unsigned n) {
			 assert(childs.size() > n);

		 		std::list<TreeNode*>::iterator it = childs.begin();

		 		for (int i = 0; i < n; ++i) {
		 			++it;
		 		}

		 		return *it;
		 }

		 void TreeNode::setTypeId(unsigned typeId) {
			 assert(nodeType == NODE_CAST);

			 TreeNode::typeId = typeId;
		 }

		 unsigned TreeNode::getTypeId() {
			 assert(nodeType == NODE_CAST);

			 return typeId;
		 }
	}
}


