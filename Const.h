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
//#define END_OF_FUNCTION(x)  void x##_end() { }
//#define LOCK_VARIABLE(x)    _go32_dpmi_lock_data((void *)&x, sizeof(x))
//#define LOCK_FUNCTION(x)    _go32_dpmi_lock_code(x, (long)x##_end - (long)x)
#define UNLOCK_VARIABLE(x)    unlock_dpmi_data((void *)&x, sizeof(x))
#define UNLOCK_FUNCTION(x)    unlock_dpmi_code(x, (long)x##_end - (long)x)
//#define LOCK_DATA(d,s)        _go32_dpmi_lock_data((d), (s))
//#define LOCK_CODE(c,s)        _go32_dpmi_lock_code((c), (s))
//#define UNLOCK_DATA(d,s)      unlock_dpmi_data((d), (s))
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


#endif //!define INC_CONST_

/*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*/
