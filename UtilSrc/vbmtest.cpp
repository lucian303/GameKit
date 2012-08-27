#include <iostream.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\palette.h"
#include "..\h\clip.h"
#include "..\h\bitmap.h"
#include "..\h\keyboard.h"
#include "..\h\vbuffer.h"
#include "..\h\animate.h"

#define UPLIMIT 63
#define LOWLIMIT -63

int main (int argc, char * argv[]) {

	int x = 0, y = 0, xinc = 1, yinc = 1;
   int rf = LOWLIMIT, gf= LOWLIMIT, bf = LOWLIMIT, rfinc = 1, gfinc= 1, bfinc =1,
		rs = .5, gs = .5, bs = .5, rsinc = .1, gsinc = .1, bsinc = .1;

	if ( argc < 1 ) {
		cout << "Not Enough Arguments\n";
		return 1;
	}

	TheKeyBoard->Install (); //install the keyboard driver

	//clip is set by default to 0,0 - 319, 199
	//ScreenClip->Set (50,50, 319-50,199-50);

	TheScreen->SetMode13h(); //set mode13h (don't forget)
	ScreenPal->Load("vbmview.vpl", "vbmview.pal");
	ScreenPal->PutToScreen();
	VirtualBuffer * MyBuffer = new VirtualBuffer;


	//Bitmap * MyBitmap = new Bitmap; //make a new bitmap
	//int retval = MyBitmap->Load(argv[1]); //load a sample bitmap
	char * myfiles [] = {
		"back.vbm",
		"ball.vbm",
		"ball3.vbm",
		"nowman1.vbm",
		"nba1.vbm"
	};

	int mydurs [] = {
		15,35,15,50,15
	};

	Animation * MyAnim = new Animation;
	MyAnim->AddAll(5, myfiles, mydurs);
	AnimationRef * Anim1 = new AnimationRef;
	Anim1->AssignAnimation(MyAnim);
	AnimationRef * Anim2 = new AnimationRef;
	Anim2->AssignAnimation(MyAnim);



	//if ( retval == 0) //do we NOT have an error?
	//{
		ScreenPal->PrepareFadeIn();

		MyBuffer->Clear();
		//MyBitmap->PutRegularFaded (x, y, MyBuffer->Memory, rf, gf, bf);
		MyAnim->PutRegular18bit (x, y, MyBuffer->Memory);

		MyBuffer->CopyToScreen();

		ScreenPal->FadeIn(1500);


		do {
			x +=xinc;
			y+=yinc;

			rf += rfinc;
			gf += gfinc;
			bf += bfinc;
			rs += rsinc;
			gs += gsinc;
			bs += bsinc;

			if (rs < 0)
				 rs = 0; rsinc = -rsinc;
			if (rs > 1)
				 rs = 1; rsinc = -rsinc;

			if (gs < 0)
				 gs = 0; gsinc = -gsinc;
			if (gs > 1)
				 gs = 1; gsinc = -gsinc;


			if (bs < 0)
				 bs = 0; bsinc = -bsinc;
			if (bs > 1)
				 bs = 1; bsinc = -bsinc;



			if ( rf < LOWLIMIT) {
				rf =LOWLIMIT; rfinc = -rfinc;
			}
			if ( gf < LOWLIMIT) {
				gf = LOWLIMIT; gfinc = -gfinc;
			}
			if ( bf < LOWLIMIT) {
				bf = LOWLIMIT; bfinc = -bfinc;
			}

			if ( rf >UPLIMIT) {
				rf = UPLIMIT; rfinc = -rfinc;
			}
			if ( gf > UPLIMIT) {
				gf = UPLIMIT; gfinc = -gfinc;
			}
			if ( bf > UPLIMIT) {
				bf = UPLIMIT; bfinc = -bfinc;
			}


			if ( x < 0) {
				x = 0;
				xinc = -xinc;
			}
			if ( y < 0) {
				y = 0;
				yinc = -yinc;
			}
			if ( (x + MyAnim->GetWidth() ) > 319) {
				x = 319 - MyAnim->GetWidth();
				xinc = -xinc;
			}

			if ( (y + MyAnim->GetHeight() ) > 199) {
				y = 199 - MyAnim->GetHeight();
				yinc = -yinc;
			}

			/*if ( (x + 100) > 319) {
				x = 319 - 10;
				xinc = -xinc;
			}

			if ( (y + 100) > 199) {
				y = 199 - 100;
				yinc = -yinc;
			}  */


			MyBuffer->Clear();
			//MyBitmap->PutRegularFaded (x, y, MyBuffer->Memory, rf, gf, bf);

			Anim1->Advance();
			Anim2->Advance();
			Anim2->Advance();
			Anim1->PutRegular18bit (x, y, MyBuffer->Memory);
			Anim2->PutRegular18bit (x + 100, y, MyBuffer->Memory);
			TheScreen->WaitVerticalRetraceStart();
			MyBuffer->CopyToScreen();

		} while (!TheKeyBoard->GetKeyState(KEY_ESC) );

		 ScreenPal->FadeOut(1500);

	//}


	TheKeyBoard->Remove(); //remove keyboard driver (SUPERLY important!!!)
	TheScreen->SetTextMode(); //set text mode (important!!!)
	//delete MyBitmap; //delete this bitmap
	delete MyAnim;

	return 0;
}




