/*
 * TreeNode.h
 *
 *  Created on: 08.03.2013
 *      Author: user
 */

#ifndef TREENODE_H_
#define TREENODE_H_

#include <cassert>
#include <list>

#include "../tokenizer/Token.h"

namespace Syntax {
using std::list;
using Tokenizer::Token;

/*
 * EXP = EXP + EXP
 * EXP = EXP * EXP
 * EXP = (EXP)
 * EXP = TERM
 */

class TreeNode {
	TreeNode* parent;
	list<TreeNode*> childList;

	const char * nodeName;
	Tokenizer::Token nodeValue;

	bool leaf;

public:
	TreeNode(Tokenizer::Token nodeValue)
		: parent(NULL), nodeName(NULL), nodeValue(nodeValue), leaf(true) {

	}

	TreeNode(const char* nodeName) : parent(NULL), nodeName(nodeName), nodeValue(nodeValue), leaf(false) {

	}

	void addChild(TreeNode* child) {
		assert(!leaf);
		assert(child->parent == NULL);

		child->setParent(this);

		childList.push_back(child);
	}

	void setParent(TreeNode* parent) {
		TreeNode::parent = parent;
	}

	Tokenizer::Token getLeafValue() const {
		assert(leaf);
		return nodeValue;
	}

	const char* getNodeName() {
		assert(!leaf);
		return nodeName;
	}

	list<TreeNode*>& getChildList()  {
		assert(!leaf);
		return childList;
	}

	bool isLeaf() const {
		return leaf;
	}

	~TreeNode() {
		for (list<TreeNode*>::iterator it = childList.begin(); it != childList.end(); ++it) {
			delete *it;
		}
	}

	TreeNode* getParent() {
		return parent;
	}
};

} /* namespace Syntax */
#endif /* TREENODE_H_ */
