#pragma once

#include <fstream>
#include <istream>

#include "TreeNode.h"

namespace Compiler {
	class TreeBuilder {
		public:
			ASTBuilder::TreeNode* parseStream(std::istream &iniStream);
	};

}
