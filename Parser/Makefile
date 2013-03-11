CC     = g++
CFLAGS = -Wall -IC:\Program\ Files\ (x86)\FlexBison
BIN    = Parser.exe
LIB	   = libParser.a

$(LIB) : parser.o scanner.o TreeBuilder.o TreeNode.o
	ar -r libParser.a parser.o scanner.o TreeBuilder.o TreeNode.o

$(BIN) : parser.o scanner.o main.cpp
	$(CC) $(CFLAGS) main.cpp src/TreeBuilder.cpp src/TreeNode.cpp scanner.o parser.o -o $(BIN)

TreeBuilder.o :
	$(CC) $(CFLAGS) -c -o TreeBuilder.o src/TreeBuilder.cpp
	
TreeNode.o : 
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