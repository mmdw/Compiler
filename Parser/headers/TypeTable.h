/*
 * TypeTable.h
 *
 *  Created on: 24.03.2013
 *      Author: user
 */

#ifndef TYPETABLE_H_
#define TYPETABLE_H_

#include <string>
#include <map>
#include <list>

#include "definitions.h"

namespace Compiler {
namespace ASTBuilder {

	enum TypeKind {
		TYPE_KIND_LABEL,
		TYPE_KIND_BASIC,
		TYPE_KIND_TYPEDEF,
		TYPE_KIND_FUNCTION,
		TYPE_KIND_PQUEUE,
	};

	std::string typeKindToString(TypeKind typeKind);

	class TypeRow {
	public:
		TypeRow(TypeKind kind);

		TypeId				getItemType() const;
		TypeId 				getReturnType() const;
		TypeId				getDefinedType() const;
		std::string 		getName() const;
		std::list<SymbolId> getArguments() const;
		TypeKind			getKind() const;

		void 				setItemType(TypeId ref);
		void 				setReturnType(TypeId ref);
		void				setDefinedType(TypeId Ref);
		void 				setName(const std::string& name);
		void   				setArguments(const std::list<SymbolId>& args);

	private:
		TypeKind kind;
		TypeId	ref;
		std::string name;
		std::list<TypeId> arguments;
	};

	class TypeTable {
	private:
		std::map<TypeId, TypeRow> rows;
		unsigned typeCount;
		int mainId;

	public:
		const TypeId LABEL;
		const TypeId BASIC_VAR;
		const TypeId BASIC_BOOL;
		const TypeId BASIC_INT;
		const TypeId BASIC_FLOAT;
		const TypeId BASIC_DOUBLE_FLOAT;
		const TypeId BASIC_VOID;

		TypeTable();
		TypeRow& get(TypeId id);

		TypeId findDefinedType(std::string identifier);
		TypeId pqueueType(TypeId refType);
		TypeId insertTypedef(TypeId refType, const std::string& identifier);
		TypeId insertFunction(TypeId returnType, const std::string& identifier);
		TypeId getReferencedType(TypeId typeId);

		TypeId getMainId();

		void debug(std::ostream& os);

	private:
		TypeId addBasic(const std::string& name);
		TypeId addLabel();
	};
}
}



#endif /* TYPETABLE_H_ */
