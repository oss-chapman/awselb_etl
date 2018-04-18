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
