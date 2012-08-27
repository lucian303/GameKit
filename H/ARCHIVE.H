#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "..\h\gkit.h"
#include <fstream.h>

class ArchiveFile {

	friend class Archive;

	protected:
	//char typ;
	char DirectoryName[128]; //to classify the file
	char TheName[128];
	UINT32 TheStart;
	UINT32 Length;

};

class Archive {

	protected:
	char * arcname;
	UINT32 numfiles;
	ArchiveFile **TheFiles;
    fstream TFile;

	int FindFile(char * myfile, char * mydir);

	public:
	Archive();//
	~Archive();//

	void Load (char * myfile);//
	//void Save (char * myfile);

	void Add (char * myfile, char * mydir, char * dosfile);
	void Delete (char * myfile, char * mydir);
	void Extract (char * myfile, char * mydir, char * destfile);//
	UINT32 GetStart (char * myfile, char * mydir);//
	char * LoadToMemory (char * myfile, char * mydir);//
	char * DOSLoadToMemory (char * myfile, long &size);
	//char ** GetDirectories ();
	//ArchiveFile ** GetFiles (char * mydir);

};

#endif
