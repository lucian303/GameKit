#ifndef CLIP_H
#define CLIP_H

#include "..\h\gkit.h"

class ClippingRectangle {
	public:
	UINT32 x1, y1, x2, y2;

	void Set (UINT32 mx1, UINT32 my1, UINT32 mx2, UINT32 my2);
	ClippingRectangle ();
	ClippingRectangle (UINT32 mx1a, UINT32 my1a, UINT32 mx2a, UINT32 my2a);
	~ClippingRectangle () {};

};
extern ClippingRectangle * ScreenClip;
#endif
