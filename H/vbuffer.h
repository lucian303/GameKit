//Vbuffer.h
//header for virtual buffer classes
#ifndef VBUFFER_H
#define VBUFFER_H

#include "..\h\gkit.h"

class VirtualBuffer {
	protected:

	UINT32 size;

	public:
	char * Memory;

	VirtualBuffer();
	~VirtualBuffer();
	void ReAllocate();
	void CopyToScreen();
	void GetFromScreen();
	void Clear();
	void Clear (int value);
	int GetSize();
};

#endif
