

CFLAGS=-O6 -ggdb -Wall
LDFLAGS=-lsqlite3 -lz

all: elb_lexer
test: sqlite3_coldefs_test

# .INTERMEDIATE: elbparse.ll.c

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^


elbparse.yy.o: elbparse.lex
	flex -o elbparse.yy.c $<
	gcc -c $(CFLAGS) -o $@ elbparse.yy.c

elb_lexer: elbparse.yy.o elb_lexer.o parse_request.o sqlite3_coldefs.o sqlite3_log.o sqlite3_elblog.o
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f elbparse.yy.c *.o *~

sqlite3_coldefs_test: sqlite3_coldefs_test.o sqlite3_coldefs.o sqlite3_log.o
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^
