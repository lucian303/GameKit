#include <iostream.h>
#include "..\h\gkit.h"
#include "..\h\bitmap.h"
#include "..\utilsrc\pcx2vbm.h"
#include "..\utilsrc\pcx.h"

//#include <conio.h>
//#include "..\h\mode13h.h"

int main (int argc, char * Files []) {

	cout << "PCX to VBM Converter Version 0.1Beta\n" << "Working . . .\n";

	if ( argc < 2 ) {
		cout << "Not Enough Arguments\n" << "Usage:\n" <<
			"\tpcx2vbm pcxfile1 pcxfile2 . . . pcxfilen" <<
			"\tWhere pcxfile1 to pcxfile2 are PCX files to be converted\n";
	}

	for (int tru = 1; tru <= (argc - 1); tru++) {

		//make names usable
      //char * thisFile = strcpy(thisFile, *(Files + tru));
      //char * writeFile = strcpy(writeFile, *(Files + tru));
      char thisFile[128];
      char writeFile[128];
      //char * thisFile = new char [12];
      //char * writeFile = new char [12];
      strcpy(thisFile, *(Files + tru));
      strcpy(writeFile, *(Files + tru));
		int repplace = 0;
		for (int tru2 = 0; tru2 < 20; tru2++) {
			char thechar = *(writeFile + tru2);
			if (thechar == '.') {
				repplace = tru2;
            break;
			}
		}
		char * vbmstr = ".vbm";
		memcpy(writeFile + repplace, vbmstr, 4);

		cout << "Converting " << thisFile <<  " to " << writeFile << "\n";

		LINEAR_BITMAP * TempBitmap = new LINEAR_BITMAP;
		UCHAR PCXPal[256][3];

		//load the pcx file
		TempBitmap = LoadPCX(thisFile, PCXPal);
		if (TempBitmap == NULL) {
			delete TempBitmap;
			cout << "\nCan't load file: " << thisFile << "\n";
			return 1;
		}

		//transfer stuff from one bitmap to another
		Bitmap * MyBitmap = new Bitmap;
		UCHAR * tPointer = (UCHAR *)&(TempBitmap->Data);
		//convert stuff
		MyBitmap->MakeNewBmp (TempBitmap->Width, TempBitmap->Height, tPointer, PCXPal);
		//save it
		MyBitmap->SaveToDisk (writeFile);
      //TheScreen->SetMode13h(); //set mode13h (don't forget)
      //MyBitmap->PutRegular (0, 0, VGAMEMORY);
      //getch();
      //TheScreen->SetTextMode(); //set mode13h (don't forget)
      delete MyBitmap;
	}

	cout << "Done";

	return 0;
}

