// Keyboard.cpp
//The keyboard handling functions
#include <assert.h>
#include <dos.h>
#include <conio.h>
#include "..\h\gkit.h"
#include "..\h\keyboard.h"

/* constants */
#define MAX_SCANCODES      (128)
#define NEW_HANDLER        (1)
#define OLD_HANDLER        (0)
#define KEYBOARD_INPUT     (0x60)
#define KEYBOARD_XT_CLEAR  (0x61)

/* module variables */
static void __far interrupt (*OldInt9Handler)();  /* old keybd int handler */
static int     KeyboardStateFlag = OLD_HANDLER;/* current keybd handler */
static UINT8   KeyStateArray[MAX_SCANCODES]; /* current key state */
static UINT8   WasPressedArray[MAX_SCANCODES];  /* flags set if key hit */

KeyBoard * TheKeyBoard = new KeyBoard;


/*
   Function: KeyIntHandler
    Description:
      Replacement for the BIOS Int 9 handler.  Detects when a
      key is pressed and released.  Updates KeyStateArray to
      reflect the state of each key.  Sets KeyStateArray to 1 if key
      is currently being held down, 0 if released.  When a key is
      released, WasPressedArray is set to 1.  This allows a program to
      detect that a key was pressed and then released between
      checks of the array.  WasPressedArray is cleared by the program
      reading the state of a key, not by this routine.  KeyStateArray
      and WasPressedArray are indexed by the keyboard scan code returned
      from the keyboard controller.
*/
static void __far interrupt KeyIntHandler(void)
{
    UINT8 ScanCode;
    UINT8 Temp;

    /* read scan code */
    ScanCode = inp(KEYBOARD_INPUT);

    /* clear keyboard controller on XT machines */
    Temp = inp(KEYBOARD_XT_CLEAR);
    outp(KEYBOARD_XT_CLEAR, Temp | 0x80);
    outp(KEYBOARD_XT_CLEAR, Temp & 0x7F);

    if (ScanCode & 0x80) {
      /* key up */
        ScanCode &= 0x7F;
        KeyStateArray[ScanCode] = 0;
    }
    else {
      /* key down */
        KeyStateArray[ScanCode] = 1;
        WasPressedArray[ScanCode] = 1;
    }

    outp(PIC, NONSPECIFIC_EOI);
}

/*
   Function: SetButtonKeysMode
   Description:
      Sets up the keyboard as a set of buttons.  To do this,
      SetButtonKeysMode() initializes the key state and was-pressed
      arrays and installs the INT 9 handler, KeyIntHandler().
*/
void SetButtonKeysMode(void)
{
   int i;

   /* if this is not true, the calls to SetButtonKeysMode() and */
   /* SetNormalKeysMode() have not been balanced and trouble */
   /* could result when you quit the program and try to */
   /* restore the original INT 9 handler. */
   assert(KeyboardStateFlag == OLD_HANDLER);

   /* initialize state arrays */
   for (i = 0; i < 128; i++) {
      KeyStateArray[i] = WasPressedArray[i] = 0;
   }

   /* install new handler */
   OldInt9Handler = _dos_getvect(9);
   _dos_setvect(9, KeyIntHandler);

    /* mark new handler as being installed */
   KeyboardStateFlag = NEW_HANDLER;
}

/*
   Function: SetNormalKeysMode
   Description:
      NormalKeys() sets the INT 9 handler back to the routine
      stored in OldInt9Handler.  This routine should only be called
      after SetButtonKeysMode() has been called.  If this routine is
      called before SetButtonKeysMode() has been called at least once,
      the INT 9 vector will be set to garbage.
*/
void SetNormalKeysMode(void)
{
   /* if this is not true, there could be big problems at */
   /* program termination.  */
   assert(KeyboardStateFlag == NEW_HANDLER);

    /* reinstall original handler */
   _dos_setvect(9, OldInt9Handler);

    /* indicate that the old handler is current */
   KeyboardStateFlag = OLD_HANDLER;
}

/*
   Function: GetKeyState
   Description:
      Returns 1 if the key is currently down or was pressed
      since this function was last called for the key,
      0 otherwise.  The function continues to return 1 as
      long as the key is held down.  This function should only
      be called while in SetButtonKeysMode() mode.
*/
int KeyBoard::GetKeyState(int ScanCode)
{
   int result;

   /* scan codes should only be 0-127 */
   assert(ScanCode < 128);
   assert(KeyboardStateFlag == NEW_HANDLER);

   result = KeyStateArray[ScanCode] | WasPressedArray[ScanCode];
   WasPressedArray[ScanCode] = 0;
   return result;
}

/*
   Function: GetTrueKeyState
   Description:
      Returns 1 if the key is currently down.  The function
      continues to return 1 as long as the key is held down.
      This function differs from GetKeyState() in that it does
      not check if the key has been pressed and then released
      before this function was called.  Note that GetKeyState()
      will still indicate if this has happened, even if this
      function is called first.  This function should only
      be called while in SetButtonKeysMode() mode.
*/
int KeyBoard::GetTrueKeyState(int ScanCode)
{
   int result;

   /* scan codes should only be 0-127 */
   assert(ScanCode < 128);
   assert(KeyboardStateFlag == NEW_HANDLER);

   result = KeyStateArray[ScanCode];
   return result;
}

void KeyBoard::Install () {
   SetButtonKeysMode();
}
void KeyBoard::Remove () {
   SetNormalKeysMode();
}
