/****************************************************************************/
 //////SYMBOLIC NAME DEFINITIONS FOR EACH OF THE KEYBOARD SCANCODES AND//////
 //////////DECLARATION OF EACH OF THE PUBLIC FUNCTIONS IN KEYBOARD.C/////////
 ///////////////////////////////VERSION 1.09/////////////////////////////////
/****************************************************************************/
/*
 ----------------------------------------------------------------------------
|                Copyright 1998-99-2000 Frederico Jeronimo                   |
 ----------------------------------------------------------------------------
                                                                            */

// HISTORY////////////////////////////////////////////////////////////////////


/* -----------------08-09-98 17:07-----------------
 V1.00 : First working version.
--------------------------------------------------*/

/* -----------------25-12-99 13:11-----------------
 V1.01 : Added the copyright notice.
--------------------------------------------------*/

/* -----------------18-03-00 07:11PM---------------
 V1.06 : Moved all global data and defines from the
 C file to here where it belongs.
 -------------------------------------------------*/

/* -----------------18-03-00 08:33PM---------------
 V1.07 : 'KeyStateArray' and 'PressedArray' are no
 longer static as they might be needed by NASM
 keyboard handler.
 -------------------------------------------------*/

/* -----------------23-11-00 19:27-----------------
 V1.08 : Added the prototype for KeyInthandler_end(),
 used in all locking operations.
--------------------------------------------------*/

/* -----------------27-11-00 3:55------------------
 V1.09 : Changed the prototypes for the functions
 originally returning void and now returning int
 due to improved error checking.
--------------------------------------------------*/

// INCLUDE PROTOCOL///////////////////////////////////////////////////////////


#if !defined INC_KEYBOARD_
    #define INC_KEYBOARD_

// INCLUDES///////////////////////////////////////////////////////////////////


#include <dpmi.h>
#include <go32.h>
#include "Const.h"

// DEFINES////////////////////////////////////////////////////////////////////


/* Some useful constants */
#define MAX_SCANCODES         (128)
#define NEW_HANDLER           (1)
#define OLD_HANDLER           (0)
#define KEYBOARD_INPUT        (0x60)
#define KEYBOARD_XT_CLEAR     (0x61)

/* Symbolic name definitions of each of the scancodes */

//undef all to remove conflicts with allegro
#undef KEY_A
#undef KEY_B
#undef KEY_C
#undef KEY_D
#undef KEY_E
#undef KEY_F
#undef KEY_G
#undef KEY_H
#undef KEY_I
#undef KEY_J
#undef KEY_K
#undef KEY_L
#undef KEY_M
#undef KEY_N
#undef KEY_O
#undef KEY_P
#undef KEY_Q
#undef KEY_R
#undef KEY_S
#undef KEY_T
#undef KEY_U
#undef KEY_V
#undef KEY_W
#undef KEY_X
#undef KEY_Y
#undef KEY_Z
#undef KEY_1
#undef KEY_2
#undef KEY_3
#undef KEY_4
#undef KEY_5
#undef KEY_6
#undef KEY_7
#undef KEY_8
#undef KEY_9
#undef KEY_0
#undef KEY_DASH
#undef KEY_EQUAL
#undef KEY_LBRACKET
#undef KEY_RBRACKET
#undef KEY_SEMICOLON
#undef KEY_RQUOTE
#undef KEY_LQUOTE
#undef KEY_PERIOD
#undef KEY_COMMA
#undef KEY_SLASH
#undef KEY_BACKSLASH
#undef KEY_F1
#undef KEY_F2
#undef KEY_F3
#undef KEY_F4
#undef KEY_F5
#undef KEY_F6
#undef KEY_F7
#undef KEY_F8
#undef KEY_F9
#undef KEY_F10
#undef KEY_ESC
#undef KEY_BACKSPACE
#undef KEY_TAB
#undef KEY_ENTER
#undef KEY_CONTROL
#undef KEY_LSHIFT
#undef KEY_RSHIFT
#undef KEY_PRTSCR
#undef KEY_ALT
#undef KEY_SPACE
#undef KEY_CAPSLOCK
#undef KEY_NUMLOCK
#undef KEY_SCROLLLOCK
#undef KEY_HOME
#undef KEY_UP
#undef KEY_PGUP
#undef KEY_MINUS
#undef KEY_LEFT
#undef KEY_CENTER
#undef KEY_RIGHT
#undef KEY_PLUS
#undef KEY_END
#undef KEY_DOWN
#undef KEY_PGDWN
#undef KEY_INS
#undef KEY_DEL


#define KEY_A                (0x1E)
#define KEY_B                (0x30)
#define KEY_C                (0x2E)
#define KEY_D                (0x20)
#define KEY_E                (0x12)
#define KEY_F                (0x21)
#define KEY_G                (0x22)
#define KEY_H                (0x23)
#define KEY_I                (0x17)
#define KEY_J                (0x24)
#define KEY_K                (0x25)
#define KEY_L                (0x26)
#define KEY_M                (0x32)
#define KEY_N                (0x31)
#define KEY_O                (0x18)
#define KEY_P                (0x19)
#define KEY_Q                (0x10)
#define KEY_R                (0x13)
#define KEY_S                (0x1F)
#define KEY_T                (0x14)
#define KEY_U                (0x16)
#define KEY_V                (0x2F)
#define KEY_W                (0x11)
#define KEY_X                (0x2D)
#define KEY_Y                (0x15)
#define KEY_Z                (0x2C)
#define KEY_1                (0x02)
#define KEY_2                (0x03)
#define KEY_3                (0x04)
#define KEY_4                (0x05)
#define KEY_5                (0x06)
#define KEY_6                (0x07)
#define KEY_7                (0x08)
#define KEY_8                (0x09)
#define KEY_9                (0x0A)
#define KEY_0                (0x0B)
#define KEY_DASH             (0x0C)  //     -_
#define KEY_EQUAL            (0x0D)  //     =+
#define KEY_LBRACKET         (0x1A)  //     [{
#define KEY_RBRACKET         (0x1B)  //     ]}
#define KEY_SEMICOLON        (0x27)  //     ;:
#define KEY_RQUOTE           (0x28)  //     '"
#define KEY_LQUOTE           (0x29)  //     `~
#define KEY_PERIOD           (0x33)  //     .>
#define KEY_COMMA            (0x34)  //     ,<
#define KEY_SLASH            (0x35)  //     /?
#define KEY_BACKSLASH        (0x2B)  //     \|
#define KEY_F1               (0x3B)
#define KEY_F2               (0x3C)
#define KEY_F3               (0x3D)
#define KEY_F4               (0x3E)
#define KEY_F5               (0x3F)
#define KEY_F6               (0x40)
#define KEY_F7               (0x41)
#define KEY_F8               (0x42)
#define KEY_F9               (0x43)
#define KEY_F10              (0x44)
#define KEY_ESC              (0x01)
#define KEY_BACKSPACE        (0x0E)
#define KEY_TAB              (0x0F)
#define KEY_ENTER            (0x1C)
#define KEY_CONTROL          (0x1D)
#define KEY_LSHIFT           (0x2A)
#define KEY_RSHIFT           (0x36)
#define KEY_PRTSCR           (0x37)
#define KEY_ALT              (0x38)
#define KEY_SPACE            (0x39)
#define KEY_CAPSLOCK         (0x3A)
#define KEY_NUMLOCK          (0x45)
#define KEY_SCROLLLOCK       (0x46)
#define KEY_HOME             (0x47)
#define KEY_UP               (0x48)
#define KEY_PGUP             (0x49)
#define KEY_MINUS            (0x4A)
#define KEY_LEFT             (0x4B)
#define KEY_CENTER           (0x4C)
#define KEY_RIGHT            (0x4D)
#define KEY_PLUS             (0x4E)
#define KEY_END              (0x4F)
#define KEY_DOWN             (0x50)
#define KEY_PGDWN            (0x51)
#define KEY_INS              (0x52)
#define KEY_DEL              (0x53)

//GLOBAL VARIABLES////////////////////////////////////////////////////////////


/* The structures that will hold the old and new keyboard handlers */
_go32_dpmi_seginfo old_key_handler,new_key_handler;

extern int KeyboardStateFlag;           /* Current keybd handler */
UINT8 KeyStateArray[MAX_SCANCODES];     /* Current key state */
UINT8 PressedArray[MAX_SCANCODES];      /* Flags set if key hit */
int KeyUpBuffer[255];
int KeyUpBufferCount = 0;

//FUNCTION PROTOTYPES/////////////////////////////////////////////////////////


int KeyIntHandler(void);
void KeyIntHandler_end();
int SetKb(void);
int ResetKb(void);
int KeyState(int scancode);
int TrueKeyState(int scancode);
int KeyUpWaiting();
int GetNextKeyUpCode();

//END PROTOCOL////////////////////////////////////////////////////////////////


#endif //!define INC_KEYBOARD_

/*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*/
