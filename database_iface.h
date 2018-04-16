#ifndef _DATABASE_IFACE_H_
#define _DATABASE_IFACE_H_

/*
 * The database is expected to implement these calls, 
 * 0 return is good, anything else is bad.
 * the UserData is a void* passed to each call, the database can decide 
 * what to do with it.
 */
// Do whatever necessary to get the database ready for connections and inserts.
extern int db_prepare_connection(int argc, char** argv, void ** ppUserData);


// insert a single row read from filename@lineno
extern int db_insert_elbrow(void* pUserData, const char* filename, int lineno, const struct elb_entry row); 


// end of all runs.
extern int db_finalize(void* pUserData);



#endif _DATABASE_IFACE_H_
