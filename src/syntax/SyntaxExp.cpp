/*
 * Syntax.cpp
 *
 *  Created on: 08.03.2013
 *      Author: user
 */

#include "../tokenizer/TokenName.h"
#include "SyntaxExp.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

namespace Syntax {
using std::cerr;
using std::cout;

SyntaxExp::SyntaxExp(Tokenizer& tokenizer, TreeNode** pp_node) : tokenizer(tokenizer),
		EXP("EXP"), TERM("TERM") {

	*pp_node = exp(tokenizer);
//	cleanUp(*pp_node);
}


static void error(const char* message) {
	cerr << "Syntax Analyzer error: " << message;
	exit(-1);
}

/**
 * EXP = TERM EXP1
 */
TreeNode* SyntaxExp::exp(Tokenizer& tokenizer) {
	TreeNode* result = new TreeNode(EXP);

	result->addChild(term(tokenizer));
	result->addChild(exp1(tokenizer));

	return result;
}

/*
 * EXP1	= +EXP | -EXP | e
 *
 * EXP1 = EXP + T | EXP - T | e
 */
TreeNode* SyntaxExp::exp1(Tokenizer& t) {
	TreeNode* result = new TreeNode("EXP1");
	TokenName name = t.getToken().getName();

	if (name == OP_PLUS || name == OP_MINUS) {
		result->addChild(new TreeNode(t.getToken()));
		t.goNext();

		result->addChild(exp(t));
	}

	return result;
}

/**
 * TERM = FACTOR TERM1
 */
TreeNode* SyntaxExp::term(Tokenizer& t) {
	TreeNode* result = new TreeNode(TERM);

	result->addChild(factor(t));
	result->addChild(term1(t));

	return result;
}


/**
 * FACTOR = X | Y | ... | (EXP) | e
 */
TreeNode* SyntaxExp::factor(Tokenizer& t) {
	TreeNode* result = new TreeNode("FACTOR");
	TokenName name = t.getToken().getName();

	if (name == LEFT_ROWND_BRACKET) {
		t.goNext();	// (

		result->addChild(exp(t));

		if (t.getToken().getName() == RIGHT_ROWND_BRACKET) {
			t.goNext(); // )
		} else {
			error(") expected");
		}
	} else if (name == CONST_INT || name == CONST_FLOAT) {
		result->addChild(new TreeNode(t.getToken()));
		t.goNext(); // CONST
	} else {
		error("expected: FACTOR");
	}

	return result;
}

/**
 * TERM1 = * TERM | / TERM | e
 */
TreeNode* SyntaxExp::term1(Tokenizer& t) {
	TokenName name = t.getToken().getName();
	TreeNode* result = new TreeNode("TERM1");

	if (name == OP_MUL || name == OP_DIV) {
		result->addChild(new TreeNode(t.getToken()));
		t.goNext();

		result->addChild(term(t));
	}

	return result;
}

SyntaxExp::~SyntaxExp() {

}

static void newline(std::ostream& os, int tabs) {
	os << std::endl;

	for (int i = 0; i < 2 * tabs; ++i) {
		os << " ";
	}
}

void SyntaxExp::cleanUp(TreeNode* p_node) {
	if (!p_node->isLeaf()) {
		list<TreeNode*> temp = p_node->getChildList();

		for (list<TreeNode*>::iterator it = temp.begin();
				it != temp.end(); ++it) {

			cleanUp(*it);
		}

		if (p_node->getNodeName() != EXP && p_node->getNodeName() != TERM) {
			list<TreeNode*>& parentChilds = p_node->getParent()->getChildList();

			list<TreeNode*>::iterator pos = std::find(parentChilds.begin(), parentChilds.end(), p_node);
			assert(pos != parentChilds.end());

			for (list<TreeNode*>::iterator it = p_node->getChildList().begin();
					it != p_node->getChildList().end(); ++it) {

				(*it)->setParent(p_node->getParent());

				parentChilds.insert(pos, (*it));
			}

			p_node->getChildList().clear();
			parentChilds.remove(p_node);

			delete p_node;
		}
	}
}

void SyntaxExp::printTree(ostream& os, TreeNode* root, int tabs) {
	if (root->isLeaf()) {
		cout << tokenizer.tokenToString(root->getLeafValue());
	} else {
		os << root->getNodeName() <<" {";
		++tabs;

		for (list<TreeNode*>::const_iterator it = root->getChildList().begin();
				it != root->getChildList().end(); ++it) {

			if (it != root->getChildList().begin()) {
				os << ",";
			}
			newline(os, tabs);
			printTree(os, *it, tabs);
		}

		--tabs;
		if (!root->getChildList().empty()) {
			newline(os, tabs);
		}

		os << "}";
	}
}

} /* namespace Syntax */
