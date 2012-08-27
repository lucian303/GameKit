//MOUSEDMO.CPP
//DEMONSTRATES MOUSE
#include <stdlib.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\palette.h"
#include "..\h\clip.h"
#include "..\h\bitmap.h"
#include "..\h\keyboard.h"
#include "..\h\vbuffer.h"
#include "..\h\animate.h"
#include "..\h\fixed.h"
#include "..\h\mouse.h"


int main () {

	//TheKeyBoard->Install (); //install the keyboard driver
	TheScreen->SetMode13h(); //set mode13h (don't forget)
   //ScreenPal->Load("animdemo.vpl", "animdemo.pal");
	ScreenPal->PutToScreen();

	Bitmap * MyBitmap = new Bitmap;
   MyBitmap->Load("ball.vbm");

	VirtualBuffer * buf = new VirtualBuffer;
//   MyBitmap->PutRegular(0,0,VGAMEMORY);




   //do {   } while (!(TheKeyBoard->GetKeyState(KEY_ESC)));
  // for (int dddd = 0; dddd < 9999999L; dddd ++);

	int sx = 0, sy = 0;

	for ( sx = 0; sx < 199; sx++) {
		sy = sx;
		MyBitmap->PutScaled(0,0,sx,sy,buf->Memory);
		buf->CopyToScreen();
	}

	TheMouse->Show();
/*	for ( sx = 100; sx > 0; sx--) {
		sy = sx;
		MyBitmap->PutScaled(0,0,sx,sy,buf->Memory);
		buf->CopyToScreen();

	}

  */

   //for (int ddddd = 0; ddddd < 9999999L; ddddd ++);


   //do {   } while (!(TheKeyBoard->GetKeyState(KEY_ESC)));


	TheScreen->SetTextMode();
	//TheKeyBoard->Remove ();
	return 0;

}