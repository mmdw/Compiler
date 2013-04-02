%{
	#include "src/Scanner.h"
	#include "headers/TreeNode.h"
	#include "src/StringHolder.h"
	
	#define YY_USER_ACTION yylloc->columns(yyleng);
	
	typedef Compiler::ASTBuilder::Parser::token Token;
%}


%option nodefault yyclass="Scanner" noyywrap c++

IDENTIFIER		[[:alpha:]][[:alnum:]]*
FLOAT			[[:digit:]]+"."[[:digit:]]+
INT				[[:digit:]]+

%s line_comment comment
%%
 
%{
	yylloc->step();
%}

"//"				BEGIN(line_comment); 
<line_comment>\n 	BEGIN(INITIAL); 
<line_comment>.*	

"/*"				BEGIN(comment);	
<comment>"*/"       BEGIN(INITIAL);
<comment>[^*\n]*        
<comment>"*"+[^*/\n]*

class			{  return Token::KEYWORD_CLASS; }
pqueue			{  return Token::KEYWORD_PQUEUE; }
cast			{  return Token::KEYWORD_CAST; }
char			{  return Token::KEYWORD_CHAR; }
void 			{  return Token::KEYWORD_VOID; }
int 			{  return Token::KEYWORD_INT; }
float			{  return Token::KEYWORD_FLOAT; }
bool			{  return Token::KEYWORD_BOOL; }
while			{  return Token::KEYWORD_WHILE; }
println			{  return Token::KEYWORD_PRINTLN; }
print			{  return Token::KEYWORD_PRINT; }
readln			{  return Token::KEYWORD_READLN; }			
typedef			{ return Token::KEYWORD_TYPEDEF; }
return 			{ return Token::KEYWORD_RETURN; }
if				{ return Token::KEYWORD_IF; }
else			{ return Token::KEYWORD_ELSE; }

"+"			 	{ return Token::ADD; }
"-"			 	{ return Token::SUB; }
"*"			 	{ return Token::MUL; }
"/"			 	{ return Token::DIV; }
";"				{ return Token::SEMICOLON; }
"("     	 	{ return Token::OP; }
")"      	 	{ return Token::CP; }
"{"				{ return Token::OB; }
"}"				{ return Token::CB; }
"!"			 	{ return Token::NOT; }
"."				{ return Token::DOT; }
".push"			{ return Token::PQUEUE_PUSH; }
".pop"			{ return Token::PQUEUE_POP; }
".top"			{ return Token::PQUEUE_TOP; }
".top_priority"	{ return Token::PQUEUE_TOP_PRIORITY; }
".size"			{ return Token::PQUEUE_SIZE; }
,				{ return Token::COMMA; }
==				{ return Token::EQUAL; }
!=				{ return Token::NOT_EQUAL; }
"&&"			{ return Token::AND; }
"||"			{ return Token::OR; }
"<"				{ return Token::LESS; }
">"				{ return Token::GREATER; }
"<="			{ return Token::LESS_EQUAL; }
">="			{ return Token::GREATER_EQUAL; }
"="				{ return Token::ASSIGN; }

true			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Token::BOOL_VALUE; }
false			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Token::BOOL_VALUE; }

{IDENTIFIER} 	{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Token::IDENTIFIER; }
{INT}		 	{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Token::INT_NUMBER; }
{FLOAT}			{ yylval->str = StringHolder::store(new std::string(yytext, yyleng)); return Token::FLOAT_NUMBER; }

" "            	{ yylloc->step(); }
\t				{ yylloc->step(); }
\n      	 	{ yylloc->lines(); }
.				{ throw std::string("unknown token: ") + yytext; }
