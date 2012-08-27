//archive.cpp
//implements an archive class to store multiple files in one, and compress them???
//no compression yet
/*format
long numfiles
file entries:
char[128] name, char[128] directory, long start, long length
All the data...
*/
#include <string.h>
#include <fstream.h>
#include <stdio.h>
#include "..\h\gkit.h"
#include "..\h\archive.h"

Archive::Archive () {
	numfiles = 0;
	TheFiles = NULL;
}

Archive::~Archive () {
	TFile.close();
	delete TheFiles;
}

void Archive::Add (char * myfile, char * mydir, char * dosfile) {
	fstream tempfile;
	char * buf;
	long size;
	buf = DOSLoadToMemory(dosfile, size);
	tempfile.open("temp.tmp", ios::out | ios::binary);
	tempfile.seekg(0, ios::beg);

	long numf = numfiles + 1;
	tempfile.write((char *) &numf, sizeof(numf));
	tempfile.write(myfile, 128);
	tempfile.write(mydir, 128);
	//write start
   TFile.close();
   TFile.open(myfile, ios::in | ios::out | ios::binary); //reload
   TFile.seekg(0, ios::end);
   long thestart = TFile.tellg();
	thestart += 264; //extend because adding a file!!!!!
	tempfile.write((char *) &thestart, sizeof(thestart));

	tempfile.write((char *) &size, sizeof(size));

	//write rest
	long tru;
   long tmp1;
	for (tru = 0; tru < numfiles; tru++) {
      tmp1 = TheFiles[tru]->TheStart + 264;
      tempfile.write(TheFiles[tru]->TheName, 128);
		tempfile.write(TheFiles[tru]->DirectoryName, 128);
		tempfile.write((char *) &tmp1, sizeof(tmp1));
		tempfile.write((char *) &TheFiles[tru]->Length, sizeof(TheFiles[tru]->Length));
	}
   char * buf1 = NULL;
	long tru1;
	long tmp2;

	for (tru1 = 0; tru1 < numfiles; tru1++) {
		if ( buf1 != NULL)
			delete buf1;

		buf1 = new char [TheFiles[tru1]->Length];
		tmp2 = TheFiles[tru1]->TheStart + 264;

		TFile.seekg(TheFiles[tru1]->TheStart, ios::beg);
		TFile.read(buf1, TheFiles[tru1]->Length);
		tempfile.seekg(tmp2, ios::beg);
		tempfile.write(buf1, TheFiles[tru1]->Length);
	}

	tempfile.write(buf, size); //write the file

	TFile.close(); //reload
   remove(arcname);
   rename("temp.tmp", arcname);

	Load(arcname);

	return;
}


int Archive::FindFile(char * myfile, char * mydir) {

	int tm = -1;
	for ( int tru = 0; tru < numfiles; tru++) {
		if ((strcmp(TheFiles[tru]->TheName, myfile) == 0) &&
				 (strcmp(TheFiles[tru]->DirectoryName, mydir) == 0)) {
			 tm = tru;
		 }
	}
	return tm;
}

UINT32 Archive::GetStart(char * myfile, char * mydir) {
	int num = FindFile(myfile, mydir);
	UINT32 st = TheFiles[num]->TheStart;
	return st;
}

void Archive::Extract (char * myfile, char * mydir, char * destfile) {
	ofstream fil;
	int num = FindFile(myfile, mydir);
	fil.open(destfile, ios::in | ios::binary);
	char * buf = new char [TheFiles[num]->Length];
	TFile. seekg(TheFiles[num]->TheStart, ios::beg);
	TFile.read(buf, TheFiles[num]->Length);
	fil.write(buf, TheFiles[num]->Length);
	fil.close();
	return ;
}


char * Archive::LoadToMemory (char * myfile, char * mydir) {
	int num = FindFile(myfile, mydir);
	char * buf = new char [TheFiles[num]->Length];
	TFile.seekg(TheFiles[num]->TheStart, ios::beg);
	TFile.read(buf, TheFiles[num]->Length);
	return buf;
}

char * Archive::DOSLoadToMemory (char * myfile, long &size) {
	ifstream fil;
	fil.open(myfile, ios::in | ios::binary);
  	fil.seekg(0, ios::end);
  	size = fil.tellg();
	char * buf = new char [size];
	TFile.read(buf, size);
	fil.close();
	return buf;
}


void Archive::Load(char *myfile) {

	strcpy(arcname, myfile);
	UINT32 c;
	char mybuf[4];

   TFile.open(myfile, ios::in | ios::out | ios::binary);

   TFile.seekg(0, ios::end);
   long size = TFile.tellg();
   //long thefour = 0;
   if (size < 4) {
      numfiles = 0;
      TFile.seekg(0, ios::beg);
      TFile.write((char *) &numfiles, sizeof(numfiles));
      return;
   }

   TFile.seekg(0, ios::beg);
   TFile.read(mybuf, 4);
   c = *(UINT32 *) &mybuf[0];

   if ( c == 0 ) {
		numfiles = 0;
      return;
 	}
	else {
		numfiles = c;
		TheFiles = new ArchiveFile * [numfiles];

		for ( int tru = 0; tru < numfiles; tru++) {
			TFile.read(TheFiles[tru]->TheName, 128);
			TFile.read(TheFiles[tru]->DirectoryName, 128);
			TFile.read(mybuf, 4);
			TheFiles[tru]->TheStart = *(UINT32 *) &mybuf[0];
			TFile.read(mybuf, 4);
			TheFiles[tru]->Length = *(UINT32 *) &mybuf[0];
		}
	}
	return ;
}




