//Bitmap.cpp
//The bitmap class
//Bitmap format
//word with, height, transparency, fade
//bitmap data size = width * height
//bitmap palette -> 768 bytes
//different from screen palette
#include <iostream.h>
#include <string.h>
#include <fstream.h>
#include <assert.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\palette.h"
#include "..\h\clip.h"
#include "..\h\bitmap.h"
#include "..\h\fixed.h"

Bitmap::Bitmap () {
   width = height = 0;
   BmpSource = NULL; //so it doesn't get messed up
}

Bitmap::Bitmap (char * TheFile) {
   width = height = 0;
	BmpSource = NULL; //so it doesn't get messed up
	Load(TheFile);

}

Bitmap::~Bitmap () {
   if ( BmpSource) {
      delete BmpSource;
   }
}
int Bitmap::GetHeight () { return height;}
int Bitmap::GetWidth () { return width;}

int Bitmap::MakeNewBmp (UINT32 w, UINT32 h, UCHAR * Source, UCHAR Pal[256][3]) {

	width = w;
	height = h;
   if ( BmpSource) {
      delete BmpSource;
   }
	BmpSource = new UCHAR [width * height];
	if ( BmpSource == NULL) {
		return 1;
	}
	memcpy (BmpSource, Source, (width * height));
   for ( int col = 0; col < 256; col++) {
		ThePal[col][0] = Pal[col][0];
		ThePal[col][1] = Pal[col][1];
		ThePal[col][2] = Pal[col][2];
	}
	return 0;
}




int Bitmap::Load (char * file) {
	ifstream InFile;
	InFile.open( file, ios::in | ios::binary | ios::nocreate);
	if ( !InFile) {
		return 1;
	}
   //InFile >> width >> height >> transparency >> fade;
   UCHAR NumberBuf[8]; // = new char[8]

   InFile.read(NumberBuf, 8);
   width = *(UINT32 *) &NumberBuf[0];
   height = *(UINT32 *) &NumberBuf[4];

   //delete [] NumberBuf;

   BmpSource = new  UCHAR [ (width * height) ];
	if ( BmpSource == NULL)
	{
		delete BmpSource;
		return 2;
	}

   InFile.read(BmpSource, (width * height));

   for ( int col = 0; col < 256; col++) {
		InFile.get (ThePal[col][0]);
		InFile.get (ThePal[col][1]);
		InFile.get (ThePal[col][2]);
	}

	InFile.close();
	return 0;
}

int Bitmap::SaveToDisk (char * file) {
	ofstream OutFile;
	OutFile.open( file, ios::out | ios::binary);
	if ( !OutFile) {
		return 1;
	}

   //UCHAR NumberBuf[8]; // = new char[8]

	//NumberBuf[0] = *(UCHAR *) &width;
	OutFile.write((char *) &width, sizeof(width));
	OutFile.write((char *) &height, sizeof(height));

	//OutFile.write(NumberBuf, 8);
	//OutFile << width << height << transparency << fade;
	//OutFile

	OutFile.write(BmpSource, width * height);

   for ( int col = 0; col < 256; col++) {
		OutFile.put (ThePal[col][0]);
		OutFile.put (ThePal[col][1]);
		OutFile.put (ThePal[col][2]);
	}

	OutFile.close();
	//UCHAR myval = ScreenPal.VirPal[0][0][0]; a test!
	return 0;
}

/*Putscaled
 * Puts scaled bitmaps!
 * no clip yet
 * dw = destwidth, dh =destheight
 * scales first line right only for now !!!!
*/

void Bitmap::PutScaled (UINT32 x, UINT32 y, UINT32 dw, UINT32 dh, char * offs) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
   int BltWidth;  /* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
   char * DestScreen; /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   int ScreenIncrement;
	int tdh = dh, tdw = dw; //so we can change tdh for clipping while still maintaining scaling

   Left = x;
   Top  = y;

	//clipping!!!

	if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) dw) < ScreenClip->x1 ||
	(Top + (int)dh) < ScreenClip->y1)
      return;

	int BmOffsY = 0;
	int ClipTop = Top;

   // if ( Top < 0 ) {
   //     int t = (0 - Top);
   //     BmOffsY += t * BltWidth;
   //     tdh -= t;
    //    ClipTop = 0;
    //}

    //if ( Top > ScreenHeight ) {
//        tdh = 320 - Top;
  //      //top + heig = 320 heigh = 320 - top bla
    //}


   TempOffset = ClipTop * ScreenWidth + Left;
   DestScreen = offs + TempOffset;

   BltHeight = height;
   BltWidth = width;

   BMap = BmpSource + BmOffsY;

   /* blt to screen */
   ScreenIncrement = ScreenWidth - dw; //screen class

   int srcPos = 0, destPosX = 0, destPosY = 0, numX = dw >> 1, numY = 0; //dh >> 1;
	//int numX = 0, numY = 0;
   //dw and dh are >> 1 or divided by 2


	while ( destPosY < tdh) {
		while ( destPosX < tdw) {
         //*DestScreen = *(BMap + srcPos);
         //DestScreen++;
         *DestScreen++ = BMap[srcPos];
			destPosX++;
			numX += BltWidth;
			while ( numX > dw) {
				numX -= dw;
				srcPos++;
			}
		}
		destPosX = 0;
		numX = dw >> 1;
		//numX = 0;
		destPosY++;
		numY += BltHeight;
      DestScreen += ScreenIncrement;
      srcPos-=BltWidth;
      while ( numY > dh) {
			numY -= dh;
         srcPos+=BltWidth;

	   }
	}

}



/*PutRegular -> Put The Bitmap Without Transparent pixels, effects, or scaling*/
void Bitmap::PutRegular (UINT32 x, UINT32 y, char * offs) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen; /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned HeightCounter;
	unsigned WidthCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;


   Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
	//doesn't need transparent pixels, so we can easily memcpy each line for speed
	/*for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
      memcpy ((void  *)Screen, (void *)BMap, BltWidth);
		Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }  doesn't work have to optimize*/
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
			*Screen++ = *BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }

}




//PutTransparent Puts bitmap with PIXELS of color 0 transparent!!
void Bitmap::PutTransparent (UINT32 x, UINT32 y, char * offs) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	if (*BMap != 0) {
				*Screen = *BMap;
			}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}

//PutTransparent18bit Puts bitmap with PIXELS of color 0 transparent!! it uses the
//virtual 18bit palette
void Bitmap::PutTransparent18bit (UINT32 x, UINT32 y, char * offs) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   int tempr, tempg, tempb;//temporary color vars


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	if (*BMap != 0) {
				//this blits with the 18bit color!!!!
				tempr = (int)ThePal[*BMap][0];
				tempg = (int)ThePal[*BMap][1];
				tempb = (int)ThePal[*BMap][2];
            *Screen = (ScreenPal->VirPal[tempr][tempg][tempb]);

				//*Screen = *BMap;
			}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}

//PutRegular18it Puts bitmap
//uses virtual 18bit palette
void Bitmap::PutRegular18bit (UINT32 x, UINT32 y, char * offs) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   int tempr, tempg, tempb;//temporary color vars


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	//if (*BMap != 0) {
				//this blits with the 18bit color!!!!
			tempr = (int)ThePal[*BMap][0];
			tempg = (int)ThePal[*BMap][1];
			tempb = (int)ThePal[*BMap][2];
         *Screen = (ScreenPal->VirPal[tempr][tempg][tempb]);

				//*Screen = *BMap;
			//}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}


//Puts w/ transparen pixels, but also fades
void Bitmap::PutTransparentFaded (UINT32 x, UINT32 y, char * offs,
   INT32 rfade, INT32 gfade, INT32 bfade) {


	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   int tempr, tempg, tempb;//temporary color vars


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	if (*BMap != 0) {
				//this blits with the 18bit color!!!!
            tempr = (int)ThePal[*BMap][0] + rfade;
				(tempr < 0) ? tempr = 0 : tempr;
				(tempr > 63) ? tempr = 63 : tempr;
				tempg = (int)ThePal[*BMap][1] + gfade;
				(tempg < 0) ? tempg = 0 : tempg;
				(tempg > 63) ? tempg = 63 : tempg;
				tempb = (int)ThePal[*BMap][2] + bfade;
				(tempb < 0) ? tempb = 0 : tempb;
				(tempb > 63) ? tempb = 63 : tempb;

            *Screen = ScreenPal->VirPal[tempr][tempg][tempb];
				//*Screen = *BMap;
			}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}

//Puts wo/ transparen pixels, but also fades
void Bitmap::PutRegularFaded (UINT32 x, UINT32 y, char * offs,
   INT32 rfade, INT32 gfade, INT32 bfade) {


	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   int tempr, tempg, tempb;//temporary color vars


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
			//this blits with the 18bit color!!!!
		      tempr = (int)ThePal[*BMap][0] + rfade;
				(tempr < 0) ? tempr = 0 : tempr;
				(tempr > 63) ? tempr = 63 : tempr;
				tempg = (int)ThePal[*BMap][1] + gfade;
				(tempg < 0) ? tempg = 0 : tempg;
				(tempg > 63) ? tempg = 63 : tempg;
				tempb = (int)ThePal[*BMap][2] + bfade;
				(tempb < 0) ? tempb = 0 : tempb;
				(tempb > 63) ? tempb = 63 : tempb;

         *Screen = ScreenPal->VirPal[tempr][tempg][tempb];

        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}

void Bitmap::PutTransparentSemi (UINT32 x, UINT32 y, char * offs,
   float rtrans, float gtrans, float btrans) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;


	//assert (rtrans => 0);
	//assert (gtrans => 0);
	//assert (btrans => 0);
	//assert (rtrans <= 1);
	//assert (gtrans <= 1);
	//assert (btrans <= 1);

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   int tempr, tempg, tempb;//temporary color vars
   fixed rt = ConvertToFix((float)1 - rtrans);
   fixed gt = ConvertToFix((float)1 - gtrans);
   fixed bt = ConvertToFix((float)1 - btrans);

   fixed rtr = ConvertToFix(rtrans);
   fixed gtr = ConvertToFix(gtrans);
   fixed btr = ConvertToFix(btrans);


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	if (*BMap != 0) {
				//this blits with the 18bit color!!!!
          /*  tempr = ((int)ScreenPal->RegPal[*Screen][0] * rtrans +
					(int)ThePal[*BMap][0] * (1 - rtrans));

            tempg = ((int)ScreenPal->RegPal[*Screen][1] * gtrans +
					(int)ThePal[*BMap][1] * (1 - gtrans));

            tempb = ((int)ScreenPal->RegPal[*Screen][2] * btrans +
					(int)ThePal[*BMap][2] * (1 - btrans));*/

				tempr = ConvertToInt(FixMulFix(ConvertToFix((short)ScreenPal->RegPal[*Screen][0]),
               rtr) + FixMulFix (ConvertToFix((short)ThePal[*BMap][0]), rt));

				tempg = ConvertToInt(FixMulFix(ConvertToFix((short)ScreenPal->RegPal[*Screen][1]),
               gtr) + FixMulFix (ConvertToFix((short)ThePal[*BMap][1]), gt));

				tempb = ConvertToInt(FixMulFix(ConvertToFix((short)ScreenPal->RegPal[*Screen][2]),
               btr) + FixMulFix (ConvertToFix((short)ThePal[*BMap][2]), bt));

            *Screen = ScreenPal->VirPal[tempr][tempg][tempb];
				//*Screen = *BMap;

			}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}


void Bitmap::PutRegularSemi (UINT32 x, UINT32 y, char * offs,
   float rtrans, float gtrans, float btrans) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;


	//assert (rtrans => 0);
	//assert (gtrans => 0);
	//assert (btrans => 0);
	//assert (rtrans <= 1);
	//assert (gtrans <= 1);
	//assert (btrans <= 1);

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   int tempr, tempg, tempb;//temporary color vars
   fixed rt = ConvertToFix((float)1 - rtrans);
   fixed gt = ConvertToFix((float)1 - gtrans);
   fixed bt = ConvertToFix((float)1 - btrans);

   fixed rtr = ConvertToFix(rtrans);
   fixed gtr = ConvertToFix(gtrans);
   fixed btr = ConvertToFix(btrans);


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
         //if (*BMap != 0) {
         //this blits with the 18bit color!!!!
         /*  tempr = ((int)ScreenPal->RegPal[*Screen][0] * rtrans +
            (int)ThePal[*BMap][0] * (1 - rtrans));

         tempg = ((int)ScreenPal->RegPal[*Screen][1] * gtrans +
            (int)ThePal[*BMap][1] * (1 - gtrans));

         tempb = ((int)ScreenPal->RegPal[*Screen][2] * btrans +
            (int)ThePal[*BMap][2] * (1 - btrans));*/

         tempr = ConvertToInt(FixMulFix(ConvertToFix((short)ScreenPal->RegPal[*Screen][0]),
            rtr) + FixMulFix (ConvertToFix((short)ThePal[*BMap][0]), rt));

         tempg = ConvertToInt(FixMulFix(ConvertToFix((short)ScreenPal->RegPal[*Screen][1]),
            gtr) + FixMulFix (ConvertToFix((short)ThePal[*BMap][1]), gt));

         tempb = ConvertToInt(FixMulFix(ConvertToFix((short)ScreenPal->RegPal[*Screen][2]),
            btr) + FixMulFix (ConvertToFix((short)ThePal[*BMap][2]), bt));

         *Screen = ScreenPal->VirPal[tempr][tempg][tempb];
         //*Screen = *BMap;

         Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}



//PutTransparent Puts bitmap with PIXELS of color 0 transparent in one color
void Bitmap::PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, char thecol) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	if (*BMap != 0) {
				*Screen = thecol;
			}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}

//PutTransparentOneCol Puts bitmap with PIXELS of color 0 transparent!! in one color
//it uses the virtual 18bit palette
void Bitmap::PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, int cr, int cg, int cb) {

	int Top;	/* coordinate values of bitmap top-left corner */
   int Left;
	unsigned BMOffsetX;	/* starting offset into clipped bitmap */
   unsigned BMOffsetY;
   unsigned ClippedLeft;/* top-left corner position of clipped bitmap */
   unsigned ClippedTop;
   int BltWidth;	/* width of clipped bitmap */
   int BltHeight; /* height of clipped bitmap */
   UINT32 TempOffset;	/* temp variable to calc far pointer offsets */
	char * Screen;  /* pointer to current screen position */
	UCHAR * BMap; /* pointer to current bitmap position */
   unsigned WidthCounter;
   unsigned HeightCounter;
   unsigned ScreenIncrement;
   unsigned BitmapIncrement;

	Left = x;
   Top  = y;

   if (Left >= (int) ScreenClip->x2 || Top >= (int) ScreenClip->y2 ||
   (Left + (int) width) < ScreenClip->x1 ||
	(Top + (int)height) < ScreenClip->y1)
      return;

   /* clip bitmap to upper left edge of rect */

/* if Left < ScreenClip->x1, calculate starting x offset in bitmap */
   if (Left < ScreenClip->x1) {
   BMOffsetX = ScreenClip->x1 - Left;
      ClippedLeft = ScreenClip->x1;
      BltWidth = width - BMOffsetX;
   }
   else {
   BMOffsetX = 0;
      ClippedLeft = Left;
      BltWidth = width;
	}

   /* if Top < ScreenClip->y1, calculate starting y offset in bitmap */
   if (Top < ScreenClip->y1) {
   BMOffsetY = ScreenClip->y1 - Top;
      ClippedTop = ScreenClip->y1;
      BltHeight = height - BMOffsetY;
   }
   else {
   BMOffsetY = 0;
      ClippedTop = Top;
      BltHeight = height;
   }

   /* clip bitmap to lower right edge of rect */

   /* if Left + bitmap width > rect width, calc ending x point */
   if ((ClippedLeft + BltWidth) > ScreenClip->x2) {
   BltWidth -= ClippedLeft + BltWidth - ScreenClip->x2;
   }

   /* if Top + bitmap height > rect height, calc ending y point */
   if ((ClippedTop + BltHeight) > ScreenClip->y2) {
   BltHeight -= ClippedTop + BltHeight - ScreenClip->y2;
   }

   /* calculate screen pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = ClippedTop * ScreenWidth + ClippedLeft;


	Screen = offs + TempOffset;

   /* calculate bitmap pointer starting position based on */
   /* clipped bitmap location */
   TempOffset = BMOffsetY * width + BMOffsetX;
   BMap = BmpSource;
   BMap += TempOffset;

   //int tempr, tempg, tempb;//temporary color vars


   /* blt to screen */
   ScreenIncrement = ScreenWidth - BltWidth; //screen class
   BitmapIncrement = width - BltWidth;
   for (HeightCounter = 0; HeightCounter < BltHeight; HeightCounter++) {
   	for (WidthCounter = 0; WidthCounter < BltWidth; WidthCounter++) {
      	if (*BMap != 0) {
				//this blits with the 18bit color!!!!
            //tempr = (int)ThePal[*BMap][0];
            //tempg = (int)ThePal[*BMap][1];
            //tempb = (int)ThePal[*BMap][2];
            *Screen = (ScreenPal->VirPal[cr][cg][cb]);

				//*Screen = *BMap;
			}
        	Screen++;
        	BMap++;
      }
      Screen += ScreenIncrement;
      BMap += BitmapIncrement;
   }
}
