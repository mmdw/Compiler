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

%error-verbose

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
	TypeId			typeId;
	std::string*	str;
}

%type <val> program
%type <val> statement statement_seq
%type <val> local_variable_definition
%type <val> local_variable_definition_with_init
%type <val> global_variable_definition
%type <val> global_variable_definition_with_init
%type <val> definition_seq
%type <val> marker_block_start marker_block_end
%type <val> func_def func_def_1 func_def_2
%type <val> type_def
%type <val> block
%type <val> func_arg_definition
%type <val> arg_definition arg_definition_seq
%type <val> func_arg arg
%type <val> exp exp_0 exp_1 exp_2 exp_3 exp_4 exp_5 exp_6 exp_7 
%type <val> while_statement identifier if_statement

%token <tptr> ADD  NOT SUB CB MUL DIV DOT OP CP EOL OR OB  INC DEC SEMICOLON AND COMMA
%token <tptr> EQUAL				NOT_EQUAL		LESS 			LESS_EQUAL 		GREATER 		GREATER_EQUAL 	ASSIGN 
%token <tptr> KEYWORD_VOID 		KEYWORD_IF 		KEYWORD_ELSE 	KEYWORD_WHILE 	KEYWORD_PRINTLN KEYWORD_PRINT
%token <tptr> KEYWORD_PQUEUE 	KEYWORD_CAST 	KEYWORD_READLN 	KEYWORD_RETURN	KEYWORD_CLASS	KEYWORD_NAMESPACE
%token <tptr> KEYWORD_TYPEDEF 	PQUEUE_PUSH 	PQUEUE_POP 		PQUEUE_SIZE 	PQUEUE_TOP 		PQUEUE_TOP_PRIORITY
%token <tptr> KEYWORD_VAR


%type <typeId> typename
%token <str> IDENTIFIER 		INT_NUMBER  	FLOAT_NUMBER  	KEYWORD_CHAR 	KEYWORD_INT 	KEYWORD_FLOAT 	KEYWORD_BOOL 	BOOL_VALUE

%%

program : KEYWORD_NAMESPACE IDENTIFIER OB KEYWORD_CLASS IDENTIFIER OB definition_seq CB CB	{ *pp_root = $7; }
 ;

definition_seq : 											{ $$ = new TreeNode(NODE_DEFINITION_SEQUENCE); }
 | definition_seq func_def									{ $$ = $1->append($2); }
 | definition_seq global_variable_definition				
 | definition_seq global_variable_definition_with_init		{ $$ = $1->append($2); }
 | definition_seq type_def
 | KEYWORD_NAMESPACE IDENTIFIER OB definition_seq CB		
 ;

global_variable_definition : typename IDENTIFIER SEMICOLON	{ p_resolver->insertVariable($1, *$2, ALLOCATION_VARIABLE_GLOBAL);  }
 ;

global_variable_definition_with_init : typename IDENTIFIER ASSIGN exp SEMICOLON
	{ TreeNode* p_identifier = new TreeNode(NODE_SYMBOL);
	  p_identifier->symbolId = p_resolver->insertVariable($1, *$2, ALLOCATION_VARIABLE_GLOBAL); 
	  $$ = (new TreeNode(NODE_ASSIGN))->append(p_identifier)->append($4); }
 ;
 
local_variable_definition : typename IDENTIFIER SEMICOLON	
	{ p_resolver->insertVariable($1, *$2, ALLOCATION_VARIABLE_LOCAL); }
 ;
 
local_variable_definition_with_init : typename IDENTIFIER ASSIGN exp SEMICOLON
	{ TreeNode* p_identifier = new TreeNode(NODE_SYMBOL);
	  p_identifier->symbolId = p_resolver->insertVariable($1, *$2, ALLOCATION_VARIABLE_LOCAL);
	  $$ = (new TreeNode(NODE_ASSIGN))->append(p_identifier)->append($4); }
 ;

type_def: KEYWORD_TYPEDEF typename IDENTIFIER SEMICOLON
	{ p_resolver->type()->insertTypedef($2, *$3); }

typename : KEYWORD_INT								{ $$ = p_resolver->type()->BASIC_INT; }		
 | KEYWORD_BOOL										{ $$ = p_resolver->type()->BASIC_BOOL; }
 | KEYWORD_FLOAT									{ $$ = p_resolver->type()->BASIC_FLOAT; }
 | KEYWORD_VAR										{ $$ = p_resolver->type()->BASIC_VAR; }
 | IDENTIFIER										{ $$ = p_resolver->type()->findDefinedType(*$1); }
 | KEYWORD_VOID										{ $$ = p_resolver->type()->BASIC_VOID; }
 | KEYWORD_PQUEUE LESS typename GREATER				{ $$ = p_resolver->type()->pqueueType($3); }
 ;
 
func_def :  func_def_1 block marker_block_end 		{ $$->append($2); }
 ;

func_def_1 : func_def_2  OP func_arg_definition CP
	{ p_resolver->insertFunctionArgs($1->symbolId, $3); 
	  $$ = $1; }
 ;

func_def_2 : typename IDENTIFIER 									
	{ 	  TreeNode* p_node = new TreeNode(NODE_FUNCTION_DEFINITION);
	 	  p_node->symbolId = p_resolver->insertFunction($1, *$2); 
	 	  $$ =  p_node;  }
 ;

func_arg_definition : 							{ $$ = new TreeNode(NODE_FUNCTION_ARGUMENTS); }
 | arg_definition_seq							{  }
 ;

arg_definition_seq : arg_definition				{ $$ = (new TreeNode(NODE_FUNCTION_ARGUMENTS))->append($1); }
 | arg_definition_seq COMMA  arg_definition		{ $$ = $1->append($3); }
 

arg_definition : typename IDENTIFIER			{ TreeNode* p_node = new TreeNode(NODE_SYMBOL);
												  p_node->symbolId = p_resolver->insertVariable($1, *$2, ALLOCATION_VARIABLE_ARGUMENT);
												  $$ =  p_node; }
 ;
 
block : OB marker_block_start statement_seq marker_block_end CB
			{ $$ = (new TreeNode(NODE_STATEMENT_BLOCK))->append($3); }
 ;
 
marker_block_start:								{ p_resolver->push(); }
 ;
 
marker_block_end:								{ p_resolver->pop(); }
 ;
 
statement_seq : 										{ $$ = new TreeNode(NODE_STATEMENT_SEQUENCE); }
 | statement_seq statement								{ $$ = $1->append($2); }
 | statement_seq local_variable_definition
 | statement_seq local_variable_definition_with_init	{ $$ = $1->append($2); }
 ;

while_statement: KEYWORD_WHILE OP exp CP statement { $$ = (new TreeNode(NODE_WHILE_STATEMENT))->append($3)->append($5); } 
 ;
 
statement : exp SEMICOLON
 | block
 | while_statement
 | if_statement
 | identifier ASSIGN 	  	exp				SEMICOLON				{ $$ = (new TreeNode(NODE_ASSIGN))->append($1)->append($3); } 									
 | KEYWORD_RETURN 			exp 			SEMICOLON				{ $$ = (new TreeNode(NODE_RETURN))->append($2); 			}
 | KEYWORD_PRINTLN OP 		exp CP			SEMICOLON				{ $$ = (new TreeNode(NODE_PRINTLN))->append($3);			}
 | KEYWORD_PRINT   OP 		exp CP			SEMICOLON				{ $$ = (new TreeNode(NODE_PRINT))->append($3);				}
 | KEYWORD_READLN  OP		identifier CP 	SEMICOLON  			 	{ $$ = (new TreeNode(NODE_READLN))->append($3);				}
 ;

if_statement: KEYWORD_IF OP exp CP statement						{ $$ = (new TreeNode(NODE_IF))->append($3)->append($5);  }
 |  KEYWORD_IF OP exp CP statement KEYWORD_ELSE statement			{ $$ = (new TreeNode(NODE_IF_ELSE))->append($3)->append($5)->append($7);  }

identifier : IDENTIFIER							
	{  TreeNode* p_node = new TreeNode(NODE_SYMBOL);
	   p_node->symbolId = p_resolver->resolve(*$1);
	   $$ = p_node; }
 ;
 
exp : exp_0
 ;

exp_0 : exp_1
 | exp_0 OR exp_1					{ $$ = (new TreeNode(NODE_OR))->append($1)->append($3); }
 ;

exp_1 : exp_2						
 | exp_1 AND exp_2					{ $$ = (new TreeNode(NODE_AND))->append($1)->append($3); }
 ;

exp_2 : exp_3
 | exp_2 EQUAL exp_3				{ $$ = (new TreeNode(NODE_EQUAL))->append($1)->append($3); }
 | exp_2 NOT_EQUAL exp_3			{ $$ = (new TreeNode(NODE_NOT_EQUAL))->append($1)->append($3); }
 ;

exp_3 : exp_4
 | exp_4 LESS exp_4					{ $$ = (new TreeNode(NODE_LESS))->append($1)->append($3);  }
 | exp_4 LESS_EQUAL exp_4			{ $$ = (new TreeNode(NODE_LESS_EQUAL))->append($1)->append($3); }
 | exp_4 GREATER exp_4				{ $$ = (new TreeNode(NODE_GREATER))->append($1)->append($3);  }
 | exp_4 GREATER_EQUAL exp_4		{ $$ = (new TreeNode(NODE_GREATER_EQUAL))->append($1)->append($3); }
 ;
 

exp_4: exp_5
 | exp_4 ADD exp_5					{ $$ = (new TreeNode(NODE_ADD))->append($1)->append($3); }
 | exp_4 SUB exp_5					{ $$ = (new TreeNode(NODE_SUB))->append($1)->append($3); }
 ;

exp_5: exp_6
 | exp_5 MUL exp_6					{ $$ = (new TreeNode(NODE_MUL))->append($1)->append($3); }
 | exp_5 DIV exp_6					{ $$ = (new TreeNode(NODE_DIV))->append($1)->append($3); }
 ;

exp_6: exp_7
 | exp_7 INC						{ $$ = (new TreeNode(NODE_INC))->append($1); 		  	}
 | exp_7 DEC 						{ $$ = (new TreeNode(NODE_DEC))->append($1); 		  	}
 | SUB exp_6						{ $$ = (new TreeNode(NODE_UMINUS))->append($2); 		  	}
 | NOT exp_6						{ $$ = (new TreeNode(NODE_NOT))->append($2); 		 	  	}
 | KEYWORD_CAST	LESS typename GREATER OP exp CP	
 									{ TreeNode* p_node = (new TreeNode(NODE_CAST))->append($6);
 									  p_node->setTypeId($3); 		 	  
 									  $$ = p_node; }
 ;
 
exp_7: INT_NUMBER					{ TreeNode* p_node = new TreeNode(NODE_SYMBOL); 
									  p_node->symbolId = p_resolver->insertConst(*$1, p_resolver->type()->BASIC_INT);
									  $$ = p_node; }
												  
 | FLOAT_NUMBER						{ TreeNode* p_node = new TreeNode(NODE_SYMBOL); 
									  p_node->symbolId = p_resolver->insertConst(*$1, p_resolver->type()->BASIC_FLOAT);
									  $$ = p_node; } 
												  
 | BOOL_VALUE						{ TreeNode* p_node = new TreeNode(NODE_SYMBOL); 
									  p_node->symbolId = p_resolver->insertConst(*$1, p_resolver->type()->BASIC_BOOL);
									  $$ = p_node; } 
 | identifier						
 | OP exp CP 						{ $$ = $2; } 
 | identifier OP func_arg CP		{ $$ = (new TreeNode(NODE_CALL))->append($1)->append($3); 	} 

 | exp_7 PQUEUE_TOP  OP CP			{ $$ = (new TreeNode(NODE_PQUEUE_TOP))->append($1);							}
 | exp_7 PQUEUE_SIZE  OP CP						{ $$ = (new TreeNode(NODE_PQUEUE_SIZE))->append($1);						}
 | exp_7 PQUEUE_TOP_PRIORITY  OP CP				{ $$ = (new TreeNode(NODE_PQUEUE_TOP_PRIORITY))->append($1);				}
  
 | exp_7 PQUEUE_PUSH  OP exp COMMA exp CP		{ $$ = (new TreeNode(NODE_PQUEUE_PUSH))->append($1)->append($4)->append($6);}
 | exp_7 PQUEUE_POP   OP CP						{ $$ = (new TreeNode(NODE_PQUEUE_POP))->append($1);							}
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
	ret << "Parser Error at line " << loc.begin.line << " column " << loc.begin.column << ": " << msg;
	throw ret.str();
}

#include "src/Scanner.h"
static int yylex(Compiler::ASTBuilder::Parser::semantic_type * yylval,
                 Compiler::ASTBuilder::Parser::location_type * yylloc,
                 Compiler::ASTBuilder::Scanner &scanner) {
	
	return scanner.yylex(yylval, yylloc);
}