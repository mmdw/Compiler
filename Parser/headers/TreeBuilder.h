#pragma once

#include <fstream>
#include <istream>

#include "TreeNode.h"
#include "SymbolTable.h"

namespace Compiler {
	class TreeBuilder {
		public:
			void parseStream(ASTBuilder::TreeNode** pp_node, ASTBuilder::SymbolTable** pp_table,
					ASTBuilder::TypeTable** pp_type, std::istream &iniStream);
	};
}
