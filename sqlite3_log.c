#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3_log.h"

void debug(const char* fmt, ... )
{
  va_list ap;
  fprintf(stdout, __FILE__ ": debug ");
  
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap); 
}

void die(int err, const char* fmt, ... )
{
  va_list ap;
  fprintf(stderr, "%s: DB Error 0x%x ",__FILE__, err);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr,"\nForcing exit\n");
  exit(5);
}
