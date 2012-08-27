//Palette.cpp
//A class to manage the screen palette
#include <iostream.h>
#include <fstream.h>
#include <conio.h>
#include <assert.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\palette.h"

Palette * ScreenPal = new Palette;

#define STEPS_PER_SECOND	(70)	/* number of fade steps per second */
									/*   = 1 per VGA scan frame */


Palette::Palette (char * vfile, char * rfile) {
	Load (vfile, rfile);
	return;
}

Palette::~Palette (){
	return;
}


void Palette::SetVgaPaletteBlock (UCHAR MyPal[256][3], int Start, int Length) {

   int End = Start + Length;
   outp(COLOR_ADDRESS_WRITE, Start);


   for ( int tru = Start; tru < End ; tru++) {

		outp(COLOR_DATA, MyPal[tru][0]);
		outp(COLOR_DATA, MyPal[tru][1]);
		outp(COLOR_DATA, MyPal[tru][2]);

	}

	return ;
}


/*
	Function: ComputeFadeStep
    Description:
    	Computes a palette for a single step of a palette fade.  The
        starting palette and ending palette for the fade are given.
        The amount to fade each entry in the block is computed
        as the ratio of the current step to the number of steps
        in the fade.  Note that when step = 0, the resulting
        palette equals the starting palette and when step =
        NumSteps, the resulting palette equals the ending palette.
        Only the palette entries specified in the block will be
        altered in the FadeStepPalette.  A calling routine
        should use CopyPaletteBlock from either the starting or
        ending palettes to ensure that FadeStepPalette contains
        correct entries outside the specified block.
*/
void Palette::ComputeFadeStep
	(
	UINT8 StartPalette[256][3],
	UINT8 EndPalette[256][3],
	int Start,
	int Length,
	long NumSteps,
	long CurrentStep,
	UINT8 FadeStepPalette[256][3]
	)
{
	int Index;
    long RDelta;
    long GDelta;
    long BDelta;

	assert(StartPalette != NULL);
	assert(EndPalette != NULL);
    assert(0 <= Start && Start <= 255);
    assert(Length > 0);
    assert(Start + Length <= 256);
    assert(NumSteps > 0);
    assert(0 <= CurrentStep && CurrentStep <= NumSteps);
    assert(FadeStepPalette != NULL);

    for (Index = Start; Index < Start + Length; Index++) {
		RDelta = EndPalette[Index][0] - StartPalette[Index][0];
        RDelta = (RDelta * CurrentStep) / NumSteps;

		GDelta = EndPalette[Index][1] - StartPalette[Index][1];
        GDelta = (GDelta * CurrentStep) / NumSteps;

		BDelta = EndPalette[Index][2] - StartPalette[Index][2];
        BDelta = (BDelta * CurrentStep) / NumSteps;

        FadeStepPalette[Index][0] = (char)((int)StartPalette[Index][0] + (int)RDelta);
        FadeStepPalette[Index][1] = (char)((int)StartPalette[Index][1] + (int)GDelta);
        FadeStepPalette[Index][2] = (char)((int)StartPalette[Index][2] + (int)BDelta);
    }
}

/*
	Function: FadePaletteBlock
    Description:
    	Fades a block of palette entries on the screen.  The starting
        and ending palettes are given as well as the block to fade
        and the length of time to fade.  The routine computes the
        number of fade steps from the length of time given.  It
        then computes each fade step and sets the block of VGA
        palette entries to the values computed for the step.
        Note that the VGA palette corresponding to the block
        specified should equal what is contained in StartPalette,
        otherwise a color jump will result when the palette is
        set on the first fade step.  The routine will leave the
        VGA palette entries corresponding to the block specified
        equal to the values in EndPalette.  If the routine is
        passed a length greater than 128, it will fade half the
        color entries on every other retrace and alternate between
        the two halfs.  This avoids snow.
*/

void Palette::FadePaletteBlock
	(
	UINT8 StartPalette[256][3],
	UINT8 EndPalette[256][3],
    int Start,
    int Length,
	int Milliseconds
	)
{
	UINT8 IntermedPalette[256][3];
	long TotalSteps;
    long Step;

    assert(StartPalette != NULL);
    assert(EndPalette != NULL);
    assert(0 <= Start && Start <= 255);
    assert(Length > 0);
    assert(Start + Length <= 256);
    assert(Milliseconds > 0);

    if (Length <= 128) {
	    TotalSteps = ((long) Milliseconds * STEPS_PER_SECOND) / 1000;
    }
    else {
	    TotalSteps = ((long) (Milliseconds / 2) * STEPS_PER_SECOND) / 1000;
    }

    for (Step = 0; Step <= TotalSteps; Step++) {
    	ComputeFadeStep(StartPalette, EndPalette, Start, Length,
        	TotalSteps, Step, IntermedPalette);
        if (Length <= 128) {

			  TheScreen->WaitVerticalRetraceStart();
			 SetVgaPaletteBlock(IntermedPalette, Start, Length);
        }
        else {

			  TheScreen->WaitVerticalRetraceStart();
    	    SetVgaPaletteBlock(IntermedPalette, Start, Length / 2);
	        TheScreen->WaitVerticalRetraceStart();
    	    SetVgaPaletteBlock(IntermedPalette, Start + (Length / 2),
				Length - (Length / 2));
        }
    }
}


void Palette::Rotate (int Rotation, int Start, int Length) {

	UINT8 TempPalette[256][3];
	int Index;
   int SourceIndex;

	assert(0 <= Start && Start <= 255);
   assert(Length > 0);
   assert(Start + Length <= 256);

	int End = Start + Length;

	if (Rotation < 0) {
   	Rotation = Length + Rotation;
   }

   for (Index = 0; Index < Length; Index++) {
   	SourceIndex = (Index + Rotation) % Length;
   	TempPalette[Start + Index][0] = RegPal[Start + SourceIndex][0];
   	TempPalette[Start + Index][1] = RegPal[Start + SourceIndex][1];
   	TempPalette[Start + Index][2] = RegPal[Start + SourceIndex][2];
   }
	for (int tru = Start; tru < End; tru++) {
		RegPal[tru][0] = TempPalette[tru][0];
		RegPal[tru][1] = TempPalette[tru][1];
		RegPal[tru][2] = TempPalette[tru][2];
	}
	PutToScreen(Start, Length);

	return ;
}


void Palette::LightenOrDarken(int Start, int Len, int MilliSec,
	int rdark, int gdark, int bdark) {
	UCHAR TempPal[256][3];
	int End = Start + Len;
	Save();
	int rt, gt, bt, col;
	for ( col = Start; col <= End; col++) {
		rt = (int)RegPal[col][0];
		gt = (int)RegPal[col][1];
		bt = (int)RegPal[col][2];
		rt += rdark;
		gt += gdark;
		bt += bdark;
      (rt < 0) ? rt = 0 : rt;
      (rt > 63) ? rt = 63 : rt;
      (gt < 0) ? gt = 0 : gt;
      (gt > 63) ? gt = 63 : gt;
      (bt < 0) ? bt = 0 : bt;
      (bt > 63) ? bt = 63 : bt;

		TempPal[col][0] = rt;
		TempPal[col][1] = gt;
		TempPal[col][2] = bt;
	}

	FadePaletteBlock(RegPal, TempPal, 0, 256, MilliSec);

	for ( col = Start; col < End; col++) {
		RegPal[col][0] = TempPal[col][0];
		RegPal[col][1] = TempPal[col][1];
		RegPal[col][2] = TempPal[col][2];
	}

}


void Palette::PrepareFadeIn() {
	MakeBlackSaved();
	PutSavedToScreen();
}

void Palette::FadeIn(int MilliSec)
{
 	//FadePaletteBlock(SavePal, RegPal, 0, 128, MilliSec);
	//FadePaletteBlock(SavePal, RegPal, 128, 256, MilliSec);
	FadePaletteBlock(SavePal, RegPal, 0, 256, MilliSec);

}

/*
	Function: FadeOut
    Description:
    	Fades from the current VGA palette to black.
*/
void Palette::FadeOut(int MilliSec)
{
	MakeBlackSaved();
	FadePaletteBlock(RegPal, SavePal, 0, 256, MilliSec);
}


int Palette::LoadRegPal (char * file) {
	ifstream InFile;
	InFile.open( file, ios::in | ios::binary | ios::nocreate);
	if ( !InFile) {
		return 1;
	}
	//unsigned char RPal[256][3];
	// load array from file

	for ( int col = 0; col <= 256; col++) {

		InFile.get (RegPal[col][0]);
		InFile.get (RegPal[col][1]);
		InFile.get (RegPal[col][2]);
	}

	InFile.close();
	return 0;
}

int Palette::LoadVirPal (char * file) {
	int rtru, gtru, btru;
	ifstream InFile;
	InFile.open( file, ios::in | ios::binary | ios::nocreate);
	if ( !InFile) {
		return 1;
	}

	for (  rtru = 0; rtru < 64; rtru ++) {
		for (  gtru = 0; gtru < 64; gtru ++) {
			for (  btru = 0; btru < 64; btru ++) {
				//	for ( int col = 0; col <= 256; col++) {
				InFile.get (VirPal[rtru][gtru][btru]);
			}
		}
	}

	InFile.close();
	return 0;
}


int Palette::Load (char * virpalfile, char * realpalfile) {
	int my_err;
	my_err = LoadRegPal (realpalfile);
	if ( my_err != 0 ) {
		return 1;
	}

	my_err = LoadVirPal (virpalfile);
	if ( my_err != 0 ) {
		return 2;
	}
	return 0;
}

void Palette::MakeBlack () {
	for ( int col = 0; col < 256; col++) {
		RegPal[col][0] = 0;
		RegPal[col][1] = 0;
		RegPal[col][2] = 0;
	}
	return ;
}

void Palette::MakeBlackSaved () {
	for ( int col = 0; col < 256; col++) {
		SavePal[col][0] = 0;
		SavePal[col][1] = 0;
		SavePal[col][2] = 0;
	}
	return ;
}

void Palette::Save () {
	for ( int col = 0; col < 256; col++) {
		SavePal[col][0] = RegPal[col][0];
		SavePal[col][1] = RegPal[col][1];
		SavePal[col][2] = RegPal[col][2];
	}
	return ;
}

void Palette::Restore () {
	for ( int col = 0; col < 256; col++) {
		RegPal[col][0] = SavePal[col][0];
		RegPal[col][1] = SavePal[col][1];
		RegPal[col][2] = SavePal[col][2];
	}
	return ;
}

void Palette::Save (int Start, int Length) {

	int End = Start + Length;

   for ( int tru = Start; tru < End ; tru++) {

		SavePal[tru][0] = RegPal[tru][0];
		SavePal[tru][1] = RegPal[tru][1];
		SavePal[tru][2] = RegPal[tru][2];

	}

	return ;
}

void Palette::Restore (int Start, int Length) {

	int End = Start + Length;

   for ( int tru = Start; tru < End ; tru++) {

		RegPal[tru][0] = SavePal[tru][0];
		RegPal[tru][1] = SavePal[tru][1];
		RegPal[tru][2] = SavePal[tru][2];

	}

	return ;
}

void Palette::PutToScreen  () {

	outp(COLOR_ADDRESS_WRITE, 0);

	for ( int tru = 0; tru < 256; tru++) {

		outp(COLOR_DATA, RegPal[tru][0]);
		outp(COLOR_DATA, RegPal[tru][1]);
		outp(COLOR_DATA, RegPal[tru][2]);

	}

	return ;
}

void Palette::PutSavedToScreen  () {

	outp(COLOR_ADDRESS_WRITE, 0);

	for ( int tru = 0; tru < 256; tru++) {

		outp(COLOR_DATA, SavePal[tru][0]);
		outp(COLOR_DATA, SavePal[tru][1]);
		outp(COLOR_DATA, SavePal[tru][2]);

	}

	return ;
}

void Palette::GetFromScreen () {

	outp(COLOR_ADDRESS_READ, 0);

	for ( int tru = 0; tru < 256; tru++) {

		RegPal[tru][0] = (char)inp(COLOR_DATA);
		RegPal[tru][1] = (char)inp(COLOR_DATA);
		RegPal[tru][2] = (char)inp(COLOR_DATA);

	}

	return ;
}

void Palette::PutToScreen (int Start, int Length) {

	int End = Start + Length;
   outp(COLOR_ADDRESS_WRITE, Start);

   for ( int tru = Start; tru < End ; tru++) {

		outp(COLOR_DATA, RegPal[tru][0]);
		outp(COLOR_DATA, RegPal[tru][1]);
		outp(COLOR_DATA, RegPal[tru][2]);

	}

	return ;
}


void Palette::PutSavedToScreen (int Start, int Length) {

	int End = Start + Length;
   outp(COLOR_ADDRESS_WRITE, Start);

   for ( int tru = Start; tru < End ; tru++) {

		outp(COLOR_DATA, SavePal[tru][0]);
		outp(COLOR_DATA, SavePal[tru][1]);
		outp(COLOR_DATA, SavePal[tru][2]);

	}

	return ;
}


void Palette::GetFromScreen (int Start, int Length) {

	int End  = Start + Length;
	outp(COLOR_ADDRESS_READ, Start);

	for ( int tru = Start; tru < End; tru++) {

		RegPal[tru][0] = (char)inp(COLOR_DATA);
		RegPal[tru][1] = (char)inp(COLOR_DATA);
		RegPal[tru][2] = (char)inp(COLOR_DATA);

	}

	return ;

}

void Palette::GetSavedFromScreen (int Start, int Length) {

	int End  = Start + Length;
	outp(COLOR_ADDRESS_READ, Start);

	for ( int tru = Start; tru < End; tru++) {

		SavePal[tru][0] = (char)inp(COLOR_DATA);
		SavePal[tru][1] = (char)inp(COLOR_DATA);
		SavePal[tru][2] = (char)inp(COLOR_DATA);

	}

	return ;

}

void Palette::GetSavedFromScreen () {

	outp(COLOR_ADDRESS_READ, 0);

	for ( int tru = 0; tru < 256; tru++) {

		SavePal[tru][0] = (char)inp(COLOR_DATA);
		SavePal[tru][1] = (char)inp(COLOR_DATA);
		SavePal[tru][2] = (char)inp(COLOR_DATA);

	}

	return ;

}



