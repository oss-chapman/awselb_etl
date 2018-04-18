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
#ifndef _SQLITE3_COLDEFS_H_
#define _SQLITE3_COLDEFS_H_

struct my_column_def {
  int sqlite_type;
  int insert;
  char name[32];
  char ddl[64];
};


extern const struct my_column_def elblog_file_columns[];
extern const struct my_column_def request_columns[];
extern const struct my_column_def elblog_columns[];

extern int db_execute_stmt_simple(sqlite3_stmt* stmt);

extern int create_table_ddl(sqlite3* db,
                            const char* table_name,
                            const struct my_column_def* cols,
                            sqlite3_stmt** ddl);

extern int db_create_transaction_stmts(sqlite3* db,
                                       sqlite3_stmt** begin_stmt,
                                       sqlite3_stmt** commit_stmt);

extern int create_insert_stmt(sqlite3* db,
                              const char* table_name,
                              const struct my_column_def* cols,
                              sqlite3_stmt** dml);



extern int db_run_insert(sqlite3_stmt* stmt, sqlite3_int64* rowid, const struct my_column_def* cols, ...);


#endif //_SQLITE3_COLDEFS_H_
