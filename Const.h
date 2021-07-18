/****************************************************************************/
 ///////////////////////////GENERAL TYPE DEFINITIONS/////////////////////////
 ////////////////////////////////VERSION 1.13////////////////////////////////
/****************************************************************************/
/*
 ----------------------------------------------------------------------------
|                 Copyright 1998-99-2000 Frederico Jeronimo                  |
 ----------------------------------------------------------------------------
                                                                            */

// NOTES//////////////////////////////////////////////////////////////////////


/* Note: This is a file containing general definitions that I use in most
         of my programs */

// HISTORY////////////////////////////////////////////////////////////////////


/* -----------------03-09-98 18:04-----------------
 V1.00: First working version.
--------------------------------------------------*/

/* -----------------08-12-98 20:36-----------------
 V1.02 : Added the locking defines...
--------------------------------------------------*/

/* -----------------12-08-99 17:00-----------------
 V1.03 : Added the copyright notice.
--------------------------------------------------*/

/* -----------------10-05-00 06:40-----------------
 V1.10 : Added exit and return codes. Added several
 new locking defines (LOCK_DATA, UNLOCK_DATA, LOCK_CODE,
 UNLOCK_CODE, UNLOCK_VARIABLE, UNLOCK_FUNCTION).
--------------------------------------------------*/

/* -----------------23-11-00 18:46-----------------
 V1.12 : Added the handler install defines.
--------------------------------------------------*/

/* -----------------27-11-00 15:32-----------------
 V1.13 : Added the byte sizes
--------------------------------------------------*/

// INCLUDE PROTOCOL///////////////////////////////////////////////////////////


#if !defined INC_CONST_
    #define INC_CONST_

// TYPEDEFS///////////////////////////////////////////////////////////////////


/* New data type names (Not that useful... perhaps I'll get rid of them ) */
typedef char              INT8;
typedef unsigned char     UINT8;
typedef int               INT16;
typedef unsigned int      UINT16;
typedef long              INT32;
typedef unsigned long     UINT32;

typedef unsigned int      WORD;
typedef unsigned long     LONG;

typedef int               BOOL;

// DEFINES////////////////////////////////////////////////////////////////////


/* Boolean constants */
//#define TRUE (1)
//#define FALSE (0)

/* Handler install flags */
#define TRAD 0
#define DPMI 0
#define AS   1
#define NASM 0

/* Exit codes */
#define EXIT_SUCCESS 0
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/* Return codes */
#define RET_SUCCESS 0
#define RET_FAILURE -1

/* Protection of variables and functions */
#ifndef END_OF_FUNCTION(x)
	#define END_OF_FUNCTION(x)  void x##_end() { }
#endif

#ifndef LOCK_VARIABLE(x)
	#define LOCK_VARIABLE(x)    _go32_dpmi_lock_data((void *)&x, sizeof(x))
#endif

#ifndef LOCK_FUNCTION(x)
	#define LOCK_FUNCTION(x)    _go32_dpmi_lock_code(x, (long)x##_end - (long)x)
#endif

#define UNLOCK_VARIABLE(x)    unlock_dpmi_data((void *)&x, sizeof(x))
#define UNLOCK_FUNCTION(x)    unlock_dpmi_code(x, (long)x##_end - (long)x)

#ifndef LOCK_DATA(d,s)
	#define LOCK_DATA(d,s)        _go32_dpmi_lock_data((d), (s))
#endif

#ifndef LOCK_CODE(c,s)
	#define LOCK_CODE(c,s)        _go32_dpmi_lock_code((c), (s))
#endif

#ifndef UNLOCK_DATA(d,s)
	#define UNLOCK_DATA(d,s)      unlock_dpmi_data((d), (s))
#endif

#define UNLOCK_CODE(c,s)      unlock_dpmi_code((c), (s))

/* Number of elements of a given structure */
#define DIM(x) (sizeof(x)/sizeof(x[0]))

/* Byte access */
#define LOWBYTE(x) ((x) & 0xFF)
#define HIGHBYTE(x) (((x) >> 8) & 0xFF)

/* Number of bytes */
#define BYTE 1
#define WORD 2
#define DWORD 4

/* Ignore case-sensitivity... */
//#define MIN(a,b) min(a,b)
//#define MAX(a,b) max(a,b)

/* Interrupt acknowledgement stuff */
#define PIC               (0x20)
#define NONSPECIFIC_EOI   (0x20)

// GLOBAL VARIABLES///////////////////////////////////////////////////////////


/* Test : Some yet to be refined geometrical stuff... */
typedef struct {
	int Top;
	int Left;
	int Bottom;
	int Right;
} RECT;

//END PROTOCOL////////////////////////////////////////////////////////////////

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
    PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
    PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
    PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
    PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */



#endif //!define INC_CONST_

/*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*/
