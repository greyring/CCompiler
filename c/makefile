CC_HOME=.
CC=gcc -std=c99 -g -Wall
#parser.a:y.tab.o lex.yy.o absync.o symbol.o util.o
#	ar rcs $@ $^
y.tab.o:y.tab.c absync.h symbol.h util.h
	$(CC) -c $< -o $@
lex.yy.o:lex.yy.c y.tab.h absync.h symbol.h util.h
	$(CC) -c $< -o $@
lex.yy.c:lex.l y.tab.h
	lex $<
y.tab.h y.tab.c:parser.y
	bison -b y -d $<
absync.o:absync.c absync.h symbol.h util.h y.tab.h
	$(CC) -c $< -o $@
table.o: table.c util.h table.h
	$(CC) -c $< -o $@
symbol.o:symbol.c symbol.h util.h table.h
	$(CC) -c $< -o $@
util.o:util.c util.h
	$(CC) -c $< -o $@
testAST.o:testAST.c testAST.h absync.h util.h 
	$(CC) -c $< -o $@
test:main.c y.tab.o lex.yy.o absync.o table.o symbol.o util.o testAST.o absync.h y.tab.h
	$(CC) main.c y.tab.o lex.yy.o absync.o table.o symbol.o util.o testAST.o -o $@
clean:
	rm y.tab.h y.tab.c lex.yy.c parser.a *.o *~ *.exe *.gch
	
