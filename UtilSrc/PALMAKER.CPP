#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#include <time.h>
#include "..\h\gkit.h"

int main (int argc, char * argv []) {

   cout << "Virtual Palette Maker V1.0 Beta\n" << "Working\n";

	if ( argc < 3 ) {
		cout << "Not Enough Arguments\n" << "Usage:\n" <<
			"\tpalmaker RegPalFile VirPalFile\n" <<
			"\tWhere RegPalFile is the regular 768 byte palette file and\n" <<
			"\tVirPalFile is the Virtual Palette File For The Game Kit (256KB)\n";
		return 1;
	}

	cout << "Please Wait . . .\n";

	// load specified file
	cout << "Input File: " << argv[1] << "\n";
	cout << "Output File: " << argv[2] << "\n";

	ifstream InFile;
	InFile.open( argv[1], ios::in | ios::binary | ios::nocreate);
	if ( !InFile) {
		cout << "Error Opening Input File";
		return 1;
	}
	time_t TheTime;
	TheTime = time( NULL );
   cout << "\nStart Time: " << ctime( &TheTime ) << "\n";

	unsigned char RPal[256][3];
	// load array from file

	for ( int col = 0; col <= 256; col++) {
		InFile.get (RPal[col][0]);
		InFile.get (RPal[col][1]);
		InFile.get (RPal[col][2]);
	}

	InFile.close();

   int rtru, gtru, btru, in;
	float nop = 0;
	char outc, bloc1 = 176, bloc = 219;//, outcdebug;
	int eror, terror, bestmatch, percent;
	struct rccoord old_pos;
	//struct rccoord old_pos;


	ofstream OutFile;
	OutFile.open(argv[2], ios::out | ios::binary);
	//ofstream DebugFil;
	//DebugFil.open("debug.bin", ios::out | ios::binary);

	//cout << "\n";

	old_pos = _gettextposition();

	for ( int foo = 0; foo <= 63; foo ++) {
		cout << bloc1;
	}
	cout << "\n";
	//old_posb = _gettextposition();

	for (  rtru = 0; rtru <= 63; rtru ++) {
		//print % and bar

		_settextposition( old_pos.row, old_pos.col + nop);
		cout << bloc;

		percent = ((nop / 64) * 100);
		_settextposition( old_pos.row + 1, old_pos.col );
		cout << percent << "%";

		nop++;
		//done printing
		for (  gtru = 0; gtru <= 63; gtru ++) {
			for (  btru = 0; btru <= 63; btru ++) {
				eror = 30000;
				bestmatch = 0;
				for ( in = 0; in < 256; in++) {
					//get the error
					terror = sqrt( (rtru - RPal [in][0]) * (rtru - RPal [in][0]) +
										(gtru - RPal [in][1]) * (gtru - RPal [in][1]) +
										(btru - RPal [in][2]) * (btru - RPal [in][2]) );


					if ( terror < eror) {
						// update cause found a better value
						bestmatch = in;
						eror = terror;
					}
				}
				//outcdebug = (char)eror;
				outc = (char)bestmatch;
				OutFile.put (outc);
				//DebugFil.put (outcdebug);
				//cout << ".";
			}
		}
		//cout << "."; //print a period so they can see it's not locked up




	}

	//print out 100% so they know it's done!
	percent = ((nop / 64) * 100);
	_settextposition( old_pos.row + 1, old_pos.col );
	cout << percent << "%";

	OutFile.close();

	//finito (finished)
	TheTime = time( NULL );
   cout << "\n\nEnd Time: " << ctime( &TheTime ) << "\nYou figure it out";

	cout << "\nDone\n"; //duh!

	return 0;
}
