
#include <stdio.h>
#include "..\h\keyboard.h"

/* string names for each of the characters in the numbers row */
char *KeyNameArray[] = {
   "",            /* 0x00 - place holder */
    "Esc",        /* 0x01 - place holder */
    "1",       /* 0x02 */
    "2",       /* 0x03 */
    "3",       /* 0x04 */
    "4",       /* 0x05 */
    "5",       /* 0x06 */
    "6",       /* 0x07 */
    "7",       /* 0x08 */
    "8",       /* 0x09 */
    "9",       /* 0x0A */
    "0",       /* 0x0B */
    "-",       /* 0x0C */
    "=",       /* 0x0D */
    "Backspace"      /* 0x0E */
};


int main(void)
{
   int i;

   //extern KeyBoard * TheKeyBoard;
   TheKeyBoard->Install();

    while (!TheKeyBoard->GetKeyState(KEY_ESC)) {

      for (i = 2; i <= 0x0E ; i++) {
         if (TheKeyBoard->GetKeyState(i)) {
               printf("%s ", KeyNameArray[i]);
            }
        }
        printf("\n");
    }

    TheKeyBoard->Remove();

    return 0;
}

