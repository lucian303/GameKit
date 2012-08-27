//Clip.cpp
//functions to set the clipping rectangle
#include "..\h\gkit.h"
#include "..\h\clip.h"

void ClippingRectangle::Set (UINT32 mx1, UINT32 my1, UINT32 mx2, UINT32 my2) {
	x1 = mx1; y1 = my1; x2 = mx2; y2 = my2;
}

ClippingRectangle::ClippingRectangle () {
	x1 = 0; y1 = 0; x2 = 319; y2 = 199;
}

ClippingRectangle::ClippingRectangle (UINT32 mx1a, UINT32 my1a, UINT32 mx2a, UINT32 my2a) {
	Set (mx1a, my1a, mx2a, my2a);
}

ClippingRectangle * ScreenClip = new ClippingRectangle;

