#pragma once

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

// Override the interface for yylex since we namespaced it
#undef YY_DECL
#define YY_DECL int Compiler::ASTBuilder::Scanner::yylex()

// Include Bison for types / tokens
#include "../parser.tab.h"

namespace Compiler {
	namespace ASTBuilder {
		class Scanner : public yyFlexLexer {
			public:
				// constructor accepts the input and output streams
				// 0 means std equivilant (stdin, stdout)
				Scanner(std::istream * in = 0, std::ostream * out = 0) : yyFlexLexer(in, out) { }

				// overloaded version of yylex - we need a pointer to yylval and yylloc
				inline int yylex(Parser::semantic_type * lval,
				                 Parser::location_type * lloc);
		
			private:
				// Scanning function created by Flex; make this private to force usage
				// of the overloaded method so we can get a pointer to Bison's yylval
				int yylex();
			
				// point to yylval (provided by Bison in overloaded yylex)
				Parser::semantic_type * yylval;
				
				// pointer to yylloc (provided by Bison in overloaded yylex)
				Parser::location_type * yylloc;
				
				// block default constructor
				Scanner();
				// block default copy constructor
				Scanner(Scanner const &rhs);
				// block default assignment operator
				Scanner &operator=(Scanner const &rhs);
		};
		
		// all our overloaded version does is save yylval and yylloc to member variables
		// and invoke the generated scanner
		int Scanner::yylex(Parser::semantic_type * lval,
		                   Parser::location_type * lloc) {
			yylval = lval;
			yylloc = lloc;
			return yylex();
		}

	}
}

