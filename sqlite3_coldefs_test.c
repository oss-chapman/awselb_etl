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
  
  sqlite3_open("testdb.sqlite3", &db);

  
  printf("calling create table ddl\n");

  create_table_ddl(db, "foobar", elblog_file_columns, &create_stmt);
  printf("Table Create produced:\n");
  db_execute_stmt_simple(create_stmt);
  
  create_insert_stmt(db, "foobar", elblog_file_columns, &insert_stmt);
  printf("Table INSERT produced:\n");

  printf("insert a row, and get the rowid\n");
  db_run_insert(insert_stmt, elblog_columns, "my_filename", time(0)*1.0);
  
  return 0;
}
  
