all: parser

parser: lex.yy.c y.tab.c
	gcc -o parser lex.yy.c y.tab.c -ly -lfl
y.tab.c: parser.y
	yacc -d -v parser.y
lex.yy.c: new.l
	lex new.l

clean:
	rm -f lex.yy.c y.tab.c y.tab.h y.output parser
