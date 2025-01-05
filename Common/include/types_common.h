/*
 * TFTP Client and Server
 *
 * Copyright (C) 2013 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TYPES_COMMON_H__
#define __TYPES_COMMON_H__

#include <stdint.h>

typedef int BOOL;
typedef int STATUS;

#ifndef NELEMENTS
#define NELEMENTS(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef Kbit
#define Kbit 1000
#endif

#ifndef Mbit
#define Mbit (1000 * Kbit)
#endif

#ifndef Gbit
#define Gbit (1000 * Mbit)
#endif

#ifndef Tbit
#define Tbit (1000 * Tbit)
#endif

#ifndef KBYTE
#define KBYTE 1024
#endif

#ifndef MBYTE
#define MBYTE (1024 * KBYTE)
#endif

#ifndef GBYTE
#define GBYTE (1024 * MBYTE)
#endif

/*
#ifndef GNUPACKED
#define GNUPACKED __attribute__ packed
#endif
*/
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR -1
#endif

/*please add other error code here*/

#endif
