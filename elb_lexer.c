#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elb_entry.h"
#include "elb_lexer.h"

int found_aws_elb_entry(const char* filename, int lineno, const struct elb_entry* entry, enum elb_entry_field state)
{
  struct parsed_request* req;
  //printf("got here, entry=%p, lineno=%d state=%d\n", entry, lineno, state);
  parse_request(entry->request, &req);
  
  printf("REQUEST -> method    = %s\n"
         "        -> url_proto = %s\n"
         "        -> hostname  = %s\n"
         "        -> port      = %d\n"
         "        -> path      = %s\n"
         "        -> args      = %s\n"
         "        -> protocol  = %s\n",
         req->method,
         req->url_protocol,
         req->hostname,
         req->port,
         req->path,
         req->args,
         req->protocol);
         

  
  free_request(req);
  return 0;
}




int main(int argc, char** argv)
{
  yy_flex_debug=0;

  db_open

  
  do_run_ddl();
  
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
  
}
