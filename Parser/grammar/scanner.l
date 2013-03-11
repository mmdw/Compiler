%{
	#include "src/Scanner.h"
	#include "headers/TreeNode.h"
	
	#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%option nodefault yyclass="Scanner" noyywrap c++

%%
 
%{
	yylloc->step();
%}

"+"			 { return Compiler::ASTBuilder::Parser::token::ADD; }
"-"			 { return Compiler::ASTBuilder::Parser::token::SUB; }
"*"			 { return Compiler::ASTBuilder::Parser::token::MUL; }
"/"			 { return Compiler::ASTBuilder::Parser::token::DIV; }

"("     	 { return Compiler::ASTBuilder::Parser::token::OP; }
")"      	 { return Compiler::ASTBuilder::Parser::token::CP; }
[0-9]+		 { yylval->val = new TreeNode(yytext); return Compiler::ASTBuilder::Parser::token::NUMBER; }

.            { yylloc->step(); }
\n      	 { return Compiler::ASTBuilder::Parser::token::EOL; }

