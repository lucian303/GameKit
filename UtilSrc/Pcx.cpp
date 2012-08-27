/* File: PCX.C
** Description:
**   Routines to load, encode, and decode PCX format files.
** Copyright:
**   Copyright 1994, David G. Roberts
*/

#include <malloc.h>
#include <assert.h>
#include <dos.h>
#include <stdio.h>
#include "..\h\gkit.h"
#include "..\utilsrc\pcx2vbm.h"
#include "..\utilsrc\pcx.h"

/*
	Function: DecodePCXLine
    Description:
    	A module local function that decodes a lines-worth of data
        from a PCX file.
*/
static int DecodePCXLine
	(
	UINT8  * LineBuffer,
	unsigned TotalBytes,
	FILE * File
	)
{
	unsigned Position;
    int Count;
    UINT8 Color;
    int RawData;
    int i;

    Position = 0;

    while (Position < TotalBytes) {
    	RawData = fgetc(File);
        if (RawData == EOF) {
        	return -1;
        }
        if ((RawData & 0xC0) == 0xC0) {
        	Count = RawData & 0x3F;
            RawData = fgetc(File);
            if (RawData == EOF) {
            	return -1;
            }
            Color = RawData;
        }
        else {
        	Count = 1;
            Color = RawData;
        }
        for (i = 0; i < Count; i++) {
        	LineBuffer[Position++] = Color;
        }
    }

    return 0;
}

/*
	Function: EncodeRun
    Description:
    	Encodes a run length and a data value and writes them to
        a PCX file.  This routine has been adapted from the ZSoft
        PCX documentation.  The routine returns a non-zero value
        in the event of an error (typically disk full).
*/
static int EncodeRun(int Val, int RunLength, FILE * File)
{
	if (RunLength) {
    	if ((RunLength == 1) && (0xC0 != (0xC0 & Val))) {
        	/* run of 1 with color index < 0xC0 so no encoding */
            if (EOF == fputc(Val, File)) {
            	return 1;
            }
        }
        else {
        	if (EOF == fputc(0xC0 | RunLength, File)) {
            	return 1;
            }
            if (EOF == fputc(Val, File)) {
            	return 1;
            }
        }
	}
    return 0;
}

/*
	Function: EncodePCXLine
    Description:
    	Takes a line of byte data and run-length encodes it in PCX
        format.  This routine has been adapted from the ZSoft PCX
        documentation.  The routine returns a non-zero value in
        the event of an error (typically disk full).
*/
static int EncodePCXLine
	(
	UINT8  * LineBuffer,
	unsigned Length,
	FILE * File
	)
{
	int OldVal;
    int NewVal;
    int Count;
    unsigned LengthCounter;

    /* start us off */
    OldVal = *LineBuffer++;
    LengthCounter = 1;
    Count = 1;

    while (LengthCounter < Length) {
    	NewVal = *LineBuffer++;
        if (NewVal == OldVal) {
        	/* we've found a run */
        	Count++;
            if (Count == 63) {
            	/* this run is at least 63 long, so save off what we */
                /* have so far */
            	if (EncodeRun(OldVal, Count, File)) {
                	return 1; /* error */
                }
                Count = 0;
            }
        }
        else {
        	/* no run */
        	if (Count) {
            	/* save off the run we were working on */
            	if (EncodeRun(OldVal, Count, File)) {
                	return 1;
                }
            }
            OldVal = NewVal;
            Count = 1;
        }
        LengthCounter++;
    }

    /* save off last run */
    if (Count) {
    	if (EncodeRun(OldVal, Count, File)) {
        	return 1;
        }
    }

	return 0;
}

/*
	Function: LoadPCX
    Description:
    	Loads a PCX file into memory and converts it to LINEAR_BITMAP
        format.  The palette data for the PCX file is ignored.
*/
LINEAR_BITMAP  * LoadPCX(char * FileName, UINT8 Palette[256][3])
{
	FILE * File;
    UINT8  * LineBuffer;
    LINEAR_BITMAP  * LinearBM;
    UINT8  * LinearDataPointer;
    PCX_HEADER PCXHeader;
    unsigned LineCounter;
    unsigned WidthCounter;
    int Result;
    unsigned XSize;
    unsigned YSize;
    unsigned TotalBytes;
    int ColorIndex;

    assert(FileName != NULL);

    /* read in header */
    File = fopen(FileName, "rb");
    if (File == NULL) {
    	return NULL;
    }
    Result = fread(&PCXHeader, 1, sizeof(PCXHeader), File);
    if (Result != sizeof(PCXHeader)) {
    	fclose(File);
    	return NULL;
    }

    /* compute size of image and scanline size */
    XSize = PCXHeader.XMax - PCXHeader.XMin + 1;
    YSize = PCXHeader.YMax - PCXHeader.YMin + 1;
    TotalBytes = PCXHeader.NPlanes * PCXHeader.BytesPerLine;

    /* allocate scanline buffer */
    LineBuffer = (UINT8  *) malloc(TotalBytes);
    if (LineBuffer == NULL) {
    	fclose(File);
    	return NULL;
    }

    /* allocate and fill in LINEAR_BITMAP data structure */
    LinearBM = (LINEAR_BITMAP  *) malloc(sizeof(LINEAR_BITMAP) +
    	XSize * YSize - 1);
    if (LinearBM == NULL) {
    	fclose(File);
    	free(LineBuffer);
        return NULL;
    }

    LinearBM->Width = XSize;
    LinearBM->Height = YSize;
    LinearBM->OriginX = 0;
    LinearBM->OriginY = 0;

    /* decode data */
    LinearDataPointer = (UINT8  *) &(LinearBM->Data);

    for (LineCounter = 0; LineCounter < YSize; LineCounter++) {
    	Result = DecodePCXLine(LineBuffer, TotalBytes, File);
        if (Result != 0) {
        	free(LineBuffer);
            free(LinearBM);
            return NULL;
        }
        for (WidthCounter = 0; WidthCounter < XSize; WidthCounter++) {
        	*LinearDataPointer++ = LineBuffer[PCXHeader.XMin + WidthCounter];
        }
    }

    free(LineBuffer);

    /* if the caller doesn't want palette information, just return */
    if (Palette == NULL) {
    	fclose(File);
        return LinearBM;
    }

    /* get the palette */
    if (PCXHeader.Version != 5) {
	    /* if the PCX file doesn't support the 256-color palette */
        /* use the 16-color palette instead */
    	for (ColorIndex = 0; ColorIndex < 16; ColorIndex++) {
            Palette[ColorIndex][0] = PCXHeader.ColorMap[ColorIndex][0] / 4;
            Palette[ColorIndex][1] = PCXHeader.ColorMap[ColorIndex][1] / 4;
            Palette[ColorIndex][2] = PCXHeader.ColorMap[ColorIndex][2] / 4;
        }
        for (ColorIndex = 16; ColorIndex < 256; ColorIndex++) {
            Palette[ColorIndex][0] = 0;
            Palette[ColorIndex][1] = 0;
            Palette[ColorIndex][2] = 0;
        }
    }
    else {
    	fseek(File, -((256 * 3) + 1), SEEK_END);
        Result = fgetc(File);
        if (Result != 12) {
        	/* this doesn't have a 256-color palette, */
            /* so use the 16-color one */
	    	for (ColorIndex = 0; ColorIndex < 16; ColorIndex++) {
            	/* copy entries and divide by four since PCX colors */
                /* can range from 0 to 255 and VGA colors can only */
                /* go as high as 63 */
    	        Palette[ColorIndex][0] =
					PCXHeader.ColorMap[ColorIndex][0] / 4;
        	    Palette[ColorIndex][1] =
					PCXHeader.ColorMap[ColorIndex][1] / 4;
            	Palette[ColorIndex][2] =
					PCXHeader.ColorMap[ColorIndex][2] / 4;
        	}
	        for (ColorIndex = 16; ColorIndex < 256; ColorIndex++) {
    	        Palette[ColorIndex][0] = 0;
    	        Palette[ColorIndex][1] = 0;
    	        Palette[ColorIndex][2] = 0;
    	    }
        }
        else {
        	/* read in the palette in one chunk */
        	fread(Palette, sizeof(UINT8), 256 * 3, File);
            /* now divide the RGB entries down so they range */
            /* from 0 to 63 */
            for (ColorIndex = 0; ColorIndex < 256; ColorIndex++) {
            	Palette[ColorIndex][0] /= 4;
            	Palette[ColorIndex][1] /= 4;
            	Palette[ColorIndex][2] /= 4;
            }
        }
    }

    /* clean up a bit */
    fclose(File);

    return LinearBM;
}

/*
	Function: SavePCX
    Description:
    	Take a linear bitmap and a palette as input and saves the
        linear bitmap as a PCX file with the specified name.
        The bitmap will be saved as a version 5 format PCX file with
        a palette.
*/
int SavePCX
	(
	char * Filename,
	LINEAR_BITMAP  * Bitmap,
	UINT8 Palette[256][3]
	)
{
	PCX_HEADER PCXHeader;
    FILE * File;
    int ColorIndex;
    int i;
    int Result;
    unsigned LineCounter;
    UINT8  * Data;
    int PalData;

    assert(Filename != NULL);
    assert(Bitmap != NULL);
    assert(Palette != NULL);

    /* fill in PCX header */
    PCXHeader.Manufacturer 	= 10;
    PCXHeader.Version		= 5;
    PCXHeader.Encoding		= 1;
    PCXHeader.BitsPerPixel	= 8;
    PCXHeader.XMin			= 0;
    PCXHeader.YMin			= 0;
	PCXHeader.XMax			= Bitmap->Width - 1;
    PCXHeader.YMax			= Bitmap->Height - 1;
    PCXHeader.HDpi          = Bitmap->Width;
    PCXHeader.VDpi			= Bitmap->Height;
    PCXHeader.Reserved		= 0;
    PCXHeader.NPlanes		= 1;
    PCXHeader.BytesPerLine	= (Bitmap->Width + 1) & 0xFFFE; /* make even */
    PCXHeader.PaletteInfo	= 1;
    PCXHeader.HScreenSize	= Bitmap->Width;
    PCXHeader.VScreenSize	= Bitmap->Height;
    for (ColorIndex = 0; ColorIndex < 16; ColorIndex++) {
	    PCXHeader.ColorMap[ColorIndex][0] = Palette[ColorIndex][0] * 4;
	    PCXHeader.ColorMap[ColorIndex][1] = Palette[ColorIndex][1] * 4;
	    PCXHeader.ColorMap[ColorIndex][2] = Palette[ColorIndex][2] * 4;
    }
    for (i = 0; i < sizeof(PCXHeader.Pad); i++) {
    	PCXHeader.Pad[i] = 0;
    }

    /* open file */
    File = fopen(Filename, "wb");
    if (File == NULL) {
    	return 1; /* error */
    }

    /* write header to file */
    Result = fwrite(&PCXHeader, sizeof(PCX_HEADER), 1, File);
    if (Result != 1) {
    	/* write error */
    	fclose(File);
        return 1;
    }

    /* encode data */
    Data = (UINT8  *) &(Bitmap->Data);
    for (LineCounter = 0; LineCounter < Bitmap->Height; LineCounter++) {
    	if (EncodePCXLine(Data, Bitmap->Width, File)) {
        	fclose(File);
        	return 1; /* file write error */
		}
        /* handle odd width since BytesPerLine must be even */
        if (Bitmap->Width & 1) {
        	if (fputc(0, File) != 0) {
            	fclose(File);
            	return 1; /* file write error */
            }
        }
        Data += Bitmap->Width;
    }

    /* write palette */
    fputc(12, File);	/* palette marker */
    for (ColorIndex = 0; ColorIndex < 256; ColorIndex++) {
    	PalData = Palette[ColorIndex][0] * 4;
        Result = fputc(PalData, File);
        if (Result != PalData) {
        	fclose(File);
        	return 1; /* write error */
        }
    	PalData = Palette[ColorIndex][1] * 4;
        Result = fputc(PalData, File);
        if (Result != PalData) {
        	fclose(File);
        	return 1; /* write error */
        }
    	PalData = Palette[ColorIndex][2] * 4;
        Result = fputc(PalData, File);
        if (Result != PalData) {
        	fclose(File);
        	return 1; /* write error */
        }
    }

    /* close and exit */
    fclose(File);
    return 0;
}


