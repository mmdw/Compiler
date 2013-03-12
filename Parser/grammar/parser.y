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
%type <val> statement_seq
%type <val> statement
%type <val> definition_seq
%type <val> definition
%type <val> func_decl
%type <val> block
%type <val> typename
%type <val> func_arg_definition
%type <val> arg_definition
%type <val> arg_definition_seq
%type <val> func_arg
%type <val> arg
%type <val> exp_arithm
%type <val> exp_logic_0
%type <val> exp_logic_1
%type <val> exp_logic_2
%type <val> exp_logic_3
%type <val> exp_logic_4

%token <tptr> ADD SUB MUL DIV EOL OP CP OB CB SEMICOLON COMMA EQUAL NOT_EQUAL AND OR LESS LESS_EQUAL GREATER GREATER_EQUAL ASSIGN ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MUL ASSIGN_DIV
%token <val> INT_NUMBER KEYWORD_CHAR FLOAT_NUMBER IDENTIFIER KEYWORD_VOID KEYWORD_INT KEYWORD_RETURN

%%

program : definition_seq						{ *pp_root = $1; }
 ;

definition_seq : 								{ $$ = new TreeNode("DEFINITION_SEQUENCE"); }
 | definition_seq definition 					{ $$ = $1->append($2); }
 ;

definition : typename IDENTIFIER SEMICOLON	    { $$ = (new TreeNode("DEFINE_CHAR"))->append($2); }
 | func_decl									{ /*$$ = new TreeNode("FUNC_DECL"); */ }
 ;

typename : KEYWORD_CHAR 										
 | KEYWORD_INT									
 | IDENTIFIER
 | KEYWORD_VOID									
 ;
 
func_decl : typename IDENTIFIER OP func_arg_definition CP block { $$ = (new TreeNode("FUNC_DECL"))->append($1)->append($2)->append($4)->append($6); }
 ;

func_arg_definition : 							{ $$ = new TreeNode("FUNC_ARGS"); }
 | arg_definition_seq							{  }
 ;

arg_definition_seq : arg_definition				{ $$ = (new TreeNode("FUNC_ARGS"))->append($1); }
 | arg_definition_seq COMMA  arg_definition		{ $$ = $1->append($3); }
 

arg_definition : typename IDENTIFIER			{ $$ = (new TreeNode("VAR_DEF"))->append($1)->append($2); }

block : OB statement_seq CB						{ $$ = (new TreeNode("BLOCK"))->append($2); }
 ;
 
statement_seq : 								{ $$ = new TreeNode("INST_SEQUENCE"); }
 | statement_seq statement						{ $$ = $1->append($2); }
 ;
 
statement : exp SEMICOLON
 | IDENTIFIER ASSIGN 	  	exp		SEMICOLON	{ $$ = (new TreeNode("ASSIGN"))->append($1)->append($3); }									
 | IDENTIFIER ASSIGN_PLUS 	exp		SEMICOLON
 | IDENTIFIER ASSIGN_MINUS	exp  	SEMICOLON
 | IDENTIFIER ASSIGN_MUL	exp		SEMICOLON
 | IDENTIFIER ASSIGN_DIV 	exp  	SEMICOLON
 ;

exp : exp_logic_0
 ;

exp_logic_0 : exp_logic_1
 | exp_logic_0 OR exp_logic_1					{ $$ = (new TreeNode("||"))->append($1)->append($3); }
 ;

exp_logic_1 : exp_logic_2						
 | exp_logic_1 AND exp_logic_2					{ $$ = (new TreeNode("&&"))->append($1)->append($3); }
 ;

exp_logic_2 : exp_logic_3
 | exp_logic_2 EQUAL exp_logic_3				{ $$ = (new TreeNode("=="))->append($1)->append($3); }
 | exp_logic_2 NOT_EQUAL exp_logic_3			{ $$ = (new TreeNode("!="))->append($1)->append($3); }
 ;

exp_logic_3 : exp_logic_4
 | exp_logic_4 LESS exp_logic_4					{ $$ = (new TreeNode("<"))->append($1)->append($3); }
 | exp_logic_4 LESS_EQUAL exp_logic_4			{ $$ = (new TreeNode("<="))->append($1)->append($3); }
 | exp_logic_4 GREATER exp_logic_4				{ $$ = (new TreeNode(">"))->append($1)->append($3); }
 | exp_logic_4 GREATER_EQUAL exp_logic_4		{ $$ = (new TreeNode(">="))->append($1)->append($3); }
 
exp_logic_4 : exp_arithm
 ;

exp_arithm: factor
 | exp_arithm ADD factor						{ $$ = (new TreeNode("+"))->append($1)->append($3); }
 | exp_arithm SUB factor						{ $$ = (new TreeNode("-"))->append($1)->append($3); }
 ;

factor: term
 | factor MUL term								{ $$ = (new TreeNode("*"))->append($1)->append($3); }
 | factor DIV term								{ $$ = (new TreeNode("/"))->append($1)->append($3); }
 ;

term: INT_NUMBER
 | FLOAT_NUMBER
 | IDENTIFIER
 | OP exp CP 									{ $$ = $2; } 
 | IDENTIFIER OP func_arg CP					{ $$ = (new TreeNode("CALL"))->append($1)->append($3); }
 ;

func_arg: 										{ $$ = new TreeNode("ARGLIST"); }
 | arg											{ $$ = (new TreeNode("ARGLIST"))->append($1); }
 ;
 
arg : exp										{ $$ = (new TreeNode("FUNC_ARGS"))->append($1); }
 | arg COMMA exp								{ $$ = $1->append($3); }
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

