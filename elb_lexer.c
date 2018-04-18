/*
 * Author:: Matt Chapman (MatthewaChap@gmail.com)
 * 
 * Copyright (C) 2018  Matt Chapman
 *
 *             This program is free software: you can redistribute it and/or modify
 *             it under the terms of the GNU General Public License as published by
 *             the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *             GNU General Public License for more details.
 *
 *             You should have received a copy of the GNU General Public License
 *             along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <zlib.h>

#include "elb_entry.h"
#include "elb_lexer.h"
#include "parse_request.h"
#include "database_iface.h"

static void* userData;

int found_aws_elb_entry(const char* filename, int lineno, const struct elb_entry* entry, enum elb_entry_field state)
{
  return db_insert_elbrow(userData, filename, lineno, entry);
  
}

static void printall(int argc, char** argv){
  int i;
  printf("PRINTALL START: argc=%d\n",argc);
  i=0;
  while ( i < argc) {
    printf("argv[%d] = \"%s\"\n", i, argv[i]);
    i++;
  }
  printf("PRINTALL END\n");
}

int main(int argc, char** argv)
{
  yy_flex_debug=0;

  printall(argc, argv);
  db_prepare_connection(&argc, &argv, &userData);
  printall(argc, argv);
  
  // skip the program name
  if (argc > 1) {
    for (int i=1; i<argc; i++) {
      current_filename = argv[i];
      if (current_filename[0] == '-') {
        printf("Unhandled commandline option %s\n",argv[i]);
        exit(2);
      }
      printf("lexing %s\n", current_filename);
      gzyyin = gzopen(current_filename, "rb");
      yyrestart((FILE*)gzyyin);
      yylex();
    }
  } else {
    printf("usage: elb_lexer [db opts] elbfile.log [elbfile.log ...]n");
  }

  db_finalize(userData);
  
}
