#ifndef _OS9_TYPES_H
#define _OS9_TYPES_H
#ifdef _MSC_VER
#if _MSV_VER>=1600 
#include <stdint.h>
#else
	typedef __int8              int8_t;
	typedef __int16             int16_t;
	typedef __int32             int32_t;
	typedef __int64             int64_t;
	typedef unsigned __int8     uint8_t;
	typedef unsigned __int16    uint16_t;
	typedef unsigned __int32    uint32_t;
	typedef unsigned __int64    uint64_t;
#endif
typedef uint8_t u_int8;
typedef uint16_t u_int16;
typedef uint32_t u_int32;
typedef int32_t   int32;
typedef int16_t   int16;
typedef uint8_t  u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
typedef int pid_t;

#else
#ifdef __CYGWIN__
#if defined(__int8_t_defined) && 0 
/* Becasue of yet more confusion, this tries to use the C99 standard type from stdint.h from inttypes.h */
/* Even more confusiion as __int8_defined seesm to lie a little */
typedef uint8_t u_int8;
typedef uint16_t u_int16;
typedef uint32_t u_int32;
typedef int32_t   int32;
typedef int16_t   int16;
#else
/* Or uses some others from */
typedef u_int8_t u_int8;
typedef u_int16_t u_int16;
typedef u_int32_t u_int32;
typedef int64_t   int64;
typedef int32_t   int32;
typedef int16_t   int16;
#endif
#else
#if defined(__MINGW32__) | defined(__MINGW64__)
#if defined(__int8_t_defined) && 0 
/* Becasue of yet more confusion, this tries to use the C99 standard type from stdint.h from inttypes.h */
/* Even more confusion as __int8_defined seesm to lie a little */
typedef uint8_t u_int8;
typedef uint16_t u_int16;
typedef uint32_t u_int32;
typedef int32_t   int32;
typedef int16_t   int16;
#else
/* Or uses some others from */
#include <stdint.h>
typedef uint8_t u_int8;
typedef uint16_t u_int16;
typedef uint32_t u_int32;
typedef int32_t   int32;
typedef int16_t   int16;
typedef uint64_t u_int64_t;
#endif
#else
#ifdef linux
#if defined(__int8_t_defined) && 0 
/* Because of yet more confusion, this tries to use the C99 standard type from stdint.h from inttypes.h */
/* Even more confusion as __int8_defined seems to lie a little */
typedef uint8_t u_int8;
typedef uint16_t u_int16;
typedef uint32_t u_int32;
typedef int32_t   int32;
typedef int16_t   int16;
#else
/* Or uses some others from */
typedef u_int8_t u_int8;
typedef u_int16_t u_int16;
typedef u_int32_t u_int32;
typedef int32_t   int32;
typedef int16_t   int16;
typedef int64_t   int64;
typedef u_int64_t u_int64;
#endif
#else
#error "Cannot work out if we are in cygwin, min G win 32 or linux "
#endif
#endif
#endif
#endif

typedef int error_code;
typedef pid_t process_id;
typedef  int  path_id;
typedef int signal_code;

#define SUCCESS 0
#define NO_DEVICE 1
/* #define E_PMEM_BASE 2 */

#ifndef __LINUX__
#define PCI_IO_BASE 0
#define PCI_MEM_BASE 0
#endif

#endif

