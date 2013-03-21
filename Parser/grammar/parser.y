%require "2.4.1"
%skeleton "lalr1.cc"
%defines
%locations
%define namespace "Compiler::ASTBuilder"
%define parser_class_name "Parser"
%parse-param { Compiler::ASTBuilder::Scanner 			&scanner 	}
%parse-param { Compiler::ASTBuilder::TreeNode** 		pp_root 	}
%parse-param { Compiler::ASTBuilder::SymbolResolver* 	p_resolver 	}
%lex-param   { Compiler::ASTBuilder::Scanner &scanner 				}

%code requires {
	#include <string>
	#include <sstream>
	#include <map>
	#include <list>
	#include "src/SymbolResolver.h"
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
			TreeNode* 		p_root;
			SymbolResolver* p_resolver;
		}
	}
	
	// Prototype for the yylex function
	static int yylex(Compiler::ASTBuilder::Parser::semantic_type * yylval,
	                 Compiler::ASTBuilder::Parser::location_type * yylloc,
	                 Compiler::ASTBuilder::Scanner &scanner);
}

%union  {
	TreeNode*		val;
	std::string*	str;
}
%type <val> exp
%type <val> factor
%type <val> term
%type <val> program
%type <val> statement_seq
%type <val> statement
%type <val> definition_seq

%type <val> local_variable_definition
%type <val> global_variable_definition

%type <val> marker_block_start marker_block_end

%type <val> func_decl
%type <val> block
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
%type <val> while_statement
%type <val> identifier
%type <val> if_statement


%token <tptr> ADD NOT SUB MUL DIV OP CP EOL OB CB SEMICOLON COMMA EQUAL NOT_EQUAL AND OR LESS LESS_EQUAL GREATER GREATER_EQUAL ASSIGN KEYWORD_IF KEYWORD_ELSE KEYWORD_WHILE ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MUL ASSIGN_DIV KEYWORD_PRINTLN
%token <val>  KEYWORD_RETURN  

%type  <str> typename
%token <str> INT_NUMBER  FLOAT_NUMBER  KEYWORD_CHAR IDENTIFIER KEYWORD_VOID KEYWORD_INT KEYWORD_FLOAT KEYWORD_STRUCT KEYWORD_BOOL BOOL_VALUE

%%

program : definition_seq						{ *pp_root = $1; }
 ;

definition_seq : 								{ $$ = new TreeNode(NODE_DEFINITION_SEQUENCE); }
 | definition_seq global_variable_definition	{  }
 | definition_seq func_decl						{ $$ = $1->append($2); }
 ;

global_variable_definition : typename IDENTIFIER SEMICOLON	
	{ p_resolver->insertVariable(*$1, *$2, ALLOCATION_VARIABLE_GLOBAL);  }
 ;
 
local_variable_definition : typename IDENTIFIER SEMICOLON	
	{ p_resolver->insertVariable(*$1, *$2, ALLOCATION_VARIABLE_LOCAL);  }
 ;

typename : KEYWORD_CHAR 										
 | KEYWORD_INT
 | KEYWORD_BOOL
 | KEYWORD_FLOAT									
 | IDENTIFIER
 | KEYWORD_VOID									
 ;
 
func_decl : typename IDENTIFIER OP marker_block_start func_arg_definition CP block marker_block_end 
	{ 
	  std::list<SymbolId> args;
	  TreeNode* p_args = $5;
	  for (std::list<TreeNode*>::const_iterator it = p_args->childs.begin(); it != p_args->childs.end(); ++it) {
	  	args.push_back((*it)->symbolId);
	  }
	  
	  TreeNode* p_node = (new TreeNode(NODE_FUNCTION_DEFINITION))->append($7);
	  p_node->symbolId = p_resolver->insertFunction(*$1, *$2, args);   
	  $$ =  p_node; }
 ;

func_arg_definition : 							{ $$ = new TreeNode(NODE_FUNCTION_ARGUMENTS); }
 | arg_definition_seq							{  }
 ;

arg_definition_seq : arg_definition				{ $$ = (new TreeNode(NODE_FUNCTION_ARGUMENTS))->append($1); }
 | arg_definition_seq COMMA  arg_definition		{ $$ = $1->append($3); }
 

arg_definition : typename IDENTIFIER			
	{ TreeNode* p_node = new TreeNode(NODE_SYMBOL);
	  p_node->symbolId = p_resolver->insertVariable(*$1, *$2, ALLOCATION_VARIABLE_ARGUMENT);
	  $$ =  p_node; }
 ;
 
block : OB marker_block_start statement_seq marker_block_end CB
			{ $$ = (new TreeNode(NODE_STATEMENT_BLOCK))->append($3); }
 ;
 
marker_block_start:								{ p_resolver->push(); }
 ;
 
marker_block_end:								{ p_resolver->pop(); }
 
 
statement_seq : 								{ $$ = new TreeNode(NODE_STATEMENT_SEQUENCE); }
 | statement_seq statement						{ $$ = $1->append($2); }
 | statement_seq local_variable_definition
 ;
 
while_statement: KEYWORD_WHILE OP exp CP statement { $$ = (new TreeNode(NODE_WHILE_STATEMENT))->append($3)->append($5); } 
 ;
 
statement : exp SEMICOLON
 | block
 | while_statement
 | if_statement
 | identifier ASSIGN 	  	exp		SEMICOLON	{ $$ = (new TreeNode(NODE_ASSIGN))->append($1)->append($3); } 									
 | identifier ASSIGN_PLUS 	exp		SEMICOLON	
 | identifier ASSIGN_MINUS	exp  	SEMICOLON
 | identifier ASSIGN_MUL	exp		SEMICOLON
 | identifier ASSIGN_DIV 	exp  	SEMICOLON
 | KEYWORD_RETURN 			exp 	SEMICOLON	{ $$ = (new TreeNode(NODE_RETURN))->append($2); 			}
 | KEYWORD_PRINTLN OP 		exp CP	SEMICOLON	{ $$ = (new TreeNode(NODE_PRINTLN))->append($3);			}
 ;

if_statement: KEYWORD_IF OP exp CP statement				{ $$ = (new TreeNode(NODE_IF))->append($3)->append($5);  }
 |  KEYWORD_IF OP exp CP statement KEYWORD_ELSE statement	{ $$ = (new TreeNode(NODE_IF_ELSE))->append($3)->append($5)->append($7);  }

identifier : IDENTIFIER							
	{  TreeNode* p_node = new TreeNode(NODE_SYMBOL);
	   p_node->symbolId = p_resolver->resolve(*$1);
	   $$ = p_node; }
 ;
 
exp : exp_logic_0
 ;

exp_logic_0 : exp_logic_1
 | exp_logic_0 OR exp_logic_1					{ $$ = (new TreeNode(NODE_OR))->append($1)->append($3); }
 ;

exp_logic_1 : exp_logic_2						
 | exp_logic_1 AND exp_logic_2					{ $$ = (new TreeNode(NODE_AND))->append($1)->append($3); }
 ;

exp_logic_2 : exp_logic_3
 | exp_logic_2 EQUAL exp_logic_3				{ $$ = (new TreeNode(NODE_EQUAL))->append($1)->append($3); }
 | exp_logic_2 NOT_EQUAL exp_logic_3			{ $$ = (new TreeNode(NODE_NOT_EQUAL))->append($1)->append($3); }
 ;

exp_logic_3 : exp_logic_4
 | exp_logic_4 LESS exp_logic_4					{ $$ = (new TreeNode(NODE_LESS))->append($1)->append($3);  }
 | exp_logic_4 LESS_EQUAL exp_logic_4			{ $$ = (new TreeNode(NODE_LESS_EQUAL))->append($1)->append($3); }
 | exp_logic_4 GREATER exp_logic_4				{ $$ = (new TreeNode(NODE_GREATER))->append($1)->append($3);  }
 | exp_logic_4 GREATER_EQUAL exp_logic_4		{ $$ = (new TreeNode(NODE_GREATER_EQUAL))->append($1)->append($3); }
 
exp_logic_4 : exp_arithm
 ;

exp_arithm: factor
 | exp_arithm ADD factor						{ $$ = (new TreeNode(NODE_ADD))->append($1)->append($3); }
 | exp_arithm SUB factor						{ $$ = (new TreeNode(NODE_SUB))->append($1)->append($3); }
 ;

factor: term
 | factor MUL term								{ $$ = (new TreeNode(NODE_MUL))->append($1)->append($3); }
 | factor DIV term								{ $$ = (new TreeNode(NODE_DIV))->append($1)->append($3); }
 ;

term: INT_NUMBER								{ TreeNode* p_node = new TreeNode(NODE_SYMBOL); 
												  p_node->symbolId = p_resolver->insertConst(*$1, SYMBOL_INT);
												  $$ = p_node; }
												  
 | FLOAT_NUMBER									{ TreeNode* p_node = new TreeNode(NODE_SYMBOL); 
												  p_node->symbolId = p_resolver->insertConst(*$1, SYMBOL_FLOAT);
												  $$ = p_node; } 
												  
 | BOOL_VALUE									{ TreeNode* p_node = new TreeNode(NODE_SYMBOL); 
												  p_node->symbolId = p_resolver->insertConst(*$1, SYMBOL_BOOL);
												  $$ = p_node; } 
 
 | identifier									{ }
 | OP exp CP 									{ $$ = $2; } 
 | identifier OP func_arg CP					{ $$ = (new TreeNode(NODE_CALL))->append($1)->append($3); }
 | SUB term										{ $$ = (new TreeNode(NODE_UMINUS))->append($2); 		  }
 | NOT term										{ $$ = (new TreeNode(NODE_NOT))->append($2); 		  }
 ;

func_arg: 										{ $$ = new TreeNode(NODE_FUNCTION_ARGUMENTS); }
 | arg											{ }
 ;
 
arg : exp										{ $$ = (new TreeNode(NODE_FUNCTION_ARGUMENTS))->append($1); }
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

