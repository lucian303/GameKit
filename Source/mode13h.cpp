//Mode13h.cpp
//mode13h class for hndling video mode specific funcs
#include <iostream.h>
#include <dos.h>
#include <conio.h>
#include <string.h>
#include "..\h\gkit.h"
#include "..\h\mode13h.h"

char * VGAMEMORY = (char *)  VGASTARTADDR;
UINT16	ScreenWidth = 0;
UINT16	ScreenHeight = 0;
Mode13h * TheScreen = new Mode13h;

//constructor
Mode13h::Mode13h () {
	BOOL det = DetectVGA();
	if ( det != TRUE)
	{
		//cout << "VGA not DETECTED!!!";
		return;
	}
	//SetMode13h();
	//ClearScreen();

}

//destructor
Mode13h::~Mode13h () {
	SetTextMode();
}

#define VR_BIT (0x08)
#define DISPLAY_ENABLE (0x01)

/*
	Function: WaitVerticalRetraceStart
    Description:
    	Waits for the VGA to begin vertical retrace.  Note that if the
        VGA is already in vertical retrace, the routine will wait all
        during the next display frame until vertical retrace begins
        again.
*/
void Mode13h::WaitVerticalRetraceStart(void)
{
	UINT8 InputStatus1;

    /* make sure we aren't in retrace already */
    do {
    	InputStatus1 = inp(INPUT_STAT_1_REG);
    } while (InputStatus1 & VR_BIT);

    /* wait for retrace to start */
    do {
    	InputStatus1 = inp(INPUT_STAT_1_REG);
    } while (!(InputStatus1 & VR_BIT));
}

/*
	Function: WaitVerticalRetraceEnd
    Description:
    	Waits for vertical retrace to complete.  If the VGA is not
        in vertical retrace, it waits until the VGA enters vertical
        retrace again and the retrace subsequently completes.
*/
void Mode13h::WaitVerticalRetraceEnd(void)
{
	UINT8 InputStatus1;

    /* make sure we aren't in display state (= not retrace) */
    do {
    	InputStatus1 = inp(INPUT_STAT_1_REG);
    } while (!(InputStatus1 & VR_BIT));

    /* wait for retrace to end */
    do {
    	InputStatus1 = inp(INPUT_STAT_1_REG);
    } while (InputStatus1 & VR_BIT);
}

/*
	Function: WaitDisplayMode
    Description:
    	If the VGA is currently in display mode, the function returns
        immediately, else it waits until diplay mode is entered.
*/
void Mode13h::WaitDisplayMode(void)
{
	UINT8 InputStatus1;

    /* wait for retrace to end and display mode to start */
    do {
    	InputStatus1 = inp(INPUT_STAT_1_REG);
    } while (InputStatus1 & DISPLAY_ENABLE);
}

/*
	Function: WaitRetraceMode
    Description:
    	If the VGA is currently in retrace mode, the function returns
        immediately, else it waits until retrace mode is entered.
*/
void Mode13h::WaitRetraceMode(void)
{
	UINT8 InputStatus1;

    /* wait for display mode to end and retrace to start */
    do {
    	InputStatus1 = inp(INPUT_STAT_1_REG);
    } while (!(InputStatus1 & VR_BIT));
}

/*
	Function: VerticalRetraceOccuring
    Description:
    	Returns the current state of the VGA vertical retrace.  If
        retrace is occuring, the function returns TRUE.
*/
BOOL Mode13h::VerticalRetraceOccuring(void)
{
	return (inp(INPUT_STAT_1_REG) & VR_BIT) >> 3;
}


/*
	Function: SetVGAReg
    Description:
    	Sets a VGA register, identified by the IndexReg, DataReg, and
        Index parameters to the value specified by the Data parameter.
*/
void Mode13h::SetVGAReg(UINT16 IndexReg, UINT8 Index, UINT16 DataReg, UINT8 Data)
{
	outp(IndexReg, Index);
   outp(DataReg, Data);
}

/*
	Function: SetVGAMode
    Description:
    	Uses the video BIOS to set the VGA video mode.
*/
void Mode13h::SetVGAMode(UINT16 Mode)
{
   union REGS regs;

   regs.x.eax = Mode;

   int386(VGA_BIOS_INT, &regs, &regs);
}

/*
	Function: SetMode13h
    Description:
    	Puts the VGA into Mode 13h and sets the global variables
        describing the screen state appropriately.
*/
void Mode13h::SetMode13h(void)
{
	cout << "\n" << "Game Kit Version 0.5 BETA\n" << "Copyright 1996 Lucian Hontau\n" <<
		"Unregistered Version\n";
	SetVGAMode(0x13);

	ScreenWidth			= MODE13H_WIDTH;
	ScreenHeight			= MODE13H_HEIGHT;
	//GScreenVirtualWidth		= MODE13H_WIDTH;
	//GVGAMode				= 0x13;
	Clear();
}

void Mode13h::SetTextMode(void)
{
	SetVGAMode(TEXT_MODE_NUMBER);

	ScreenWidth			= MODE13H_WIDTH;
	ScreenHeight			= MODE13H_HEIGHT;
	//GScreenVirtualWidth		= MODE13H_WIDTH;
	//GVGAMode				= 0x13;
}


/*
	Function: DetectVGA
    Description:
    	Returns TRUE if a VGA is detected, FALSE otherwise.
*/
BOOL Mode13h::DetectVGA(void)
{
	union REGS regs;

    /* try a BIOS call only the VGA understands */
    regs.x.ebx = 0xFFFF;
    regs.x.eax = 0x101A;

    int386(VGA_BIOS_INT, &regs, &regs);

    /* if the card didn't understand, it's not a VGA */
    if (regs.x.ebx == 0xFFFF) {
    	return FALSE;
    }
    else {
    	return TRUE;
    }
}

void Mode13h::Clear() {
	memset( VGAMEMORY, 0, 64000L);
}


void Mode13h::Clear(int value) {
	memset( VGAMEMORY, value, 64000L);
}
