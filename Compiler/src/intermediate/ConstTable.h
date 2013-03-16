/*
 * ConstTable.h
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#ifndef CONSTTABLE_H_
#define CONSTTABLE_H_

#include <ostream>
#include <string>
#include <map>

#include "SymbolType.h"

namespace Compiler {
	typedef unsigned int ConstId;

	class ConstTable {
		ConstId count;
		typedef std::pair<ConstId, SymbolType> RowType;
		std::map<std::string, RowType> table;

	public:
		ConstTable() : count(0) {

		}
		unsigned put(const std::string& value, SymbolType type) {
			std::map<std::string, RowType>::iterator it = table.find(value);

			if (it != table.end()) {
				return it->second.first;
			}

			ConstId id = count++;
			table.insert(std::pair<std::string, RowType> (value, RowType(id, type)));

			return id;
		}

		void debug(std::ostream& os) {
			os << "-CONST_TABLE--------------" << std::endl;
			for (std::map<std::string, RowType>::iterator it = table.begin(); it != table.end(); ++it) {
				os << it->first << '\t' << it->second.first << std::endl;
			}
		}

		void generateCode(std::ostream& os) {
			os << "section '.data' readable" << std::endl;
			for (std::map<std::string, RowType>::iterator it = table.begin(); it != table.end(); ++it) {
				os << "__const_" << it->second.first << "\t\t";
				if (it->second.second == SYMBOL_FLOAT) {
					os << "dd " << it->first << 'f';
				} else if (it->second.second == SYMBOL_INT) {
					os << "dd " << it->first;
				}

				os << std::endl;
			}
		}
	};
}

#endif /* CONSTTABLE_H_ */
