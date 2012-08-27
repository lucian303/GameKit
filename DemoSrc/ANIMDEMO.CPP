//#include <iostream.h>
#include <stdlib.h>
//#include <i86.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\palette.h"
#include "..\h\clip.h"
#include "..\h\bitmap.h"
#include "..\h\keyboard.h"
#include "..\h\vbuffer.h"
#include "..\h\animate.h"
#include "..\h\fixed.h"

#define MAX_CREATURES 20

//protos
void MoveCreatures();
void DrawCreatures();
void InitCreatures();
void InitOneCreature (int n);

struct FallingCreature {

	AnimationRef * MyAnim;
	int x, y, xinc, yinc, alive;
};

Animation * MasterAnim = new Animation;
VirtualBuffer * MyBuffer = new VirtualBuffer;
FallingCreature MyCreatures[MAX_CREATURES];

float rs;
float gs;
float bs;

int main () {

   rs = bs = gs =.5;

	TheKeyBoard->Install (); //install the keyboard driver

	//clip is set by default to 0,0 - 319, 199
	//ScreenClip->Set (50,50, 319-50,199-50);

	TheScreen->SetMode13h(); //set mode13h (don't forget)
   ScreenPal->Load("animdemo.vpl", "animdemo.pal");
	ScreenPal->PutToScreen();

	char * myfiles [] = { "1.vbm", "2.vbm", "3.vbm", "4.vbm", "5.vbm", "6.vbm", "7.vbm",
      "8.vbm","9.vbm","10.vbm" };

	int mydurs [] = {
      10,10,10,10,10,10,10,10,10,10
	};

   MasterAnim->AddAll(10, myfiles, mydurs);

	Bitmap * MyBitmap = new Bitmap;
	MyBitmap->Load("back.vbm");

	InitCreatures();

	ScreenPal->PrepareFadeIn();
	MyBuffer->Clear();
	MyBitmap->PutRegular18bit(0,0,MyBuffer->Memory);
	DrawCreatures();
	MyBuffer->CopyToScreen();
	ScreenPal->FadeIn(1500);

   float cinc = .1;
	do {
		//MyBuffer->Clear();
		MoveCreatures ();
		MyBitmap->PutRegular18bit(0,0,MyBuffer->Memory);
      rs += cinc;
      if (rs >= 1) {
         rs= 1;
         cinc = -cinc;
      }
      if (rs <= 0) {
         rs= 0;
         cinc = -cinc;
      }
      gs = bs = rs;

      DrawCreatures();
      //MasterAnim->Advance();
      //MasterAnim->PutTransparent18bit(MyCreatures[0].x, MyCreatures[0].y,
      //      MyBuffer->Memory);

		TheScreen->WaitVerticalRetraceStart();
		//TheScreen->WaitVerticalRetraceStart();
		//TheScreen->WaitVerticalRetraceStart();

		MyBuffer->CopyToScreen();
		//delay(1000);

	} while (!TheKeyBoard->GetKeyState(KEY_ESC) );

	ScreenPal->FadeOut(1500);

	TheKeyBoard->Remove(); //remove keyboard driver (SUPERLY important!!!)
	TheScreen->SetTextMode(); //set text mode (important!!!)
	delete MasterAnim;
	delete MyBitmap;

	return 0;
}

void MoveCreatures() {
	for ( int tru = 0; tru < MAX_CREATURES; tru++) {
		//for ( int tru = 0; tru < MAX_CREATURES; tru++)
      MyCreatures[tru].MyAnim->Advance();

		MyCreatures[tru].x += MyCreatures[tru].xinc;
		MyCreatures[tru].y += MyCreatures[tru].yinc;

		if ( MyCreatures[tru].x <= 0 ) {
			MyCreatures[tru].x = 0;
			MyCreatures[tru].xinc = - MyCreatures[tru].xinc;
		}

      if ( MyCreatures[tru].x + MyCreatures[tru].MyAnim->GetWidth() >= 319 ) {
         MyCreatures[tru].x = 319 - MyCreatures[tru].MyAnim->GetWidth();
			MyCreatures[tru].xinc = - MyCreatures[tru].xinc;
		}

      if ( MyCreatures[tru].y <= 0 ) {
         MyCreatures[tru].y = 0;
         MyCreatures[tru].yinc = - MyCreatures[tru].yinc;
		}

      if ( MyCreatures[tru].y + MyCreatures[tru].MyAnim->GetHeight() >= 199 ) {
         MyCreatures[tru].y = 199 - MyCreatures[tru].MyAnim->GetHeight();
         MyCreatures[tru].yinc = - MyCreatures[tru].yinc;
      }


      /*if ( MyCreatures[tru].y >= 199 ) {
			MyCreatures[tru].alive = 0;
			InitOneCreature(tru);
      } */

	}
}

void DrawCreatures() {
	for ( int tru = 0; tru < MAX_CREATURES; tru++) {
		if ( MyCreatures[tru].alive != 0) {
			MyCreatures[tru].MyAnim->PutTransparent18bit(MyCreatures[tru].x, MyCreatures[tru].y,
				MyBuffer->Memory);
			//MyCreatures[tru].MyAnim->PutTransparentSemi(MyCreatures[tru].x, MyCreatures[tru].y,
			//	MyBuffer->Memory, rs, gs, bs);
			//MyCreatures[tru].MyAnim->PutTransparentOneCol(MyCreatures[tru].x, MyCreatures[tru].y,
			//	MyBuffer->Memory, 32);


		}
	}
}


void InitCreatures() {
	for ( int tru = 0; tru < MAX_CREATURES; tru++) {
		InitOneCreature(tru);
   	MyCreatures[tru].MyAnim = new AnimationRef;
   	MyCreatures[tru].MyAnim->AssignAnimation(MasterAnim);

	}
}


void InitOneCreature (int n) {

	//srand (rand ());
	//srand (rand ());

   MyCreatures[n].x = 160;
   MyCreatures[n].y = 50;
   MyCreatures[n].xinc = n;
   MyCreatures[n].yinc = n;
	MyCreatures[n].alive = 1;

}

