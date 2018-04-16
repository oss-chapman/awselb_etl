#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elb_entry.h"
#include "elb_lexer.h"
#include "parse_request.h"
#include "database_iface.h"


static void* userData;


int found_aws_elb_entry(const char* filename, int lineno, const struct elb_entry* entry, enum elb_entry_field state)
{
  return db_insert_elbrow(userData, filename, lineno, entry);
  
}




int main(int argc, char** argv)
{
  yy_flex_debug=0;

  db_prepare_connection(argc, argv, &userData);
  
  // skip the program name
  if (argc > 1) {
    for (int i=1; i<argc; i++) {
      current_filename = argv[i];
      printf("lexing %s\n", current_filename);
      yyrestart(fopen(argv[i], "r"));
      yylex();
    }
  } else {
    current_filename="-stdin-";
    yyrestart(stdin);
    yylex();
  }

  db_finalize(userData);
  
}
