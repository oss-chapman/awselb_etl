#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <alloca.h>

#include <sqlite3.h>
#include "sqlite3_log.h"
#include "sqlite3_coldefs.h"


const struct my_column_def elblog_file_columns[] = {
  {SQLITE_INTEGER, 0, "id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
  {SQLITE3_TEXT,   1, "filename", "TEXT UNIQUE NOT NULL"},
  {SQLITE_FLOAT,   1, "ts_ingest", "FLOAT NOT NULL"},
  {0,0,"",""}
};

const struct my_column_def request_columns[] = {
  {SQLITE_INTEGER, 0,  "id",     "INTEGER PRIMARY KEY AUTOINCREMENT"},
  {SQLITE3_TEXT,   1,  "method", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "url_protocol", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "hostname", "TEXT NOT NULL"},
  {SQLITE_INTEGER, 1,  "port", "INTEGER NOT NULL"},
  {SQLITE3_TEXT,   1,  "path", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "args", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "protocol", "TEXT NOT NULL"},
  {0, 0, "", ""}
};

const struct my_column_def elblog_columns[] = {
  {SQLITE_INTEGER, 0,  "id",     "INTEGER PRIMARY KEY AUTOINCREMENT"},
  // col 1
  {SQLITE_INTEGER, 1,  "elblog_file_id", "REFERENCES elblog_file(id) ON DELETE CASCADE"},
  {SQLITE_INTEGER, 1,  "elblog_file_lineno", "INTEGER NOT NULL"},
  {SQLITE3_TEXT,   1,  "protocol", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "timestamp", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "elb", "TEXT NOT NULL"},
  // col 6
  {SQLITE3_TEXT,   1,  "client_ipaddr", "TEXT NOT NULL"},
  {SQLITE_INTEGER, 1,  "client_port", "INTEGER NOT NULL"},
  {SQLITE3_TEXT,   1,  "target_ipaddr", "TEXT NOT NULL"},
  {SQLITE_INTEGER, 1,  "target_port", "INTEGER NOT NULL"},
  {SQLITE_FLOAT,   1,  "request_processing_time", "FLOAT NOT NULL"},
  // col 11
  {SQLITE_FLOAT,   1,  "target_processing_time", "FLOAT NOT NULL"},
  {SQLITE_FLOAT,   1,  "response_processing_time", "FLOAT NOT NULL"},
  {SQLITE_INTEGER, 1,  "elb_status_code", "INTEGER NOT NULL"},
  {SQLITE_INTEGER, 1,  "target_status_code", "INTEGER NOT NULL"},
  {SQLITE_INTEGER, 1,  "received_bytes", "INTEGER NOT NULL"},
  // col 16
  {SQLITE_INTEGER, 1,  "sent_bytes", "INTEGER NOT NULL"}, 
  {SQLITE_INTEGER, 1,  "request_id", "REFERENCES request(id) ON DELETE RESTRICT"},
  {SQLITE3_TEXT,   1,  "user_agent", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "ssl_cipher", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "ssl_protocol", "TEXT NOT NULL"},
  // col 21
  {SQLITE3_TEXT,   1,  "target_group_arn", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "trace_id", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "domain_name", "TEXT NOT NULL"},
  {SQLITE3_TEXT,   1,  "chosen_cert_arn", "TEXT NOT NULL"},
  {SQLITE_INTEGER, 1,  "matched_rule_priority", "INTEGER NOT NULL"},
  {0, 0, "", ""}
};



int create_table_ddl(sqlite3* db,
                     const char* table_name,
                     const struct my_column_def* cols,
                     sqlite3_stmt** stmt)
{
  
  char* txt = alloca(2048); // initial string size 2k
  char* ptr = txt;
  int offset;
  int err;
  const struct my_column_def* curr_col = cols;


  offset = sprintf(ptr, "CREATE TABLE IF NOT EXISTS %s ",table_name);
  ptr += offset;

  while (curr_col->sqlite_type != 0) {
    offset = sprintf(ptr, "%c %s %s", (curr_col == cols ? '(' : ','),
                     curr_col->name, curr_col->ddl);
    
    ptr += offset;
    curr_col ++;
  }
  sprintf(ptr, ");");
  
  err = sqlite3_prepare_v2(db, txt, -1, stmt, NULL);
  if (err != SQLITE_OK) {
    die(err, "%s: prepare call failed for \n%s\n", __PRETTY_FUNCTION__, txt);
  }
  
  return 0;
}

int db_execute_stmt_simple(sqlite3_stmt* stmt)
{
  int err;
  
  for (;;) {
    err = sqlite3_step(stmt);
    switch(err) {
    case SQLITE_BUSY:
      break;
    case SQLITE_DONE:
      return 0;
    default:
      die(err, "%s: unexpected return code from sqlite3_step():\n%s\n",__PRETTY_FUNCTION__,
          sqlite3_errmsg(sqlite3_db_handle(stmt)));
    }
  }
}


int create_insert_stmt(sqlite3* db,
                       const char* table_name,
                       const struct my_column_def* cols,
                       sqlite3_stmt** dml)
{
  char* text;
  char* values;
  char* ptr_a;
  char* ptr_v;
  int offset;
  int first;
  int err;
  const struct my_column_def* curr_col;
  
  text  = alloca(2048); // initial string size 2k
  values = alloca(512);
  
  ptr_a = text;
  ptr_v = values;

 
  
  memset (values, 0, 512);


  offset = sprintf(ptr_a, "INSERT INTO %s ",table_name);
  ptr_a += offset;

  curr_col = cols;

  first = 1;
  
  while (curr_col->sqlite_type != 0) {
    if (curr_col->insert) {
      offset = sprintf(ptr_a, "%c %s", (first ? '(' : ','), curr_col->name);
      ptr_a += offset;
    
      offset = sprintf(ptr_v, "%c ?", (first ? '(' : ',') );
      ptr_v += offset;
      first = 0;
    }
      
    curr_col++;

  }
  sprintf(ptr_a, ") VALUES %s );", values);

  err = sqlite3_prepare_v2(db, text, -1, dml, NULL);
  if (err != SQLITE_OK) {
    die(err, "%s: prepare call failed for \n%s\n", __PRETTY_FUNCTION__, text);
  }
  
  
  return 0;
}




int db_run_insert(sqlite3_stmt* stmt, sqlite3_int64 *rowid, const struct my_column_def* cols, ...)
{
  va_list args;
  int err;
  int insert_num;
  int ival;
  double fval;
  const char* cval;
  
  insert_num = 0;
  
  
  err = sqlite3_reset(stmt);
  if (err != SQLITE_OK) {
    goto BIND_BAD_END;
  }

  if (sqlite3_clear_bindings(stmt) != SQLITE_OK) {
    goto BIND_BAD_END;
  }
  
  va_start(args, cols);
  
  while(cols->sqlite_type != 0) {
    if (cols->insert == 1) {
      insert_num++;
      switch (cols->sqlite_type) {
      case SQLITE_INTEGER:
        ival = va_arg(args, int);
        err = sqlite3_bind_int(stmt, insert_num, ival);
        if (err != SQLITE_OK) {
          goto BIND_BAD_END;
        }
        break;
      case SQLITE_FLOAT:
        fval = va_arg(args, double);
        err = sqlite3_bind_double(stmt, insert_num, fval);
        if (err != SQLITE_OK) {
          goto BIND_BAD_END;
        }
        break;
      case SQLITE3_TEXT:
        cval = va_arg(args, char*);
        if (cval == NULL) {
          die(0, "insert value for bind field %s (%d) should not be null\n", cols->name, insert_num);
        }
        err = sqlite3_bind_text(stmt, insert_num, cval, -1, SQLITE_STATIC);
        if (err != SQLITE_OK) {
          goto BIND_BAD_END;
        }
        break;
      default:
        goto BIND_BAD_END;
        break;
      }
    }
    // advance to the next column descriptor.
    cols++;
  }
  va_end(args);
  insert_num += 10000;
  
  err = db_execute_stmt_simple(stmt);
  if (err != 0) {
    goto BIND_BAD_END;

  }
  *rowid = sqlite3_last_insert_rowid(sqlite3_db_handle(stmt));
  
  return 0;
  
 BIND_BAD_END:
  die(err, "%s: failed to bind on column %d, \n%s\n",__PRETTY_FUNCTION__,
      insert_num, sqlite3_errmsg(sqlite3_db_handle(stmt)));
}

  
