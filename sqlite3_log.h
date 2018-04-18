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
#ifndef _SQLITE3_LOG_H_
#define _SQLITE3_LOG_H_

extern void debug(const char* fmt, ... )
  __attribute__((format(gnu_printf, 1, 2)));
extern void die(int err, const char* msg, ... )
  __attribute__((format(gnu_printf, 2, 3), noreturn));

#endif // _SQLITE3_LOG_H_
