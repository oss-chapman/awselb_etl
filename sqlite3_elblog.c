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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <zlib.h>

#include "elb_entry.h"
#include "parse_request.h"
#include "database_iface.h"  
#include "sqlite3_log.h"
#include "sqlite3_coldefs.h"


struct table_name_def {
  const char* name;
  const struct my_column_def* cols;
  sqlite3_stmt* stmt;
};

static char* current_filename = NULL;
static sqlite3_int64 current_filename_rowid=0;
static int batch_size = 1;
static int batch_open = 0;
static int batch_records = 0;
static sqlite3_stmt* stmt_begin_tran;
static sqlite3_stmt* stmt_commit_tran;

struct table_name_def table_name_defs[] = {
  {"elblog_file", elblog_file_columns, NULL},
  {"elblog_request", request_columns, NULL},
  {"elblog", elblog_columns, NULL},
  {NULL, NULL, NULL}
};

static const struct option longopts[] = {
  { "help",  0, NULL, 'h' },
  { "batch", 1, NULL, 'b' },
  {0, 0, 0, 0},
};

static void parse_opts(int* argc, char***argv)
{
  int opt;

  
  for(;;) {
    int option_index = 0;
    opt = getopt_long(*argc, *argv, "hb:", longopts, &option_index);
    if (opt == -1) {
      break;
    }
    
    //printf ("getopt returns '%d', \"%s\"\n",opt, optarg);
    switch ( opt ) {
    case 'h':
      printf("The help page would be here\n");
      exit(0);
      
    case 'b':
      batch_size = atoi(optarg);
      break;
      
    case '?':
      break;
      
    default:
      printf("getopt returned 0x%x ??\n", opt);
      break;
    }
  }
  printf("Setting batch size to %d\n", batch_size);
  
  *argc = *argc - optind + 1;
  *argv = *argv + optind - 1;
   
}


static int prepare_statements(struct sqlite3* db)
{
  struct table_name_def* def;

  def = table_name_defs;
  
  while(def->name != NULL) {
    if (create_insert_stmt(db, def->name, def->cols, &def->stmt)) {
      die(0, "create_insert_stmt(%s) failed. ", def->name);
    }
    def++;
  }
  
  if (db_create_transaction_stmts(db, &stmt_begin_tran, &stmt_commit_tran)) {
    die(0, "call to create_transaction_stmts() has failed");
  }
  
  return 0;
}



static int create_tables(sqlite3* db)
{
  int err;
  sqlite3_stmt* stmt;
  struct table_name_def* def = table_name_defs;

  while(def->name != NULL) {
    if (create_table_ddl(db, def->name, def->cols, &stmt)) {
      die(0,"create table %s returned non-zero", def->name);
    }

    if (db_execute_stmt_simple(stmt) != 0) {
      die(0,"create table %s exec returned non-zero", def->name);
    }
    err = sqlite3_finalize(stmt);
    if (err != SQLITE_OK) {
      die(err, "sqlite3_finalize failed for %s", def->name);
    }
    debug("Created table %s\n", def->name);
    def++;
  }
  
  return 0;
}

static int open_connection(struct sqlite3** db)
{
  int err;
  const char* name = "elblog.sqlite3";
  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
    SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_PRIVATECACHE;
  
  err = sqlite3_open_v2(name, db, flags, "unix-excl");

  if (err != SQLITE_OK) {
    die(err, "Failed to open %s, %s.\n", name, sqlite3_errmsg(*db));
  }

  
  return 0;  
}


/**
 *  Step 1 in database connection
 */
int db_prepare_connection(int* argc, char*** argv, void** ppUserData)
{
  int err;
  struct sqlite3* db;

  parse_opts(argc, argv);
    
  err = open_connection(&db);
  if (err) return err;

  err = create_tables(db);
  if (err) return err;

  err = prepare_statements(db);
  if (err) return err;
  
  return 0;
  
}

static int restart_new_batch(void)
{
  int err;

  if (batch_open) {
    err = db_execute_stmt_simple(stmt_commit_tran);
    if (err != SQLITE_OK) {
      die(err, "%s: commit has failed\n%s\n", __PRETTY_FUNCTION__,
          sqlite3_errmsg(sqlite3_db_handle(stmt_commit_tran)));
    }
    printf("batch commit %d records\n", batch_records);
    batch_open = 0;
    batch_records = 0;
  }

  if (batch_size > 1) {
    err = db_execute_stmt_simple(stmt_begin_tran);
    if (err != SQLITE_OK) {
      die(err, "%s: begin  has failed\n%s\n", __PRETTY_FUNCTION__,
          sqlite3_errmsg(sqlite3_db_handle(stmt_begin_tran)));
    }
    batch_open = 1;
    batch_records=0;
  }
  return 0;
}


/**
 * Step 2-n, insert the rows
 */
int db_insert_elbrow(void* pUserData, const char* filename, int lineno,
                     const struct elb_entry* row)
{
  int err;
  sqlite3_int64 request_rowid;
  sqlite3_int64 elbrow_rowid;
  struct parsed_request* pr= NULL;

  batch_records++;
  
  // Handle filename change:
  if (current_filename == NULL || strcmp(current_filename, filename) != 0) {

    restart_new_batch();
    

    if (current_filename != NULL) {
      free(current_filename);
    }
    current_filename = strdup(filename);
    
 
    err = db_run_insert(table_name_defs[0].stmt,
                        &current_filename_rowid,
                        table_name_defs[0].cols,
                        current_filename,
                        time(0) * 1.0);                 
    if (err) return err;
    
    debug("file %s is rowid %lld\n",
          current_filename, current_filename_rowid);
  }
  
  // decode request and insert that:   
  err = parse_request(row->request, &pr);
  if (err) return err;

  err = db_run_insert(table_name_defs[1].stmt,
                      &request_rowid,
                      table_name_defs[1].cols,
                      pr->method,
                      pr->url_protocol,
                      pr->hostname,
                      pr->port,
                      pr->path,
                      pr->args,
                      pr->protocol);
  if (err) return err;
  
  err = free_request(pr);
  if (err) return err;
  pr = NULL;
  //debug("request %lld inserted\n", request_rowid);


  
  // insert the log entry now.
  err = db_run_insert(table_name_defs[2].stmt,
                      &elbrow_rowid,
                      table_name_defs[2].cols,
                      // col 1
                      current_filename_rowid,
                      lineno,
                      row->protocol,
                      row->timestamp,
                      row->elb,
                      // col 6
                      row->client_ipaddr,
                      row->client_port,
                      row->target_ipaddr,
                      row->target_port,
                      row->request_processing_time,
                      // col 11
                      row->target_processing_time,
                      row->response_processing_time,
                      row->elb_status_code,
                      row->target_status_code,
                      row->received_bytes,
                      // col 16
                      row->sent_bytes,
                      request_rowid,
                      row->user_agent,
                      row->ssl_cipher,
                      row->ssl_protocol,
                      // col 21
                      row->target_group_arn,
                      row->trace_id,
                      row->domain_name,
                      row->chosen_cert_arn,
                      row->matched_rule_priority,
                      -999,
                      -999,
                      -999,
                      -999);

  if (err) return err;


  // commit previous batch/ start new batch if needed.
  if (batch_open) {
    if (batch_records >= batch_size) {
      restart_new_batch();
    }
  }
  

  
  //debug("request %lld inserted\n", elbrow_rowid);

  return 0;
}



/**
 * commit, cleanup, build indices, close the conneciton..
 */
int db_finalize(void* pUserData)
{
  sqlite3* db = (sqlite3*)pUserData;

  int err;
  if (batch_open) {
    err = db_execute_stmt_simple(stmt_commit_tran);
    if (err != SQLITE_OK) {
      die(err, "%s: commit has failed\n%s\n", __PRETTY_FUNCTION__, sqlite3_errmsg(db));
    }
    printf("batch commit %d records\n", batch_records);
    batch_open = 0;
    batch_records = 0;
  }
  err = sqlite3_close((sqlite3*) pUserData);
  if (err != SQLITE_OK) {
    die(err, "Failed to close db.\n%s\n", sqlite3_errmsg((sqlite3*)pUserData));
  }
  return 0;
}
