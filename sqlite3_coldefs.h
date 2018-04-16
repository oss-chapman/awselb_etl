#ifndef _SQLITE3_COLDEFS_H_
#define _SQLITE3_COLDEFS_H_

struct my_column_def {
  int sqlite_type;
  int insert;
  char name[32];
  char ddl[64];
};


extern void debug(const char* fmt, ... )
  __attribute__((format(gnu_printf, 1, 2)));
extern void die(int err, const char* msg, ... )
  __attribute__((format(gnu_printf, 2, 3), noreturn));


extern const struct my_column_def elblog_file_columns[];
extern const struct my_column_def request_columns[];
extern const struct my_column_def elblog_columns[];

extern int db_execute_stmt_simple(sqlite3_stmt* stmt);

extern int create_table_ddl(sqlite3* db,
                            const char* table_name,
                            const struct my_column_def* cols,
                            sqlite3_stmt** ddl);

extern int create_insert_stmt(sqlite3* db,
                              const char* table_name,
                              const struct my_column_def* cols,
                              sqlite3_stmt** dml);

extern int db_run_insert(sqlite3_stmt* stmt, const struct my_column_def* cols, ...);


#endif //_SQLITE3_COLDEFS_H_
