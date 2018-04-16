

CFLAGS=-O1 -ggdb -Wall
LDFLAGS=-lsqlite3

all: elb_lexer
test: sqlite3_coldefs_test

# .INTERMEDIATE: elbparse.ll.c

%.o: %.c
	gcc -c -ggdb -Wall -O2 -o $@ $^


elbparse.yy.o: elbparse.lex
	flex -o elbparse.yy.c $<
	gcc -c $(CFLAGS) -o $@ elbparse.yy.c

elb_lexer: elbparse.yy.o elb_lexer.o
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f elbparse.yy.c *.o *~

sqlite3_coldefs_test: sqlite3_coldefs_test.o sqlite3_coldefs.o
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^
