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
		struct TreeNode {
			std::string value;
			std::list<TreeNode*> childs;

			TreeNode(const char* value);
			TreeNode* append(TreeNode* p_node);
			static void printTree(std::ostream& os, TreeNode* root, int tabs);
		};
	}
}


#endif /* TREENODE_H_ */
