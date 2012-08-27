#ifndef BITMAP_H
#define BITMAP_H

#include "..\h\gkit.h"
#include "..\h\fixed.h"

class Bitmap {
	protected:
	UINT32 width, height;
	UCHAR * BmpSource;
	UCHAR ThePal[256][3];

	public:
	Bitmap (char * TheFile);
	Bitmap ();
	~Bitmap ();
	int MakeNewBmp (UINT32 w, UINT32 h, UCHAR * Source, UCHAR Pal[256][3]);
	int Load (char * file); //done
	int SaveToDisk (char * file); //done
	void PutRegular (UINT32 x, UINT32 y, char * offs);
	void PutTransparent (UINT32 x, UINT32 y, char * offs);
	void PutRegular18bit (UINT32 x, UINT32 y, char * offs);
	void PutTransparent18bit (UINT32 x, UINT32 y, char * offs);

	void PutTransparentFaded (UINT32 x, UINT32 y, char * offs,
       INT32 rfade, INT32 gfade, INT32 bfade);

	void PutRegularFaded (UINT32 x, UINT32 y, char * offs,
	   INT32 rfade, INT32 gfade, INT32 bfade);

   void PutTransparentSemi (UINT32 x, UINT32 y, char * offs, float rtrans, float gtrans, float btrans);
   void PutRegularSemi (UINT32 x, UINT32 y, char * offs, float rtrans, float gtrans, float btrans);

   void PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, char thecol);
   void PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, int cr, int cg, int cb);

	//scaled puts!
	void Bitmap::PutScaled (UINT32 x, UINT32 y, UINT32 dw, UINT32 dh, char * offs);

   int GetWidth ();
   int GetHeight ();

};


#endif