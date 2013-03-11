/*
 * Syntax.h
 *
 *  Created on: 08.03.2013
 *      Author: user
 */

#ifndef SYNTAX_H_
#define SYNTAX_H_

#include "../tokenizer/Tokenizer.h"
#include "TreeNode.h"

#include <ostream>

namespace Syntax {
using Tokenizer::Tokenizer;
using std::ostream;

class SyntaxExp {
	Tokenizer& tokenizer;
	void cleanUp(TreeNode* node);

	TreeNode* exp(Tokenizer& t);
	TreeNode* exp1(Tokenizer& t);
	TreeNode* term(Tokenizer& t);
	TreeNode* term1(Tokenizer& t);
	TreeNode* factor(Tokenizer& t);

public:
	const char* EXP;
	const char* TERM;


	SyntaxExp(Tokenizer& tokenizer, TreeNode** pp_node);
	virtual ~SyntaxExp();

	void printTree(ostream& os, TreeNode* root, int tabs = 0);
};

} /* namespace Syntax */
#endif /* SYNTAX_H_ */
