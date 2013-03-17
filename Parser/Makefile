CC     = g++
CFLAGS = -Wall -g -IC:\Program\ Files\ (x86)\FlexBison
BIN    = Parser.exe
LIB	   = libParser.a

$(LIB) : parser.o scanner.o TreeBuilder.o TreeNode.o  StringHolder.o  SymbolTable.o 
	ar -r libParser.a parser.o scanner.o TreeBuilder.o TreeNode.o StringHolder.o  SymbolTable.o 

$(BIN) : parser.o scanner.o main.cpp TreeNode.o StringHolder.o TreeBuilder.o TreeNode.o 
	$(CC) $(CFLAGS) main.cpp SymbolTable.o StringHolder.o TreeBuilder.o TreeNode.o scanner.o parser.o -o $(BIN)

SymbolTable.o : src/SymbolTable.cpp
	$(CC) $(CFLAGS) -c -o SymbolTable.o src/SymbolTable.cpp
	
StringHolder.o : src/StringHolder.cpp
	$(CC) $(CFLAGS) -c -o StringHolder.o src/StringHolder.cpp
	
TreeBuilder.o :  src/TreeBuilder.cpp
	$(CC) $(CFLAGS) -c -o TreeBuilder.o src/TreeBuilder.cpp
	
TreeNode.o : src/TreeNode.cpp
	$(CC) $(CFLAGS) -c -o TreeNode.o src/TreeNode.cpp
	
parser.o : grammar/parser.y
	win_bison grammar/parser.y
	$(CC) $(CFLAGS) -c -o parser.o parser.tab.c 

scanner.o : grammar/parser.y grammar/scanner.l
	win_flex grammar/scanner.l
	$(CC) $(CFLAGS) -c -o scanner.o lex.yy.cc

.PHONY : clean
clean :
	rm -rf parser.tab.c parser.tab.h location.hh position.hh stack.hh
	rm -rf lex.yy.cc
	rm -rf $(BIN) *.o
	rm -rf *.a
	
all: $(LIB)