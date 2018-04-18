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
#ifndef _DATABASE_IFACE_H_
#define _DATABASE_IFACE_H_

/*
 * The database is expected to implement these calls, 
 * 0 return is good, anything else is bad.
 * the UserData is a void* passed to each call, the database can decide 
 * what to do with it.
 */
// Do whatever necessary to get the database ready for connections and inserts.
extern int db_prepare_connection(int* argc, char*** argv, void ** ppUserData);


// insert a single row read from filename@lineno
extern int db_insert_elbrow(void* pUserData, const char* filename, int lineno, const struct elb_entry* row); 


// end of all runs.
extern int db_finalize(void* pUserData);



#endif //_DATABASE_IFACE_H_
