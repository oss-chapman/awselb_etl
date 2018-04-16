#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sqlite3.h>
#include <time.h>

#include "parse_request.h"
#include "database_iface.h"  


void db_run_ddl(sqlite3* db, const char* name, const char* stmt)
{
  int err;

  
  
  
  struct csqlite3_stmt* pStmt;
  err = sqlite3_prepare_v3(db,
                           elblog_file,
                           -1, // null terminated strings.
                           0,
                           &pStmt,
                           NULL);
  if (err != SQLITE_OK) {
    db_error_abort(err, "Failure running %s create.\n%s", name, sqlite3_errmsg(db));
  }

  err = 0;
  for(;;) {
    err = sqlite3_step(pStmt);
    switch (err) {
    case SQLITE_DONE:
      goto DDL_DONE;
    case SQLITE_ROW:
      db_error_abort(err, "UNEXPECTED Row returned during %s DDL execution", name);
      break;
    default:
      debug("unexpected return 0x%x, %s from DDL %s", err, sqlite3_errmsg(db), name);
      break;
    }
  }     
 DDL_DONE:
  return;
}

static struct sqlite3_stmt* pStmt_insert_filename;
static struct sqlite3_stmt* pStmt_insert_request;
static struct sqlite3_stmt* pStmt_insert_elblog;


static int prepare_statements(struct sqlite3* db)
{
  err = sqlite3_prepare_v3(db,
                           sql_insert_elblogfile,
                           -1, // null terminated strings.
                           0,
                           &pStmt_insert_filename,
                           NULL);
  if (err != SQLITE_OK) {
    db_error_abort(err, "UNEXPECTED Row returned during %s DDL execution", name);
  }
  err = sqlite3_prepare_v3(db,
                           sql_insert_request,
                           -1, // null terminated strings.
                           0,
                           &pStmt_insert_request,
                           NULL);
  if (err != SQLITE_OK) {
    db_error_abort(err, "UNEXPECTED Row returned during %s DDL execution", name);
  }
  err = sqlite3_prepare_v3(db,
                           sql_insert_elblog,
                           -1, // null terminated strings.
                           0,
                           &pStmt_insert_elblog,
                           NULL);
  if (err != SQLITE_OK) {
    db_error_abort(err, "UNEXPECTED Row returned during %s DDL execution", name);
  }
  
  return 0;
}



static int create_tables(sqlite3* db)
{
  
  
  db_run_ddl(db, "elblog_file", elblog_file);
  db_run_ddl(db, "request", request);
  db_run_ddl(db, "elblog", elblog);
  return 0;
}

static int open_connection(int argc, char** argv, struct sqlite3** db)
{
  int err;
  int dbflags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
    SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_PRIVATECACHE;
  
  err = sqlite3_open_v2("elblogs.sqlite3", db, flags, "unix-excl");

  if (err != SQLITE_OK) {
    db_error_abort(err, "Failed to open %s, %s.\n", name, sqlite3_errmsg(db));
  }

  
  return 0;  
}


/**
 *  Step 1 in database connection
 */
int db_prepare_connection(int argc, char** argv, void** ppUserData)
{
  int err;
  struct sqlite3* db;
  
  err = open_connection(argc, argv, &db);
  if (err) return err;

  err = create_tables(db);
  if (err) return err;

  err = prepare_statements(db);
  if (err) return err;
  
  return 0;
  
}




static int db_bind_filename(struct sqlite3* db, const char* filename)
{
  char ts[64];

  snprintf(ts, sizeof(ts), "%g", gettimeofday());
  
  err = sqlite3_reset(pStmt_insert_filename);
  if (err != SQLITE_OK) {
    db_error_abort(err, "Failed to reset stmt_filename %s.\n", sqlite3_errmsg(db));
  }
  err = sqlite3_bind_text(pStmt_insert_filename, 1, filename, -1, SQLITE_STATIC, NULL);
  if (err != SQLITE_OK) {
    db_error_abort(err, "Failed to bind arg 1 stmt_filename %s.\n", sqlite3_errmsg(db));
  }

  err = sqlite3_bind_text(pStmt_insert_filename, 2, ts, -1, SQLITE_TRANSIENT, NULL);
  if (err != SQLITE_OK) {
    db_error_abort(err, "Failed to bind arg 1 stmt_filename %s.\n", sqlite3_errmsg(db));
  }
  return 0;
}

static int db_bind_request(struct sqlite3* db, const struct 



/**
 * Step 2-n, insert the rows
 */
int db_insert_elbrow(void* pUserData, const char* filename, int lineno,
                     const struct elb_entry row)
{
  if (current_filename == NULL ||
      strcmp(current_filename, filename) != 0) {
    if (current_filename != NULL) {
      free(current_filename);
    }
    current_filename = strdup(filename);

    err = db_bindfilename(db, filename);
    if (err) {
      return err;
    }
    

    


  
}


/**
 * commit, cleanup, build indices, close the conneciton..
 */
int db_finalize(void* pUserData)
{
  int err  = sqlite3_close((sqlite3*) pUserData);
  if (err != SQLITE_OK) {
    db_error_abort(err, "Failed to close db, %s.\n", name, sqlite3_errmsg(db));
  }
  return 0;
}
