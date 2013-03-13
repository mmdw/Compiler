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


namespace Compiler {
	typedef unsigned int ConstId;

	enum ConstType {
		CONST_INT,
		CONST_FLOAT
	};

	class ConstTable {
		ConstId count;
		typedef std::pair<ConstId, ConstType> RowType;
		std::map<std::string, RowType> table;

	public:
		ConstTable() : count(0) {

		}
		unsigned put(const std::string& value, ConstType type) {
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
	};
}

#endif /* CONSTTABLE_H_ */
