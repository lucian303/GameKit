#include <iostream.h>
#include "..\h\archive.h"


int main (int argc, char ** argv ) {
	if ( argc < 1 )
		cout << "\nError: Not Enough Arguments";

	Archive * myarc = new Archive;
	char com = *(argv[1]);
	switch (com) {
		case 'a' : //add
		cout << "\nArchive: " << argv[2] << "\nAdding: " << argv[5] << " as: " <<
			argv[3] << " in directory: " << argv[4];
		myarc->Load(argv[2]);
		myarc->Add(argv[3], argv[4], argv[5]);
		break;

		case 'x' :
		cout << "\nArchive: " << argv[2] << "\nExtracting: " << argv[5] << " from: " <<
			argv[3] << " in directory: " << argv[4];

		myarc->Load(argv[2]);
		myarc->Extract(argv[3], argv[4], argv[5]);
		break;

		default:
		cout << "\nError: Not a valid command.";
	}

	cout << "\nDone";
	return 0;
}



