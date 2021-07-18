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
#define MAX_SCANCODES         (256)
#define NEW_HANDLER           (1)
#define OLD_HANDLER           (0)
#define KEYBOARD_INPUT        (0x60)
#define KEYBOARD_XT_CLEAR     (0x61)

/* Symbolic name definitions of each of the scancodes */

//undef all to remove conflicts with allegro
#undef KEY_NONE
#undef KEY_ESC
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
#undef KEY_MINUS
#undef KEY_EQUALS
#undef KEY_BACKSPACE
#undef KEY_TAB
#undef KEY_Q
#undef KEY_W
#undef KEY_E
#undef KEY_R
#undef KEY_T
#undef KEY_Y
#undef KEY_U
#undef KEY_I
#undef KEY_O
#undef KEY_P
#undef KEY_LEFT_BRACE
#undef KEY_RIGHT_BRACE
#undef KEY_ENTER
#undef KEY_LEFT_CONTROL
#undef KEY_A
#undef KEY_S
#undef KEY_D
#undef KEY_F
#undef KEY_G
#undef KEY_H
#undef KEY_J
#undef KEY_K
#undef KEY_L
#undef KEY_SEMICOLON
#undef KEY_APOSTROPHE
#undef KEY_ACCENT
#undef KEY_TILDE
#undef KEY_LEFT_SHIFT
#undef KEY_BACK_SLASH
#undef KEY_Z
#undef KEY_X
#undef KEY_C
#undef KEY_V
#undef KEY_B
#undef KEY_N
#undef KEY_M
#undef KEY_COMMA
#undef KEY_PERIOD
#undef KEY_FORWARD_SLASH
#undef KEY_RIGHT_SHIFT
#undef KEY_KP_STAR
#undef KEY_KP_MULTIPLY
#undef KEY_LEFT_ALT
#undef KEY_SPACE
#undef KEY_CAPS_LOCK
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
#undef KEY_NUM_LOCK
#undef KEY_SCROLL_LOCK
#undef KEY_KP_7
#undef KEY_KP_8
#undef KEY_KP_9
#undef KEY_KP_MINUS
#undef KEY_KP_4
#undef KEY_KP_5
#undef KEY_KP_6
#undef KEY_KP_PLUS
#undef KEY_KP_1
#undef KEY_KP_2
#undef KEY_KP_3
#undef KEY_KP_0
#undef KEY_KP_PERIOD

// Extended keys for the IBM 101-key Model M keyboard.
#undef KEY_RIGHT_ALT
#undef KEY_RIGHT_CONTROL
#undef KEY_LEFT_ARROW
#undef KEY_RIGHT_ARROW
#undef KEY_UP_ARROW
#undef KEY_DOWN_ARROW
#undef KEY_NUMPAD_ENTER
#undef KEY_INSERT
#undef KEY_DELETE
#undef KEY_HOME
#undef KEY_END
#undef KEY_PAGE_UP
#undef KEY_PAGE_DOWN
#undef KEY_KP_FORWARD_SLASH
#undef KEY_PRINT_SCREEN



// Original 83 Keys from the IBM 83-key Model F keyboard
#define KEY_NONE              0x00
#define KEY_ESC               0x01
#define KEY_1                 0x02
#define KEY_2                 0x03
#define KEY_3                 0x04
#define KEY_4                 0x05
#define KEY_5                 0x06
#define KEY_6                 0x07
#define KEY_7                 0x08
#define KEY_8                 0x09
#define KEY_9                 0x0A
#define KEY_0                 0x0B
#define KEY_MINUS             0x0C
#define KEY_EQUALS            0x0D
#define KEY_BACKSPACE         0x0E
#define KEY_TAB               0x0F
#define KEY_Q                 0x10
#define KEY_W                 0x11
#define KEY_E                 0x12
#define KEY_R                 0x13
#define KEY_T                 0x14
#define KEY_Y                 0x15
#define KEY_U                 0x16
#define KEY_I                 0x17
#define KEY_O                 0x18
#define KEY_P                 0x19
#define KEY_LEFT_BRACE        0x1A
#define KEY_RIGHT_BRACE       0x1B
#define KEY_ENTER             0x1C
#define KEY_LEFT_CONTROL      0x1D
#define KEY_A                 0x1E
#define KEY_S                 0x1F
#define KEY_D                 0x20
#define KEY_F                 0x21
#define KEY_G                 0x22
#define KEY_H                 0x23
#define KEY_J                 0x24
#define KEY_K                 0x25
#define KEY_L                 0x26
#define KEY_SEMICOLON         0x27
#define KEY_APOSTROPHE        0x28
#define KEY_ACCENT            0x29
#define KEY_TILDE             0x29 // Duplicate of KEY_ACCENT with popular Tilde name.
#define KEY_LEFT_SHIFT        0x2A
#define KEY_BACK_SLASH        0x2B
#define KEY_Z                 0x2C
#define KEY_X                 0x2D
#define KEY_C                 0x2E
#define KEY_V                 0x2F
#define KEY_B                 0x30
#define KEY_N                 0x31
#define KEY_M                 0x32
#define KEY_COMMA             0x33
#define KEY_PERIOD            0x34
#define KEY_FORWARD_SLASH     0x35
#define KEY_RIGHT_SHIFT       0x36
#define KEY_KP_STAR           0x37
#define KEY_KP_MULTIPLY       0x37 // Duplicate of KEY_KP_STAR
#define KEY_LEFT_ALT          0x38
#define KEY_SPACE             0x39
#define KEY_CAPS_LOCK         0x3A
#define KEY_F1                0x3B
#define KEY_F2                0x3C
#define KEY_F3                0x3D
#define KEY_F4                0x3E
#define KEY_F5                0x3F
#define KEY_F6                0x40
#define KEY_F7                0x41
#define KEY_F8                0x42
#define KEY_F9                0x43
#define KEY_F10               0x44
#define KEY_NUM_LOCK          0x45
#define KEY_SCROLL_LOCK       0x46
#define KEY_KP_7              0x47
#define KEY_KP_8              0x48
#define KEY_KP_9              0x49
#define KEY_KP_MINUS          0x4A
#define KEY_KP_4              0x4B
#define KEY_KP_5              0x4C
#define KEY_KP_6              0x4D
#define KEY_KP_PLUS           0x4E
#define KEY_KP_1              0x4F
#define KEY_KP_2              0x50
#define KEY_KP_3              0x51
#define KEY_KP_0              0x52
#define KEY_KP_PERIOD         0x53

// Extended keys for the IBM 101-key Model M keyboard.
#define KEY_RIGHT_ALT         0xE038
#define KEY_RIGHT_CONTROL     0xE01D
#define KEY_LEFT_ARROW        0xE04B
#define KEY_RIGHT_ARROW       0xE04D
#define KEY_UP_ARROW          0xE048
#define KEY_DOWN_ARROW        0xE050
#define KEY_NUMPAD_ENTER      0xE01C
#define KEY_INSERT            0xE052
#define KEY_DELETE            0xE053
#define KEY_HOME              0xE047
#define KEY_END               0xE04F
#define KEY_PAGE_UP           0xE049
#define KEY_PAGE_DOWN         0xE051
#define KEY_KP_FORWARD_SLASH  0xE035
#define KEY_PRINT_SCREEN      0xE02AE037

// SHIFT STATE BIT MASKS

#define SHIFTSTATE_SHIFT_RIGHT          0x0001 //right shift
#define SHIFTSTATE_SHIFT_LEFT           0x0002 //left shift
#define SHIFTSTATE_CTRL                 0x0004 //control key
#define SHIFTSTATE_ALT                  0x0008 //alt key
#define SHIFTSTATE_SCROLL_LOCK_ON       0x0010 //scroll lock is on
#define SHIFTSTATE_NUM_LOCK_ON          0x0020 //num lock is on
#define SHIFTSTATE_CAPS_LOCK_ON         0x0040 //caps lock is on
#define SHIFTSTATE_INSERT_MODE          0x0080 //insert or overlay mode
#define SHIFTSTATE_CTRL_LEFT            0x0100 //the left control key is pressed
#define SHIFTSTATE_ALT_LEFT             0x0200 //the left alt key is pressed
#define SHIFTSTATE_CTRL_RIGHT           0x0400 //the right control key is pressed
#define SHIFTSTATE_ALT_RIGHT            0x0800 //the right alt key is pressed.
#define SHIFTSTATE_SCROLL_LOCK_DOWN     0x1000 //the scroll lock key is pressed
#define SHIFTSTATE_NUM_LOCK_DOWN        0x2000 //the numeric lock key is pressed
#define SHIFTSTATE_CAPS_LOCK_DOWN       0x4000 //the capitals lock key is pressed
#define SHIFTSTATE_SYS_REQ_DOWN         0x8000 //the system request key is pressed


//ASCII CODES

#define ASCII_BACKSPACE 0x08
#define ASCII_TAB 0x09
#define ASCII_LINE_FEED 0x0A
#define ASCII_CARRIAGE_RETURN 0x0D
#define ASCII_ESCAPE 0x1B
#define ASCII_SPACE 0x20
#define ASCII_EXCLAMATION 0x21
#define ASCII_DOUBLE_QUOTE 0x22
#define ASCII_POUND 0x23
#define ASCII_DOLLAR 0x24
#define ASCII_PERCENT 0x25
#define ASCII_AMPERSAND 0x26
#define ASCII_APOSTROPHE 0x27
#define ASCII_LEFT_PAREN 0x28
#define ASCII_RIGHT_PAREN 0x29
#define ASCII_ASTERISK 0x2A
#define ASCII_PLUS 0x2B
#define ASCII_COMMA 0x2C
#define ASCII_DASH 0x2D
#define ASCII_PERIOD 0x2E
#define ASCII_FORWARD_SLASH 0x2F
#define ASCII_0 0x30
#define ASCII_1 0x31
#define ASCII_2 0x32
#define ASCII_3 0x33
#define ASCII_4 0x34
#define ASCII_5 0x35
#define ASCII_6 0x36
#define ASCII_7 0x37
#define ASCII_8 0x38
#define ASCII_9 0x39
#define ASCII_COLON 0x3A
#define ASCII_SEMICOLON 0x3B
#define ASCII_LESS_THAN 0x3C
#define ASCII_EQUALS 0x3D
#define ASCII_GREATER_THAN 0x3E
#define ASCII_QUESTION 0x3F
#define ASCII_AT 0x40
#define ASCII_A_UPPER 0x41
#define ASCII_B_UPPER 0x42
#define ASCII_C_UPPER 0x43
#define ASCII_D_UPPER 0x44
#define ASCII_E_UPPER 0x45
#define ASCII_F_UPPER 0x46
#define ASCII_G_UPPER 0x47
#define ASCII_H_UPPER 0x48
#define ASCII_I_UPPER 0x49
#define ASCII_J_UPPER 0x4A
#define ASCII_K_UPPER 0x4B
#define ASCII_L_UPPER 0x4C
#define ASCII_M_UPPER 0x4D
#define ASCII_N_UPPER 0x4E
#define ASCII_O_UPPER 0x4F
#define ASCII_P_UPPER 0x50
#define ASCII_Q_UPPER 0x51
#define ASCII_R_UPPER 0x52
#define ASCII_S_UPPER 0x53
#define ASCII_T_UPPER 0x54
#define ASCII_U_UPPER 0x55
#define ASCII_V_UPPER 0x56
#define ASCII_W_UPPER 0x57
#define ASCII_X_UPPER 0x58
#define ASCII_Y_UPPER 0x59
#define ASCII_Z_UPPER 0x5A
#define ASCII_LEFT_SQUARE_BRACKET 0x5B
#define ASCII_BACK_SLASH 0x5C
#define ASCII_RIGHT_SQUARE_BRACKET 0x5D
#define ASCII_CARET 0x5E
#define ASCII_UNDERSCORE 0x5F
#define ASCII_BACKTICK 0x60
#define ASCII_A_LOWER 0x61
#define ASCII_B_LOWER 0x62
#define ASCII_C_LOWER 0x63
#define ASCII_D_LOWER 0x64
#define ASCII_E_LOWER 0x65
#define ASCII_F_LOWER 0x66
#define ASCII_G_LOWER 0x67
#define ASCII_H_LOWER 0x68
#define ASCII_I_LOWER 0x69
#define ASCII_J_LOWER 0x6A
#define ASCII_K_LOWER 0x6B
#define ASCII_L_LOWER 0x6C
#define ASCII_M_LOWER 0x6D
#define ASCII_N_LOWER 0x6E
#define ASCII_O_LOWER 0x6F
#define ASCII_P_LOWER 0x70
#define ASCII_Q_LOWER 0x71
#define ASCII_R_LOWER 0x72
#define ASCII_S_LOWER 0x73
#define ASCII_T_LOWER 0x74
#define ASCII_U_LOWER 0x75
#define ASCII_V_LOWER 0x76
#define ASCII_W_LOWER 0x77
#define ASCII_X_LOWER 0x78
#define ASCII_Y_LOWER 0x79
#define ASCII_Z_LOWER 0x7A
#define ASCII_LEFT_BRACE 0x7B
#define ASCII_PIPE 0x7C
#define ASCII_RIGHT_BRACE 0x7D
#define ASCII_TILDE 0x7E
#define ASCII_DELETE 0x7F


//GLOBAL VARIABLES////////////////////////////////////////////////////////////

UINT16 CapsAsciiMap[255] = {0x0};
UINT16 StdAsciiMap[255] = {0x0};
UINT16 NumLockAsciiMap[255] = {0x0};
UINT16 ShiftAsciiMap[255] = {0x0};
int AsciiMapsPopulated = 0;

void PopulateAsciiMaps(){
    if (AsciiMapsPopulated){
        return;
    }
    // CAPS - non-extended scan codes
    CapsAsciiMap[KEY_ESC] = ASCII_ESCAPE;
    CapsAsciiMap[KEY_1] = ASCII_1;
    CapsAsciiMap[KEY_2] = ASCII_2;
    CapsAsciiMap[KEY_3] = ASCII_3;
    CapsAsciiMap[KEY_4] = ASCII_4;
    CapsAsciiMap[KEY_5] = ASCII_5;
    CapsAsciiMap[KEY_6] = ASCII_6;
    CapsAsciiMap[KEY_7] = ASCII_7;
    CapsAsciiMap[KEY_8] = ASCII_8;
    CapsAsciiMap[KEY_9] = ASCII_9;
    CapsAsciiMap[KEY_0] = ASCII_0;
    CapsAsciiMap[KEY_MINUS] = ASCII_DASH;
    CapsAsciiMap[KEY_EQUALS] = ASCII_EQUALS;
    CapsAsciiMap[KEY_BACKSPACE] = ASCII_BACKSPACE;
    CapsAsciiMap[KEY_TAB] = ASCII_TAB;
    CapsAsciiMap[KEY_Q] = ASCII_Q_UPPER;
    CapsAsciiMap[KEY_W] = ASCII_W_UPPER;
    CapsAsciiMap[KEY_E] = ASCII_E_UPPER;
    CapsAsciiMap[KEY_R] = ASCII_R_UPPER;
    CapsAsciiMap[KEY_T] = ASCII_T_UPPER;
    CapsAsciiMap[KEY_Y] = ASCII_Y_UPPER;
    CapsAsciiMap[KEY_U] = ASCII_U_UPPER;
    CapsAsciiMap[KEY_I] = ASCII_I_UPPER;
    CapsAsciiMap[KEY_O] = ASCII_O_UPPER;
    CapsAsciiMap[KEY_P] = ASCII_P_UPPER;
    CapsAsciiMap[KEY_LEFT_BRACE] = ASCII_LEFT_SQUARE_BRACKET;
    CapsAsciiMap[KEY_RIGHT_BRACE] = ASCII_RIGHT_SQUARE_BRACKET;
    CapsAsciiMap[KEY_ENTER] = ASCII_LINE_FEED;
    CapsAsciiMap[KEY_LEFT_CONTROL] = 0x0;
    CapsAsciiMap[KEY_A] = ASCII_A_UPPER;
    CapsAsciiMap[KEY_S] = ASCII_S_UPPER;
    CapsAsciiMap[KEY_D] = ASCII_D_UPPER;
    CapsAsciiMap[KEY_F] = ASCII_F_UPPER;
    CapsAsciiMap[KEY_G] = ASCII_G_UPPER;
    CapsAsciiMap[KEY_H] = ASCII_H_UPPER;
    CapsAsciiMap[KEY_J] = ASCII_J_UPPER;
    CapsAsciiMap[KEY_K] = ASCII_K_UPPER;
    CapsAsciiMap[KEY_L] = ASCII_L_UPPER;
    CapsAsciiMap[KEY_SEMICOLON] = ASCII_SEMICOLON;
    CapsAsciiMap[KEY_APOSTROPHE] = ASCII_APOSTROPHE;
    CapsAsciiMap[KEY_TILDE] = ASCII_BACKTICK;
    CapsAsciiMap[KEY_LEFT_SHIFT] = 0x0;
    CapsAsciiMap[KEY_BACK_SLASH] = ASCII_BACK_SLASH;
    CapsAsciiMap[KEY_Z] = ASCII_Z_UPPER;
    CapsAsciiMap[KEY_X] = ASCII_X_UPPER;
    CapsAsciiMap[KEY_C] = ASCII_C_UPPER;
    CapsAsciiMap[KEY_V] = ASCII_V_UPPER;
    CapsAsciiMap[KEY_B] = ASCII_B_UPPER;
    CapsAsciiMap[KEY_N] = ASCII_N_UPPER;
    CapsAsciiMap[KEY_M] = ASCII_M_UPPER;
    CapsAsciiMap[KEY_COMMA] = ASCII_COMMA;
    CapsAsciiMap[KEY_PERIOD] = ASCII_PERIOD;
    CapsAsciiMap[KEY_FORWARD_SLASH] = ASCII_FORWARD_SLASH;
    CapsAsciiMap[KEY_RIGHT_SHIFT] = 0x0;
    CapsAsciiMap[KEY_KP_STAR] = ASCII_ASTERISK;
    CapsAsciiMap[KEY_LEFT_ALT] = 0x0;
    CapsAsciiMap[KEY_SPACE] = ASCII_SPACE;
    CapsAsciiMap[KEY_CAPS_LOCK] = 0x0;
    CapsAsciiMap[KEY_F1] = 0x0;
    CapsAsciiMap[KEY_F2] = 0x0;
    CapsAsciiMap[KEY_F3] = 0x0;
    CapsAsciiMap[KEY_F4] = 0x0;
    CapsAsciiMap[KEY_F5] = 0x0;
    CapsAsciiMap[KEY_F6] = 0x0;
    CapsAsciiMap[KEY_F7] = 0x0;
    CapsAsciiMap[KEY_F8] = 0x0;
    CapsAsciiMap[KEY_F9] = 0x0;
    CapsAsciiMap[KEY_F10] = 0x0;
    CapsAsciiMap[KEY_NUM_LOCK] = 0x0;
    CapsAsciiMap[KEY_SCROLL_LOCK] = 0x0;
    CapsAsciiMap[KEY_KP_7] = 0x0;
    CapsAsciiMap[KEY_KP_8] = 0x0;
    CapsAsciiMap[KEY_KP_9] = 0x0;
    CapsAsciiMap[KEY_KP_MINUS] = ASCII_DASH;
    CapsAsciiMap[KEY_KP_4] = 0x0;
    CapsAsciiMap[KEY_KP_5] = 0x0;
    CapsAsciiMap[KEY_KP_6] = 0x0;
    CapsAsciiMap[KEY_KP_PLUS] = ASCII_PLUS;
    CapsAsciiMap[KEY_KP_1] = 0x0;
    CapsAsciiMap[KEY_KP_2] = 0x0;
    CapsAsciiMap[KEY_KP_3] = 0x0;
    CapsAsciiMap[KEY_KP_0] = 0x0;
    CapsAsciiMap[KEY_KP_PERIOD] = 0x0;

    // CAPS - extended scan codes
    CapsAsciiMap[(KEY_RIGHT_ALT & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_RIGHT_CONTROL & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_LEFT_ARROW & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_RIGHT_ARROW & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_UP_ARROW & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_DOWN_ARROW & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_NUMPAD_ENTER & 0xFF) + 0x7F] = ASCII_LINE_FEED;
    CapsAsciiMap[(KEY_INSERT & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_DELETE & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_HOME & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_END & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_PAGE_UP & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_PAGE_DOWN & 0xFF) + 0x7F] = 0x0;
    CapsAsciiMap[(KEY_KP_FORWARD_SLASH & 0xFF) + 0x7F] = ASCII_FORWARD_SLASH;
    CapsAsciiMap[(KEY_PRINT_SCREEN & 0xFF) + 0x7F] = 0x0;

    // SHIFT - non-extended scan codes
    ShiftAsciiMap[KEY_ESC] = ASCII_ESCAPE;
    ShiftAsciiMap[KEY_1] = ASCII_EXCLAMATION;
    ShiftAsciiMap[KEY_2] = ASCII_AT;
    ShiftAsciiMap[KEY_3] = ASCII_POUND;
    ShiftAsciiMap[KEY_4] = ASCII_DOLLAR;
    ShiftAsciiMap[KEY_5] = ASCII_PERCENT;
    ShiftAsciiMap[KEY_6] = ASCII_CARET;
    ShiftAsciiMap[KEY_7] = ASCII_AMPERSAND;
    ShiftAsciiMap[KEY_8] = ASCII_ASTERISK;
    ShiftAsciiMap[KEY_9] = ASCII_LEFT_PAREN;
    ShiftAsciiMap[KEY_0] = ASCII_RIGHT_PAREN;
    ShiftAsciiMap[KEY_MINUS] = ASCII_UNDERSCORE;
    ShiftAsciiMap[KEY_EQUALS] = ASCII_PLUS;
    ShiftAsciiMap[KEY_BACKSPACE] = ASCII_BACKSPACE;
    ShiftAsciiMap[KEY_TAB] = ASCII_TAB;
    ShiftAsciiMap[KEY_Q] = ASCII_Q_UPPER;
    ShiftAsciiMap[KEY_W] = ASCII_W_UPPER;
    ShiftAsciiMap[KEY_E] = ASCII_E_UPPER;
    ShiftAsciiMap[KEY_R] = ASCII_R_UPPER;
    ShiftAsciiMap[KEY_T] = ASCII_T_UPPER;
    ShiftAsciiMap[KEY_Y] = ASCII_Y_UPPER;
    ShiftAsciiMap[KEY_U] = ASCII_U_UPPER;
    ShiftAsciiMap[KEY_I] = ASCII_I_UPPER;
    ShiftAsciiMap[KEY_O] = ASCII_O_UPPER;
    ShiftAsciiMap[KEY_P] = ASCII_P_UPPER;
    ShiftAsciiMap[KEY_LEFT_BRACE] = ASCII_LEFT_BRACE;
    ShiftAsciiMap[KEY_RIGHT_BRACE] = ASCII_RIGHT_BRACE;
    ShiftAsciiMap[KEY_ENTER] = ASCII_LINE_FEED;
    ShiftAsciiMap[KEY_LEFT_CONTROL] = 0x0;
    ShiftAsciiMap[KEY_A] = ASCII_A_UPPER;
    ShiftAsciiMap[KEY_S] = ASCII_S_UPPER;
    ShiftAsciiMap[KEY_D] = ASCII_D_UPPER;
    ShiftAsciiMap[KEY_F] = ASCII_F_UPPER;
    ShiftAsciiMap[KEY_G] = ASCII_G_UPPER;
    ShiftAsciiMap[KEY_H] = ASCII_H_UPPER;
    ShiftAsciiMap[KEY_J] = ASCII_J_UPPER;
    ShiftAsciiMap[KEY_K] = ASCII_K_UPPER;
    ShiftAsciiMap[KEY_L] = ASCII_L_UPPER;
    ShiftAsciiMap[KEY_SEMICOLON] = ASCII_COLON;
    ShiftAsciiMap[KEY_APOSTROPHE] = ASCII_DOUBLE_QUOTE;
    ShiftAsciiMap[KEY_ACCENT] = ASCII_TILDE;
    ShiftAsciiMap[KEY_LEFT_SHIFT] = 0x0;
    ShiftAsciiMap[KEY_BACK_SLASH] = ASCII_PIPE;
    ShiftAsciiMap[KEY_Z] = ASCII_Z_UPPER;
    ShiftAsciiMap[KEY_X] = ASCII_X_UPPER;
    ShiftAsciiMap[KEY_C] = ASCII_C_UPPER;
    ShiftAsciiMap[KEY_V] = ASCII_V_UPPER;
    ShiftAsciiMap[KEY_B] = ASCII_B_UPPER;
    ShiftAsciiMap[KEY_N] = ASCII_N_UPPER;
    ShiftAsciiMap[KEY_M] = ASCII_M_UPPER;
    ShiftAsciiMap[KEY_COMMA] = ASCII_LESS_THAN;
    ShiftAsciiMap[KEY_PERIOD] = ASCII_GREATER_THAN;
    ShiftAsciiMap[KEY_FORWARD_SLASH] = ASCII_QUESTION;
    ShiftAsciiMap[KEY_RIGHT_SHIFT] = 0x0;
    ShiftAsciiMap[KEY_KP_STAR] = ASCII_ASTERISK;
    ShiftAsciiMap[KEY_LEFT_ALT] = 0x0;
    ShiftAsciiMap[KEY_SPACE] = ASCII_SPACE;
    ShiftAsciiMap[KEY_CAPS_LOCK] = 0x0;
    ShiftAsciiMap[KEY_F1] = 0x0;
    ShiftAsciiMap[KEY_F2] = 0x0;
    ShiftAsciiMap[KEY_F3] = 0x0;
    ShiftAsciiMap[KEY_F4] = 0x0;
    ShiftAsciiMap[KEY_F5] = 0x0;
    ShiftAsciiMap[KEY_F6] = 0x0;
    ShiftAsciiMap[KEY_F7] = 0x0;
    ShiftAsciiMap[KEY_F8] = 0x0;
    ShiftAsciiMap[KEY_F9] = 0x0;
    ShiftAsciiMap[KEY_F10] = 0x0;
    ShiftAsciiMap[KEY_NUM_LOCK] = 0x0;
    ShiftAsciiMap[KEY_SCROLL_LOCK] = 0x0;
    ShiftAsciiMap[KEY_KP_7] = 0x0;
    ShiftAsciiMap[KEY_KP_8] = 0x0;
    ShiftAsciiMap[KEY_KP_9] = 0x0;
    ShiftAsciiMap[KEY_KP_MINUS] = ASCII_DASH;
    ShiftAsciiMap[KEY_KP_4] = 0x0;
    ShiftAsciiMap[KEY_KP_5] = 0x0;
    ShiftAsciiMap[KEY_KP_6] = 0x0;
    ShiftAsciiMap[KEY_KP_PLUS] = ASCII_PLUS;
    ShiftAsciiMap[KEY_KP_1] = 0x0;
    ShiftAsciiMap[KEY_KP_2] = 0x0;
    ShiftAsciiMap[KEY_KP_3] = 0x0;
    ShiftAsciiMap[KEY_KP_0] = 0x0;
    ShiftAsciiMap[KEY_KP_PERIOD] = 0x0;

    // SHIFT - extended scan codes
    ShiftAsciiMap[(KEY_RIGHT_ALT & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_RIGHT_CONTROL & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_LEFT_ARROW & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_RIGHT_ARROW & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_UP_ARROW & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_DOWN_ARROW & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_NUMPAD_ENTER & 0xFF) + 0x7F] = ASCII_LINE_FEED;
    ShiftAsciiMap[(KEY_INSERT & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_DELETE & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_HOME & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_END & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_PAGE_UP & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_PAGE_DOWN & 0xFF) + 0x7F] = 0x0;
    ShiftAsciiMap[(KEY_KP_FORWARD_SLASH & 0xFF) + 0x7F] = ASCII_FORWARD_SLASH;
    ShiftAsciiMap[(KEY_PRINT_SCREEN & 0xFF) + 0x7F] = 0x0;

    // NON CAPS - non-extended scan codes
    StdAsciiMap[KEY_ESC] = ASCII_ESCAPE;
    StdAsciiMap[KEY_1] = ASCII_1;
    StdAsciiMap[KEY_2] = ASCII_2;
    StdAsciiMap[KEY_3] = ASCII_3;
    StdAsciiMap[KEY_4] = ASCII_4;
    StdAsciiMap[KEY_5] = ASCII_5;
    StdAsciiMap[KEY_6] = ASCII_6;
    StdAsciiMap[KEY_7] = ASCII_7;
    StdAsciiMap[KEY_8] = ASCII_8;
    StdAsciiMap[KEY_9] = ASCII_9;
    StdAsciiMap[KEY_0] = ASCII_0;
    StdAsciiMap[KEY_MINUS] = ASCII_DASH;
    StdAsciiMap[KEY_EQUALS] = ASCII_EQUALS;
    StdAsciiMap[KEY_BACKSPACE] = ASCII_BACKSPACE;
    StdAsciiMap[KEY_TAB] = ASCII_TAB;
    StdAsciiMap[KEY_Q] = ASCII_Q_LOWER;
    StdAsciiMap[KEY_W] = ASCII_W_LOWER;
    StdAsciiMap[KEY_E] = ASCII_E_LOWER;
    StdAsciiMap[KEY_R] = ASCII_R_LOWER;
    StdAsciiMap[KEY_T] = ASCII_T_LOWER;
    StdAsciiMap[KEY_Y] = ASCII_Y_LOWER;
    StdAsciiMap[KEY_U] = ASCII_U_LOWER;
    StdAsciiMap[KEY_I] = ASCII_I_LOWER;
    StdAsciiMap[KEY_O] = ASCII_O_LOWER;
    StdAsciiMap[KEY_P] = ASCII_P_LOWER;
    StdAsciiMap[KEY_LEFT_BRACE] = ASCII_LEFT_SQUARE_BRACKET;
    StdAsciiMap[KEY_RIGHT_BRACE] = ASCII_RIGHT_SQUARE_BRACKET;
    StdAsciiMap[KEY_ENTER] = ASCII_LINE_FEED;
    StdAsciiMap[KEY_LEFT_CONTROL] = 0x0;
    StdAsciiMap[KEY_A] = ASCII_A_LOWER;
    StdAsciiMap[KEY_S] = ASCII_S_LOWER;
    StdAsciiMap[KEY_D] = ASCII_D_LOWER;
    StdAsciiMap[KEY_F] = ASCII_F_LOWER;
    StdAsciiMap[KEY_G] = ASCII_G_LOWER;
    StdAsciiMap[KEY_H] = ASCII_H_LOWER;
    StdAsciiMap[KEY_J] = ASCII_J_LOWER;
    StdAsciiMap[KEY_K] = ASCII_K_LOWER;
    StdAsciiMap[KEY_L] = ASCII_L_LOWER;
    StdAsciiMap[KEY_SEMICOLON] = ASCII_SEMICOLON;
    StdAsciiMap[KEY_APOSTROPHE] = ASCII_APOSTROPHE;
    StdAsciiMap[KEY_TILDE] = ASCII_BACKTICK;
    StdAsciiMap[KEY_LEFT_SHIFT] = 0x0;
    StdAsciiMap[KEY_BACK_SLASH] = ASCII_BACK_SLASH;
    StdAsciiMap[KEY_Z] = ASCII_Z_LOWER;
    StdAsciiMap[KEY_X] = ASCII_X_LOWER;
    StdAsciiMap[KEY_C] = ASCII_C_LOWER;
    StdAsciiMap[KEY_V] = ASCII_V_LOWER;
    StdAsciiMap[KEY_B] = ASCII_B_LOWER;
    StdAsciiMap[KEY_N] = ASCII_N_LOWER;
    StdAsciiMap[KEY_M] = ASCII_M_LOWER;
    StdAsciiMap[KEY_COMMA] = ASCII_COMMA;
    StdAsciiMap[KEY_PERIOD] = ASCII_PERIOD;
    StdAsciiMap[KEY_FORWARD_SLASH] = ASCII_FORWARD_SLASH;
    StdAsciiMap[KEY_RIGHT_SHIFT] = 0x0;
    StdAsciiMap[KEY_KP_STAR] = ASCII_ASTERISK;
    StdAsciiMap[KEY_LEFT_ALT] = 0x0;
    StdAsciiMap[KEY_SPACE] = ASCII_SPACE;
    StdAsciiMap[KEY_CAPS_LOCK] = 0x0;
    StdAsciiMap[KEY_F1] = 0x0;
    StdAsciiMap[KEY_F2] = 0x0;
    StdAsciiMap[KEY_F3] = 0x0;
    StdAsciiMap[KEY_F4] = 0x0;
    StdAsciiMap[KEY_F5] = 0x0;
    StdAsciiMap[KEY_F6] = 0x0;
    StdAsciiMap[KEY_F7] = 0x0;
    StdAsciiMap[KEY_F8] = 0x0;
    StdAsciiMap[KEY_F9] = 0x0;
    StdAsciiMap[KEY_F10] = 0x0;
    StdAsciiMap[KEY_NUM_LOCK] = 0x0;
    StdAsciiMap[KEY_SCROLL_LOCK] = 0x0;
    StdAsciiMap[KEY_KP_7] = 0x0;
    StdAsciiMap[KEY_KP_8] = 0x0;
    StdAsciiMap[KEY_KP_9] = 0x0;
    StdAsciiMap[KEY_KP_MINUS] = ASCII_DASH;
    StdAsciiMap[KEY_KP_4] = 0x0;
    StdAsciiMap[KEY_KP_5] = 0x0;
    StdAsciiMap[KEY_KP_6] = 0x0;
    StdAsciiMap[KEY_KP_PLUS] = ASCII_PLUS;
    StdAsciiMap[KEY_KP_1] = 0x0;
    StdAsciiMap[KEY_KP_2] = 0x0;
    StdAsciiMap[KEY_KP_3] = 0x0;
    StdAsciiMap[KEY_KP_0] = 0x0;
    StdAsciiMap[KEY_KP_PERIOD] = 0x0;

    // NON CAPS - extended scan codes
    StdAsciiMap[(KEY_RIGHT_ALT & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_RIGHT_CONTROL & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_LEFT_ARROW & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_RIGHT_ARROW & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_UP_ARROW & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_DOWN_ARROW & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_NUMPAD_ENTER & 0xFF) + 0x7F] = ASCII_LINE_FEED;
    StdAsciiMap[(KEY_INSERT & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_DELETE & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_HOME & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_END & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_PAGE_UP & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_PAGE_DOWN & 0xFF) + 0x7F] = 0x0;
    StdAsciiMap[(KEY_KP_FORWARD_SLASH & 0xFF) + 0x7F] = ASCII_FORWARD_SLASH;

    // NUM LOCK - standard scan codes
    NumLockAsciiMap[KEY_KP_7] = ASCII_7;
    NumLockAsciiMap[KEY_KP_8] = ASCII_8;
    NumLockAsciiMap[KEY_KP_9] = ASCII_9;
    NumLockAsciiMap[KEY_KP_4] = ASCII_4;
    NumLockAsciiMap[KEY_KP_5] = ASCII_5;
    NumLockAsciiMap[KEY_KP_6] = ASCII_6;
    NumLockAsciiMap[KEY_KP_1] = ASCII_1;
    NumLockAsciiMap[KEY_KP_2] = ASCII_2;
    NumLockAsciiMap[KEY_KP_3] = ASCII_3;
    NumLockAsciiMap[KEY_KP_0] = ASCII_0;
    NumLockAsciiMap[KEY_KP_PERIOD] = ASCII_PERIOD;

    AsciiMapsPopulated = 1;
}

UINT16 ShiftStateOrderedMasks[16] = {
    SHIFTSTATE_SHIFT_RIGHT,
    SHIFTSTATE_SHIFT_LEFT,
    SHIFTSTATE_CTRL,
    SHIFTSTATE_ALT,
    SHIFTSTATE_SCROLL_LOCK_ON,
    SHIFTSTATE_NUM_LOCK_ON,
    SHIFTSTATE_CAPS_LOCK_ON,
    SHIFTSTATE_INSERT_MODE,
    SHIFTSTATE_CTRL_LEFT,
    SHIFTSTATE_ALT_LEFT,
    SHIFTSTATE_CTRL_RIGHT,
    SHIFTSTATE_ALT_RIGHT,
    SHIFTSTATE_SCROLL_LOCK_DOWN,
    SHIFTSTATE_NUM_LOCK_DOWN,
    SHIFTSTATE_CAPS_LOCK_DOWN,
    SHIFTSTATE_SYS_REQ_DOWN
};

UINT16 ShiftStateOrderedKeys[16] = {
    KEY_RIGHT_SHIFT,
    KEY_LEFT_SHIFT,
    KEY_LEFT_CONTROL,
    KEY_LEFT_ALT,
    KEY_SCROLL_LOCK,
    KEY_NUM_LOCK,
    KEY_CAPS_LOCK,
    KEY_INSERT,
    KEY_LEFT_CONTROL,
    KEY_LEFT_ALT,
    KEY_RIGHT_CONTROL,
    KEY_RIGHT_ALT,
    KEY_SCROLL_LOCK,
    KEY_NUM_LOCK,
    KEY_CAPS_LOCK,
    0x0
};

UINT16 ShiftStateOrderedKeysAlternate[16] = {
    0x0,
    0x0,
    KEY_RIGHT_CONTROL,
    KEY_RIGHT_ALT,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0
};

/* The structures that will hold the old and new keyboard handlers */
_go32_dpmi_seginfo old_key_handler,new_key_handler;

extern int KeyboardStateFlag;           /* Current keybd handler */
UINT8 KeyStateArray[MAX_SCANCODES];     /* Current key state */
UINT8 PressedArray[MAX_SCANCODES];      /* Flags set if key hit */
UINT16 LastDownShiftState[MAX_SCANCODES]; /* Shift state captured when the key was last down - used to accurately associate state with key-up events */
UINT16 ShiftState;
int KeyUpBuffer[255];
int KeyUpBufferCount = 0;
UINT16 ShiftStateBuffer[255];
UINT8 AsciiBuffer[255];
UINT8 IsPreviousCodeExtended = 0;

struct KeyUpInfo {
    UINT16 ShiftState;
    UINT16 ScanCode;
    UINT8 AsciiCharacter;
};

//FUNCTION PROTOTYPES/////////////////////////////////////////////////////////


int KeyIntHandler(void);
void KeyIntHandler_end();
int SetKb(void);
int ResetKb(void);
int KeyState(int scancode);
int TrueKeyState(int scancode);
int KeyUpWaiting();
KeyUpInfo GetNextKeyUpCode();
UINT8 GetAsciiFromScanCode(UINT16 ScanCode, UINT16 ShiftState);

//END PROTOCOL////////////////////////////////////////////////////////////////


#endif //!define INC_KEYBOARD_

/*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*/
