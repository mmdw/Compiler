#include "../headers/TreeNode.h"

namespace Compiler {
	namespace ASTBuilder {
	 	static void newline(std::ostream& os, int tabs);

		TreeNode::TreeNode(const char* value) : value(value) {
		}

		TreeNode* TreeNode::append(TreeNode* p_node) {
			childs.push_back(p_node);
			return this;
		}

		 void TreeNode::printTree(std::ostream& os, TreeNode* root, int tabs) {
			if (root->childs.empty()) {
				os << root->value;
			} else {
				os << root->value <<" {";
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
		 		std::list<TreeNode*>::iterator it = childs.begin();

		 		for (int i = 0; i < n; ++i) {
		 			++it;
		 		}

		 		return *it;
		 }
	}
}


