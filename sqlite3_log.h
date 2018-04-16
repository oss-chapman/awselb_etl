#ifndef _SQLITE3_LOG_H_
#define _SQLITE3_LOG_H_


extern void debug(const char* fmt, ... )
  __attribute__((format(gnu_printf, 1, 2)));
extern void die(int err, const char* msg, ... )
  __attribute__((format(gnu_printf, 2, 3), noreturn));

#endif // _SQLITE3_LOG_H_
