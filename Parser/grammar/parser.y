%require "2.4.1"
%skeleton "lalr1.cc"
%defines
%locations
%define namespace "Compiler::ASTBuilder"
%define parser_class_name "Parser"
%parse-param { Compiler::ASTBuilder::Scanner &scanner }
%parse-param { Compiler::ASTBuilder::TreeNode** pp_root }
%lex-param   { Compiler::ASTBuilder::Scanner &scanner }

%code requires {
	#include <string>
	#include <sstream>
	#include <map>
	#include "headers/TreeNode.h"
	
	namespace Compiler {
		namespace ASTBuilder {
			class Scanner;
		}
	}
}

%code {
	namespace Compiler {
		namespace ASTBuilder {
			TreeNode* p_root;
		}
	}
	
	// Prototype for the yylex function
	static int yylex(Compiler::ASTBuilder::Parser::semantic_type * yylval,
	                 Compiler::ASTBuilder::Parser::location_type * yylloc,
	                 Compiler::ASTBuilder::Scanner &scanner);
}

%union  {
	TreeNode*	val;
}
%type <val> exp
%type <val> factor
%type <val> term
%type <val> program

%token <tptr> ADD SUB MUL DIV EOL OP CP
%token <val> NUMBER
%%

program : exp 		{ *pp_root = $1; }
 ;

exp: factor
 | exp ADD exp 		{ $$ = (new TreeNode("+"))->append($1)->append($3); }
 | exp SUB factor	{ $$ = (new TreeNode("-"))->append($1)->append($3); }
 ;

factor: term
 | factor MUL term	{ $$ = (new TreeNode("*"))->append($1)->append($3); }
 | factor DIV term	{ $$ = (new TreeNode("/"))->append($1)->append($3); }
 ;

term: NUMBER
 | OP exp CP { $$ = $2; } 
 ;
	
%%

void Compiler::ASTBuilder::Parser::error(const Compiler::ASTBuilder::Parser::location_type &loc,
                                          const std::string &msg) {
	std::ostringstream ret;
	ret << "Parser Error at " << loc << ": " << msg;
	throw ret.str();
}

#include "src/Scanner.h"
static int yylex(Compiler::ASTBuilder::Parser::semantic_type * yylval,
                 Compiler::ASTBuilder::Parser::location_type * yylloc,
                 Compiler::ASTBuilder::Scanner &scanner) {
	
	
	return scanner.yylex(yylval, yylloc);
}

