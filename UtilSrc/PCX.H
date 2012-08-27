/* File: PCX.H
** Description:
**   Header file for PCX.C.
** Copyright:
**   Copyright 1994, David G. Roberts
*/

#ifndef _PCX_H

#include <stdio.h>
#include "..\h\gkit.h"
#include "..\utilsrc\pcx2vbm.h"

typedef struct {
	UINT8	Manufacturer;
    UINT8	Version;
    UINT8	Encoding;
    UINT8	BitsPerPixel;
    UINT16	XMin;
    UINT16	YMin;
    UINT16	XMax;
    UINT16	YMax;
    UINT16	HDpi;
    UINT16	VDpi;
    UINT8	ColorMap[16][3];
    UINT8	Reserved;
    UINT8	NPlanes;
    UINT16	BytesPerLine;
    UINT16	PaletteInfo;
    UINT16	HScreenSize;
    UINT16	VScreenSize;
    UINT8	Pad[54];
} PCX_HEADER;

LINEAR_BITMAP * LoadPCX(char * FileName, UINT8 Palette[256][3]);
int SavePCX
	(
	char * Filename,
	LINEAR_BITMAP * Bitmap,
	UINT8 Palette[256][3]
	);

#define _PCX_H

#endif
