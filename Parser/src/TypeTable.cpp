#include <ostream>

#include "../headers/TypeTable.h"

namespace Compiler {
namespace ASTBuilder {

TypeId TypeRow::getReturnType() const {
	if (kind != TYPE_KIND_FUNCTION) {
		throw std::string("TypeTable::TypeRow::getReturnType");
	}

	return ref;
}

void TypeRow::setReturnType(TypeId ref) {
	if (kind != TYPE_KIND_FUNCTION) {
		throw std::string("TypeTable::TypeRow::setReturnType");
	}

	TypeRow::ref = ref;
}

TypeId TypeRow::getDefinedType() const {
	if (kind != TYPE_KIND_TYPEDEF) {
		throw std::string("TypeTable::TypeRow::getDefinedType");
	}

	return ref;
}

void TypeRow::setDefinedType(TypeId ref) {
	if (kind != TYPE_KIND_TYPEDEF) {
		throw std::string("TypeTable::TypeRow::setDefinedType");
	}

	TypeRow::ref = ref;
}

std::string TypeRow::getName() const {
	if (kind != TYPE_KIND_TYPEDEF && kind != TYPE_KIND_FUNCTION && kind != TYPE_KIND_BASIC) {
		throw std::string("TypeTable::TypeRow::getName");
	}

	return name;
}

void TypeRow::setName(const std::string& name) {
	if (kind != TYPE_KIND_TYPEDEF && kind != TYPE_KIND_FUNCTION && kind != TYPE_KIND_BASIC) {
		throw std::string("TypeTable::TypeRow::setName");
	}

	TypeRow::name = name;
}


std::list<SymbolId> TypeRow::getArguments() const {
	if (kind != TYPE_KIND_FUNCTION) {
		throw std::string("TypeTable::TypeRow::getArguments");
	}

	return arguments;
}

void TypeRow::setArguments(const std::list<SymbolId>& arguments) {
	if (kind != TYPE_KIND_FUNCTION) {
		throw std::string("TypeTable::TypeRow::setArguments");
	}
	TypeRow::arguments = arguments;
}

TypeId Compiler::ASTBuilder::TypeRow::getItemType() const {
	if (kind != TYPE_KIND_PQUEUE) {
		throw std::string("TypeTable::TypeRow::getItemType");
	}

	return ref;
}


void Compiler::ASTBuilder::TypeRow::setItemType(TypeId ref) {
	if (kind != TYPE_KIND_PQUEUE) {
		throw std::string("setItemType");
	}

	TypeRow::ref = ref;
}


TypeTable::TypeTable()  :
	rows(std::map<TypeId, TypeRow>()),
	typeCount(0),
	mainId(-1),
	LABEL(addLabel()),
	BASIC_VAR(addBasic("var")),
	BASIC_BOOL(addBasic("bool")),
	BASIC_INT(addBasic("int")),
	BASIC_FLOAT(addBasic("float")),
	BASIC_DOUBLE_FLOAT(addBasic("double")),
	BASIC_VOID(addBasic("void")) { }

TypeRow& TypeTable::get(TypeId id) {
	return rows.at(id);
}

TypeId TypeTable::findDefinedType(std::string identifier) {
	for (std::map<TypeId, TypeRow>::const_iterator it = rows.begin(); it != rows.end(); ++it) {
		if (it->second.getKind() == TYPE_KIND_TYPEDEF && it ->second.getName() == identifier) {
			return it->first;
		}
	}

	throw std::string("unknown type: ") + identifier;
}

TypeId TypeTable::insertTypedef(TypeId parentType,
		const std::string& identifier) {

	TypeId id = typeCount++;
	TypeRow row(TYPE_KIND_TYPEDEF);
	row.setName(identifier);
	row.setDefinedType(parentType);

	rows.insert(std::pair<TypeId, TypeRow>(id, row));
	return id;
}

TypeId TypeTable::insertFunction(TypeId returnType,	const std::string& identifier) {
	TypeId id = typeCount++;
	TypeRow row(TYPE_KIND_FUNCTION);

	row.setName(identifier);
	row.setReturnType(returnType);

	rows.insert(std::pair<TypeId, TypeRow>(id, row));

	if (identifier == "main") {
		mainId = id;
	}

	return id;
}

static std::string& align(std::string& str, int tabsc) {
	int len = tabsc - str.length() / 4;
	str += '\t';
	for (int i = 0; i < len; ++i) {
		str += '\t';
	}

	return str;
}

void TypeTable::debug(std::ostream& os) {
	os << "-= TypeTable =-\n";
	for (std::map<TypeId, TypeRow>::const_iterator it = rows.begin(); it != rows.end(); ++it) {
		std::string kind = typeKindToString(it->second.getKind());
		os
			<< it->first << '\t'
			<< align(kind, 2) << '\t'
			<< (it->second.getKind() != TYPE_KIND_LABEL && it->second.getKind() != TYPE_KIND_PQUEUE ? it->second.getName() : "") << std::endl;
	}
}

TypeId Compiler::ASTBuilder::TypeTable::addLabel() {
	TypeId id = typeCount++;
	TypeRow row(TYPE_KIND_LABEL);
	rows.insert(std::pair<TypeId, TypeRow>(id, row));

	return id;
}

TypeId Compiler::ASTBuilder::TypeTable::addBasic(const std::string& name) {
	TypeId id = typeCount++;
	TypeRow row(TYPE_KIND_BASIC);
	row.setName(name);
	rows.insert(std::pair<TypeId, TypeRow>(id, row));
	return id;
}

TypeRow::TypeRow(TypeKind kind) : kind(kind) {
}

TypeKind TypeRow::getKind() const {
	return kind;
}

std::string typeKindToString(TypeKind typeKind) {
	switch (typeKind) {
	case TYPE_KIND_LABEL:		return "LABEL";
	case TYPE_KIND_BASIC:		return "BASIC";
	case TYPE_KIND_TYPEDEF: 	return "TYPEDEF";
	case TYPE_KIND_FUNCTION:	return "FUNCTION";
	case TYPE_KIND_PQUEUE:	return "PQUEUE";
	default: throw std::string("typeKindToString: not implemented yet");
	}
}

TypeId TypeTable::getReferencedType(TypeId typeId) {
	if (get(typeId).getKind() != TYPE_KIND_TYPEDEF) {
		throw std::string("getReferencedType: type is not defined type");
	}

	TypeId id = typeId;
	while (get(id).getKind() == TYPE_KIND_TYPEDEF) {
		id = get(id).getDefinedType();
	}

	return id;
}


TypeId TypeTable::pqueueType(TypeId refType) {
	for (std::map<TypeId, TypeRow>::const_iterator it = rows.begin(); it != rows.end(); ++it) {
		if (it->second.getKind() == TYPE_KIND_PQUEUE && it->second.getItemType() == refType) {
			return it->first;
		}
	}

	TypeId id = typeCount++;

	TypeRow row(TYPE_KIND_PQUEUE);
	row.setItemType(refType);
	rows.insert(std::pair<TypeId, TypeRow>(id, row));

	return id;
}

TypeId Compiler::ASTBuilder::TypeTable::getMainId() {
	if (mainId > 0) {
		return mainId;
	} else {
		throw std::string("main() not found");
	}
}

}
}

