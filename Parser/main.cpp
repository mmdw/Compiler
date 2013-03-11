/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include "headers/TreeBuilder.h"
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>

int main(int argc, char * argv[]) {
	Compiler::TreeBuilder iniFile;
	try {
		std::istringstream iss("1+2+3+4");

		Compiler::ASTBuilder::TreeNode::printTree(std::cout, iniFile.parseStream(iss), 0);
	} catch (std::string error) {
		std::cerr << "ERROR: " << error << std::endl;
		return 255;
	}

	return 0;
}

