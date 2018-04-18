# Author:: Matt Chapman (MatthewaChap@gmail.com)
#
# Copyright (C) 2018  Matt Chapman
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

CFLAGS=-O6 -ggdb -Wall
LDFLAGS=-lsqlite3 -lz

all: awselb_etl

# .INTERMEDIATE: elbparse.yy.c

%.o: %.c
	gcc -c $(CFLAGS) -o $@ $^

elbparse.yy.o: elbparse.lex
	flex -o elbparse.yy.c $<
	gcc -c $(CFLAGS) -o $@ elbparse.yy.c

awselb_etl: elbparse.yy.o elb_lexer.o parse_request.o sqlite3_coldefs.o sqlite3_log.o sqlite3_elblog.o
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f elbparse.yy.c *.o *~

