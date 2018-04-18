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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/time.h>
#include "sqlite3_coldefs.h"


int main(int argc, char** argv)
{
  sqlite3* db;
  sqlite3_stmt* create_stmt;
  sqlite3_stmt* insert_stmt;
  sqlite3_int64 rowid;
  
  sqlite3_open("testdb.sqlite3", &db);

  
  printf("calling create table ddl\n");

  create_table_ddl(db, "foobar", elblog_file_columns, &create_stmt);
  printf("Table Create produced:\n");
  db_execute_stmt_simple(create_stmt);
  
  create_insert_stmt(db, "foobar", elblog_file_columns, &insert_stmt);
  printf("Table INSERT produced:\n");

  printf("insert a row, and get the rowid\n");

  db_run_insert(insert_stmt, &rowid, elblog_file_columns, "my_filename", time(0)*1.0);
  printf("rowid is %lld\n", rowid);
  db_run_insert(insert_stmt, &rowid, elblog_file_columns, "my_filename2", time(0)*1.0);
  printf("rowid is %lld\n", rowid);
  db_run_insert(insert_stmt, &rowid, elblog_file_columns, "my_filenamesdfa", time(0)*1.0);
  printf("rowid is %lld\n", rowid);

  
  return 0;
}
  
