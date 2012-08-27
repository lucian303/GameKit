//Palette.h
#ifndef PALETTE_H
#define PALETTE_H

#include "..\h\gkit.h"

class Palette {

	protected:
	UCHAR SavePal[256][3];

	void ComputeFadeStep (UINT8 StartPalette[256][3],UINT8 EndPalette[256][3],
		int Start, int Length, long NumSteps, long CurrentStep,
		UINT8 FadeStepPalette[256][3]);

	void SetVgaPaletteBlock (UCHAR MyPal[256][3], int Start, int Length);

	public:
	UCHAR VirPal[64][64][64];
	UCHAR RegPal[256][3];

	int LoadRegPal (char * file);//
	int LoadVirPal (char * file); //
	int Load (char * virpalfile, char * realpalfile);//

	void FadePaletteBlock (	UINT8 StartPalette[256][3], UINT8 EndPalette[256][3],
   	int Start, int Length, int Milliseconds);//

	void Palette::LightenOrDarken(int Start, int Len, int MilliSec,
		int rdark, int gdark, int bdark);

	void FadeOut(int MilliSec);//
	void Palette::PrepareFadeIn();//
	void FadeIn(int MilliSec);//

	void MakeBlack ();//
	void MakeBlackSaved ();

	void Save ();//
	void Save (int Start, int Length);

	void Restore ();//
	void Restore (int Start, int Length);

	void PutSavedToScreen (); //
	void PutToScreen (); //

	void GetFromScreen (); //
	void GetSavedFromScreen ();

	void PutToScreen (int Start, int Length);
	void PutSavedToScreen (int Start, int Length);

	void GetFromScreen (int Start, int Length);
	void GetSavedFromScreen (int Start, int Length);

	void Palette::Rotate (int Rotation, int Start, int Length);

	Palette (char * vfile, char * rfile);//
	Palette::Palette () {};//
	~Palette ();//

};

extern Palette * ScreenPal;
#endif
