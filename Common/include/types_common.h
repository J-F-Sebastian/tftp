/* Copyright 2007 Diego Gallizioli
 * Source file name:
 *
 * Description:
 *
 *
 *
 *
 * Notes:
 *
 *
 *
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
