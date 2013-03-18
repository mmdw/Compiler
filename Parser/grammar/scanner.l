%{
	#include "src/Scanner.h"
	#include "headers/TreeNode.h"
	#include "src/StringHolder.h"
	
	#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%option nodefault yyclass="Scanner" noyywrap c++

IDENTIFIER		[[:alpha:]][[:alnum:]]*
FLOAT			[[:digit:]]+.[[:digit:]]+
INT				[[:digit:]]+

%%
 
%{
	yylloc->step();
%}


char			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng));  return Compiler::ASTBuilder::Parser::token::KEYWORD_CHAR; }
void 			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng));  return Compiler::ASTBuilder::Parser::token::KEYWORD_VOID; }
int 			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng));  return Compiler::ASTBuilder::Parser::token::KEYWORD_INT; }
struct 			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng));  return Compiler::ASTBuilder::Parser::token::KEYWORD_STRUCT; }
float			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng));  return Compiler::ASTBuilder::Parser::token::KEYWORD_FLOAT; }
println			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng));  return Compiler::ASTBuilder::Parser::token::KEYWORD_PRINTLN; }	
	
return 			{ return Compiler::ASTBuilder::Parser::token::KEYWORD_RETURN; }


{IDENTIFIER} 	{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Compiler::ASTBuilder::Parser::token::IDENTIFIER; }
"+"			 	{ return Compiler::ASTBuilder::Parser::token::ADD; }
"-"			 	{ return Compiler::ASTBuilder::Parser::token::SUB; }
"*"			 	{ return Compiler::ASTBuilder::Parser::token::MUL; }
"/"			 	{ return Compiler::ASTBuilder::Parser::token::DIV; }
";"				{ return Compiler::ASTBuilder::Parser::token::SEMICOLON; }
"("     	 	{ return Compiler::ASTBuilder::Parser::token::OP; }
")"      	 	{ return Compiler::ASTBuilder::Parser::token::CP; }
"{"				{ return Compiler::ASTBuilder::Parser::token::OB; }
"}"				{ return Compiler::ASTBuilder::Parser::token::CB; }
,				{ return Compiler::ASTBuilder::Parser::token::COMMA; }
==				{ return Compiler::ASTBuilder::Parser::token::EQUAL; }
!=				{ return Compiler::ASTBuilder::Parser::token::NOT_EQUAL; }
"&&"			{ return Compiler::ASTBuilder::Parser::token::AND; }
"||"			{ return Compiler::ASTBuilder::Parser::token::OR; }
"<"				{ return Compiler::ASTBuilder::Parser::token::LESS; }
">"				{ return Compiler::ASTBuilder::Parser::token::GREATER; }
"<="			{ return Compiler::ASTBuilder::Parser::token::LESS_EQUAL; }
">="			{ return Compiler::ASTBuilder::Parser::token::GREATER_EQUAL; }
"="				{ return Compiler::ASTBuilder::Parser::token::ASSIGN; }
"+="			{ return Compiler::ASTBuilder::Parser::token::ASSIGN_PLUS; }
"-="			{ return Compiler::ASTBuilder::Parser::token::ASSIGN_MINUS; }
"/="			{ return Compiler::ASTBuilder::Parser::token::ASSIGN_DIV; }
"*="			{ return Compiler::ASTBuilder::Parser::token::ASSIGN_MUL; }

{INT}		 	{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Compiler::ASTBuilder::Parser::token::INT_NUMBER; }
{FLOAT}			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Compiler::ASTBuilder::Parser::token::FLOAT_NUMBER; }

" "            	{ yylloc->step(); }
\t				{ yylloc->step(); }
\n      	 	{ yylloc->step(); }
.				{ throw std::string("unknown token: ") + yytext; }
