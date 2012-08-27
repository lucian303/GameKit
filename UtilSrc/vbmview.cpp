#include <iostream.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\palette.h"
#include "..\h\clip.h"
#include "..\h\bitmap.h"
#include "..\h\keyboard.h"

int main (int argc, char * argv[]) {

	if ( argc < 1 ) {
		cout << "Not Enough Arguments\n";
		return 1;
	}

	TheKeyBoard->Install (); //install the keyboard driver

	//clip is set by default to 0,0 - 319, 199

	TheScreen->SetMode13h(); //set mode13h (don't forget)
	ScreenPal->Load("vbmview.vpl", "vbmview.pal");
	ScreenPal->PutToScreen();

	Bitmap * MyBitmap = new Bitmap; //make a new bitmap
	int retval = MyBitmap->Load(argv[1]); //load a sample ball bitmap
	if ( retval == 0) //do we NOT have an error?
	{
		//put the bitmap at 0,0 on the screen (at VGAMEMORY location)
		//MyBitmap->PutRegular(0, 0, VGAMEMORY);
		MyBitmap->PutTransparent18bit(0, 0, VGAMEMORY);

		while (!TheKeyBoard->GetKeyState(KEY_ESC));// wait for a key
	}


	TheKeyBoard->Remove(); //remove keyboard driver (SUPERLY important!!!)
	TheScreen->SetTextMode(); //set text mode (important!!!)
	//cout <<"W:  " << MyBitmap->width << "H:  " << MyBitmap->height;
	//MyBitmap->SaveToDisk ("temp1.vbm");
	delete MyBitmap; //delete this bitmap

	return 0;
}




