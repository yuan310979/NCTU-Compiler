all: compiler

compiler: lex.yy.c y.tab.c symbolTable.c codeGenerate.c
	gcc -std=c11 -lm -o compiler lex.yy.c y.tab.c symbolTable.c codeGenerate.c -ly -lfl
y.tab.c: parser.y
	yacc -d -v parser.y
lex.yy.c: new.l
	lex new.l

clean:
	rm -f lex.yy.c y.tab.c y.tab.h y.output parser
