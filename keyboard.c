/****************************************************************************/
 ////////////A FEW ROUTINES FOR READING THE STATE OF THE KEYBOARD////////////
 ///////////////////////AS IF IT WERE A SET OF BUTTONS///////////////////////
 ////////////////////////////////VERSION 2.30////////////////////////////////
/****************************************************************************/
/*
 ----------------------------------------------------------------------------
|                Copyright 1998-99-2000 Frederico Jeronimo                   |
 ----------------------------------------------------------------------------
                                                                            */

//NOTES///////////////////////////////////////////////////////////////////////


/* Some of the functions that appear in this module are not used in any of
 * these tutorials. They are only here because they are part of my keyboard
 * handling routines. Since the keyboard handler doesn't need to be reentrant
 * we can use the traditional way to set it. But, more on that on my future
 * to be keyboard tutorial. Stay tuned. */

//HISTORY/////////////////////////////////////////////////////////////////////


/* -----------------08-09-98 16:39-----------------
 V1.00 : First working version in real mode.
--------------------------------------------------*/

/* -----------------10-12-98 10:15-----------------
 V2.00 : The keyboard handler now works in protected
 mode.
--------------------------------------------------*/

/* -----------------10-12-98 12:16-----------------
 V2.05 : Added the locking of variables and
 functions. Saved registers and disabled
 interruptions in the handle itself.
--------------------------------------------------*/

/* -----------------12-12-98 15:33-----------------
 V2.10 : Freed the allocated IRET wrapper after
 removing the new keyboard handler. Added a few
 comments.
--------------------------------------------------*/

/* -----------------25-12-99 13:06-----------------
 V2.11 : Added the copyright notice.
--------------------------------------------------*/

/* -----------------18-03-00 07:13PM---------------
 V2.16 : Moved all global data and defines to the
 header file where they belong.
 -------------------------------------------------*/

/* -----------------21-11-00 1:18------------------
 V2.25 : Added a return zero to the handler function,
 marking it as a no chaining handler. The lack of this
 return value caused a bug if this handler was used
 with an assembly wrapper. Other minor adjustments.
--------------------------------------------------*/

/* -----------------27-11-00 3:16------------------
 V2.26 : Corrected the messy presentation of this
 file. Added extra comments more fitting for a
 tutorial.
--------------------------------------------------*/

/* -----------------27-11-00 3:51------------------
 V2.30 : Added error checking to all relevant
 functions.
--------------------------------------------------*/
#ifndef keyboard_c
#define keyboard_c

//INCLUDES////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <dos.h>
#include <dpmi.h>
#include <go32.h>
#include <pc.h>
#include <bios.h>
#include "Const.h"
#include "keyboard.h"

//GLOBALS/////////////////////////////////////////////////////////////////////


int KeyboardStateFlag = OLD_HANDLER;    /* Current keybd handler */


//FUNCTIONS///////////////////////////////////////////////////////////////////

/*****************************************************************************
 * Function KeyIntHandler :
 *
 * Replacement for the BIOS int 9 handler. Detects when a key is pressed or
 * released. Updtades KeyStateArray to reflect the state of each key. Sets
 * KeyStateArray to 1 if key is currently being held down, 0 if released.
 * When a key is released, PressedArray is set to 1. This allows a program
 * to detect that a key was pressed and then released between checks of the
 * array. PressedArray is cleared by the program reading the state of a key
 * and not by this routine. KeyStateArray and PressedArray are indexed by
 * the keyboard scan code returned from the keyboard controller.
 ****************************************************************************/

int KeyIntHandler(void)
{
    UINT8 ScanCode;
	UINT8 Temp;

    /* Clear interrupts and save registers */
	asm("cli ; pusha");

    /* Let's read the Scancode */
	ScanCode = inportb(KEYBOARD_INPUT);

    /* Clear Keyboard Controller on XT machines */
	Temp = inportb(KEYBOARD_XT_CLEAR);
    outportb(KEYBOARD_XT_CLEAR, Temp | 0x80); /* Sets the highbit to 1 */
    outportb(KEYBOARD_XT_CLEAR, Temp & 0x7F); /* Resets the highbit */

    /* Key is up if the highbit of ScanCode is set to 1. We use 0x80 because
     * it corresponds to 10000000 in binary and by ADDing it to ScanCode, the
     * conditional if expression is set to 1 if the ScanCode's highbit is also
     * 1 and set to 0 otherwise */
    if (ScanCode == 0xE0){
        //extended scancode
        IsPreviousCodeExtended = 1;
    }
    else if (ScanCode & 0x80)
    {
        
        /*Key is up*/
        ScanCode &= 0x7F;
        
        int storeCodeOffset = 0;
        UINT16 storeBufferCode = ScanCode;
        if (IsPreviousCodeExtended){
            storeCodeOffset = 0x7F; //add 127 to KeyStateArray index for storing extended values
            storeBufferCode =  (0xE0 << 8) | ScanCode; //create a new scan code for the 2x byte value stored as 16-bit value - this should match the consts for comparison
            IsPreviousCodeExtended = 0;
        }

        KeyStateArray[ScanCode + storeCodeOffset] = 0;
		KeyUpBuffer[KeyUpBufferCount] = storeBufferCode;

        //un toggle any shift states,
        for (int i = 0; i < 16; i++){
            if (ScanCode == ShiftStateOrderedKeys[i] || ScanCode == ShiftStateOrderedKeysAlternate[i]){
                if (i >= 4 && i <= 7){
                    //lock style keys should be toggled from existing state
                    if (ShiftState & ShiftStateOrderedMasks[i]){
                        //was on previously, now needs to be turned off. set nth bit to 0
                        ShiftState &= ~(1UL << i);
                    } else {
                        //was off previously, now needs to be turned on. Set nth bit to 1
                        ShiftState |= 1UL << i;
                    }
                } else {
                    //since it's key up, toggle the state to OFF
                    ShiftState &= ~(1UL << i);
                }
            }
        }

        UINT16 ShiftStateAtDown = LastDownShiftState[ScanCode + storeCodeOffset];
        ShiftStateBuffer[KeyUpBufferCount] = ShiftStateAtDown;
        AsciiBuffer[KeyUpBufferCount] = GetAsciiFromScanCode(storeBufferCode, ShiftStateAtDown);
		KeyUpBufferCount++;
    }

    else
	{
		/*Key is down*/
        int storeCodeOffset = 0;
        if (IsPreviousCodeExtended){
            storeCodeOffset = 0x7F; //add 127 to KeyStateArray index for storing extended values
            IsPreviousCodeExtended = 0;
        }

        KeyStateArray[ScanCode + storeCodeOffset] = 1; /* Reflects that the key is being pressed */
		PressedArray[ScanCode + storeCodeOffset] = 1;

        //un toggle any shift states,
        for (int i = 0; i < 16; i++){
            if (ScanCode == ShiftStateOrderedKeys[i] || ScanCode == ShiftStateOrderedKeysAlternate[i]){
                if (i >= 4 && i <= 7){
                    //lock style keys can be handle by key up only
                    continue;
                } else {
                    //since it's key down, toggle the state to ON
                    ShiftState |= 1UL << i;
                }
            }
        }

        LastDownShiftState[ScanCode + storeCodeOffset] = ShiftState;

	}

    /* This command tells the PIC (programmable interrupt controller) that the
     * highest priority interrupt has been serviced and clears the interrupt */
    outportb(PIC, NONSPECIFIC_EOI);

    /* Re-enable interruptions and restore the registers */
	asm("popa; sti");
    return RET_SUCCESS;
}
END_OF_FUNCTION(KeyIntHandler);  /* Used for locking */

/// Get an ascii code from scan code. The scan code is expected to match the value stored in the constants, and not the code from a single byte handle by the ISR itself.
UINT8 GetAsciiFromScanCode(UINT16 ScanCode, UINT16 ShiftState){

    UINT16 lookupId = ScanCode;
    if(ScanCode >> 8 == 0xE0){
        //extended code matching constants was passed in, convert it to array lookup value
        // Get rid of the extended code
        ScanCode &= 0xFF;
        lookupId = ScanCode + 0x7F;
    }

    if (ShiftState & SHIFTSTATE_NUM_LOCK_ON && NumLockAsciiMap[lookupId] > 0x0){
        // first check if the num lock collection contains an ascii character (num pad chars)
        return NumLockAsciiMap[lookupId];
    }

    if (ShiftState & SHIFTSTATE_SHIFT_LEFT || ShiftState & SHIFTSTATE_SHIFT_RIGHT){
        //shift key is down, return a value from the shift lookup table
        return ShiftAsciiMap[lookupId];
    }

    if (ShiftState & SHIFTSTATE_CAPS_LOCK_ON){
        //caps mode, return a value from the caps lookup table
        return CapsAsciiMap[lookupId];
    }

    return StdAsciiMap[lookupId];
}
END_OF_FUNCTION(GetAsciiFromScanCode);

//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
 * Function SetKb :
 *
 * Sets up the keyboard as a set of buttons. To do this SetKb() initializes
 * the Key State and Pressed arrays and installs the INT 9 handler,
 * KeyIntHandler().
 ****************************************************************************/

int SetKb(void)
{
    //Before overriding the ISR, capture the shift state so we have the correct toggle position for caps lock/etc
    ShiftState = _bios_keybrd(_NKEYBRD_SHIFTSTATUS);
    PopulateAsciiMaps();

	int i;

    /* If the next expression is not true, the calls to SetKb() and ResetKb()
     * have not been balanced and trouble could arise when you quit the program
     * and try to restore the original INT 9 handler */
    assert(KeyboardStateFlag == OLD_HANDLER);

    /* Initialize state arrays */
	for( i=0; i<128; i++)
        KeyStateArray[i] = PressedArray[i] = 0;

    /* If an interrupt handler, any other functions it invokes, or any
     * variables it touches, is 'paged out', the code will bomb out with
     * a page fault. The solution is to 'lock' the memory regions that must
     * be available, telling the DPMI host to keep them in active memory at
     * all times. This is done with the following lines. */
	LOCK_VARIABLE(old_key_handler);
	LOCK_VARIABLE(new_key_handler);
	LOCK_VARIABLE(KeyStateArray);
	LOCK_VARIABLE(PressedArray);
	LOCK_FUNCTION((void *)KeyIntHandler);
    LOCK_VARIABLE(ShiftStateBuffer);
    LOCK_VARIABLE(ShiftStateOrderedKeys);
    LOCK_VARIABLE(ShiftState);
    LOCK_VARIABLE(ShiftStateOrderedKeysAlternate);
    LOCK_VARIABLE(ShiftStateOrderedMasks);
    LOCK_VARIABLE(IsPreviousCodeExtended);
    LOCK_VARIABLE(AsciiBuffer);
    LOCK_VARIABLE(CapsAsciiMap);
    LOCK_VARIABLE(NumLockAsciiMap);
    LOCK_VARIABLE(StdAsciiMap);
    LOCK_VARIABLE(ShiftAsciiMap);
    LOCK_VARIABLE(LastDownShiftState);
    LOCK_FUNCTION((void *)GetAsciiFromScanCode);


    /* Load the address of the function that defines the new interrupt handler
     * in the 'pm_offset' field of the new handler structure */
	new_key_handler.pm_offset   = (int)KeyIntHandler;
	new_key_handler.pm_selector = _go32_my_cs();

    /* old_key_handle stores the original INT9 handler so that it can be
     * restored later on when the new handle is no longer needed */
	_go32_dpmi_get_protected_mode_interrupt_vector(0x9, &old_key_handler);

    /* This function creates a small assembly routine to handle the overhead
     * of servicing an interrupt (adds a IRET at the end). The function we
     * wish to execute is passed in the 'pm_offset' field of the structure it
     * receives as an argument. This produces a similar effect to the
     * 'interrupt' keyword found in other compilers */
    if(_go32_dpmi_allocate_iret_wrapper(&new_key_handler) != 0)
        return RET_FAILURE;

    /* This function replaces the old handler with the new one */
    if(_go32_dpmi_set_protected_mode_interrupt_vector(0x9,&new_key_handler) != 0)
    {
        _go32_dpmi_free_iret_wrapper(&new_key_handler);
        return RET_FAILURE;
    }

    /* Mark the new handler as being installed */
    KeyboardStateFlag = NEW_HANDLER;

    return RET_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
 * Function ResetKb :
 *
 * The ResetKb function sets the INT9 handler back to the routine saved in
 * OldInt9handler. This routine should only be called after SetKb() has been
 * called. If it is called before SetKb is called at least once, the INT 9
 * vector will be set to garbage.
 ****************************************************************************/

int ResetKb(void)
{
    /* If the next expression doesn't evaluate to true, there could be some
     * deep shit at program termination */
    assert(KeyboardStateFlag == NEW_HANDLER);

    /* Reinstall the original handler */
    if(_go32_dpmi_set_protected_mode_interrupt_vector(0x9,&old_key_handler) !=0)
        return RET_FAILURE;

    /* Free the allocated IRET wrapper */
    if(_go32_dpmi_free_iret_wrapper(&new_key_handler) != 0)
        return RET_FAILURE;

    /* Indicate that the old handler is current */
    KeyboardStateFlag = OLD_HANDLER;

    return RET_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 * Function KeyState :
 *
 * This routine returns 1 if the key is currently down or was pressed since
 * this function was last called for the key. 0 otherwise. The function
 * continues to return 1 as long as the key is being held down. KeyState()
 * should only be called while in SetKb() mode
 ****************************************************************************/

int KeyState(int ScanCode)
{
	int result;

    assert(KeyboardStateFlag == NEW_HANDLER); /* Check if SetKb() mode is on */

    // Check for the extended code
    if(ScanCode >> 8 == 0xE0)
    {
        // Get rid of the extended code
        ScanCode &= 0xFF;
        result = KeyStateArray[ScanCode + 0x7F] | PressedArray[ScanCode + 0x7F];
        PressedArray[ScanCode + 0x7F] = 0; /* Clear PressedArray */
    }
    else
    {
        result = KeyStateArray[ScanCode] | PressedArray[ScanCode];
        PressedArray[ScanCode] = 0; /* Clear PressedArray */
    }

    /* Returns 1 if either the key is being pressed , or has been pressed
     * since the routine was last called for that scancode */
    return result;
}

//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
 * Function TrueKeyState :
 *
 * Returns 1 if the key is currently down. The function continues to return
 * 1 as long as the key is held down. This routine differs from KeyState()
 * in that it does not check if the key has been pressed and then released
 * before this function was called. Note that KeyState() will still point
 * out that this has happened, even if this function is called first. This
 * function should only be called while in SetKb() mode.
 ****************************************************************************/

int TrueKeyState(int ScanCode)
{
	int result;

    assert(KeyboardStateFlag == NEW_HANDLER); /* Check if Setkb() mode is on */

    // Check for the extended code
    if(ScanCode >> 8 == 0xE0)
    {
        // Get rid of the extended code
        ScanCode &= 0xFF;
        result = KeyStateArray[ScanCode + 0x7F];
    }
    else
    {
        result = KeyStateArray[ScanCode];
    }

	return result;
}

unsigned int GetShiftState(){
    return ShiftState;
}

int KeyUpWaiting(){
	return (KeyUpBufferCount > 0);
}

KeyUpInfo GetNextKeyUpCode(){
    KeyUpInfo keyInfo;
    keyInfo.ScanCode = 0;
    keyInfo.ShiftState = 0;
    if (KeyUpBufferCount == 0){ return keyInfo; }
	KeyUpBufferCount--;
    keyInfo.ScanCode = KeyUpBuffer[KeyUpBufferCount];
	keyInfo.ShiftState = ShiftStateBuffer[KeyUpBufferCount];
    keyInfo.AsciiCharacter = AsciiBuffer[KeyUpBufferCount];
    return keyInfo;
}

#endif

/*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*/
