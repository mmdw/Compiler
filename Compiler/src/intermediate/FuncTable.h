/*
 * FuncTable.h
 *
 *  Created on: 15.03.2013
 *      Author: user
 */

#ifndef FUNCTABLE_H_
#define FUNCTABLE_H_

#include <list>
#include <map>
#include "SymbolType.h"

namespace Compiler {
	struct FuncTableRow {
		SymbolId symbolId;
		SymbolType type;

		std::list<SymbolId> args;
	};

	typedef std::map<SymbolId, FuncTableRow> FuncTable;
}
#endif /* FUNCTABLE_H_ */
