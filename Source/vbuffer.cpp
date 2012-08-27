//VBUFFER.CPP
//Virtual buffer Classes and Functions
#include <string.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"
#include "..\h\vbuffer.h"

VirtualBuffer::VirtualBuffer () {
	Memory = NULL;//just a precaution
	size = 64000L;
	Memory = new char [size];
	Clear();
}

VirtualBuffer::~VirtualBuffer () {
	if ( Memory) {
		delete Memory;
	}
}

void VirtualBuffer::ReAllocate () {
	if ( Memory) {
		delete Memory;
	}
	Memory = NULL;
	size = 64000L;
	Memory = new char [size];
	Clear();
}

void VirtualBuffer::CopyToScreen() {
	memcpy (VGAMEMORY, Memory, size);
}

void VirtualBuffer::GetFromScreen() {
	memcpy (Memory, VGAMEMORY, size);
}

void VirtualBuffer::Clear() {
	memset( Memory, 0, size);
}

void VirtualBuffer::Clear(int value) {
	memset( Memory, value, size);
}

int VirtualBuffer::GetSize() {
	return  size;
}



