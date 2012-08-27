//Pcx2vbm.h
//converts pcx to virtual color bitmaps

#ifndef PCX2VBM_H
#define PCX2VBM_H

#include "..\h\gkit.h"

typedef struct {
	UINT16 Width;
    UINT16 Height;
    int OriginX;
    int OriginY;
    UINT8 Data;
} LINEAR_BITMAP;

#endif
