parsetest: parse.o y.tab.o lex.yy.o errormsg.o util.o absyn.o symbol.o table.o prabsyn.o
	cc -g parse.o y.tab.o lex.yy.o errormsg.o util.o absyn.o symbol.o table.o prabsyn.o -o parsetest

parse.o: parse.c errormsg.h util.h prabsyn.h
	cc -g -c parse.c

prabsyn.o: prabsyn.h
	cc -g -c prabsyn.c

absyn.o: absyn.c
	cc -g -c absyn.c

symbol.o: symbol.c
	cc -g -c symbol.c

table.o: table.c
	cc -g -c table.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	yacc -dv tiger.grm

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f parsetest util.o parsetest.o lex.yy.o errormsg.o y.tab.c y.tab.h y.tab.o absyn.o symbol.o table.o parse.o prabsyn.o
