//Mode13h.h

#include "..\h\gkit.h"

#ifndef MODE13H_H
#define MODE13H_H

#define VGASTARTADDR (0xA0000)

#define TEXT_MODE_NUMBER (0x03)

#define VGA_BIOS_INT	(0x10)	/* VGA BIOS interrupt */

#define MODE13H_WIDTH	(320)	/* width of mode 013h */
#define MODE13H_HEIGHT	(200)	/* height of mode 013h */

/* General Registers */
#define MISC_OUTPUT_REG	(0x3C2)	/* Write port for Misc. Output Register */
#define MISC_OUTPUT_REG_READ (0x3CC) /* Read port for Misc. Output Reg. */

#define FEATURE_CNTRL_REG (0x3DA) /* Write port for Feature Control Reg. */
#define FEATURE_CNTRL_REG_READ (0x3CA) /* Read port for FC Reg. */

#define INPUT_STAT_0_REG (0x3C2) /* Input Status Reg. #0 */
#define INPUT_STAT_1_REG (0x3DA) /* Input Status Reg. #1 */

/* Sequencer Registers */
#define SEQ_INDEX_REG	(0x3C4)	/* Sequencer Index Register */
#define SEQ_DATA_REG	(0x3C5)	/* Sequencer Data Register */

#define RESET_INDEX					(0)
#define CLOCKING_MODE_INDEX			(1)
#define MAP_MASK_INDEX				(2)
#define CHARACTER_MAP_SELECT_INDEX	(3)
#define MEMORY_MODE_INDEX			(4)

/* CRTC Registers */
#define CRTC_INDEX_REG	(0x3D4)	/* CRTC Index Register */
#define CRTC_DATA_REG	(0x3D5)	/* CRTC Data Register */

#define HORIZ_TOTAL_INDEX			(0x00)
#define HORIZ_DISPLAY_END_INDEX		(0x01)
#define START_HORIZ_BLANK_INDEX		(0x02)
#define END_HORIZ_BLANK_INDEX		(0x03)
#define START_HORIZ_RETRACE_INDEX	(0x04)
#define END_HORIZ_RETRACE_INDEX		(0x05)
#define VERT_TOTAL_INDEX			(0x06)
#define OVERFLOW_INDEX				(0x07)
#define PRESET_ROW_SCAN_INDEX		(0x08)
#define MAX_SCAN_LINE_INDEX			(0x09)
#define CURSOR_START_INDEX			(0x0A)
#define CURSOR_END_INDEX			(0x0B)
#define START_ADDRESS_HIGH_INDEX	(0x0C)
#define START_ADDRESS_LOW_INDEX		(0x0D)
#define CURSOR_LOCATION_HIGH_INDEX	(0x0E)
#define CURSOR_LOCATION_LOW_INDEX	(0x0F)
#define VERT_RETRACE_START_INDEX	(0x10)
#define VERT_RETRACE_END_INDEX		(0x11)
#define VERT_DISPLAY_END_INDEX		(0x12)
#define OFFSET_INDEX				(0x13)
#define UNDERLINE_LOCATION_INDEX	(0x14)
#define START_VERT_BLANK_INDEX		(0x15)
#define END_VERT_BLANK_INDEX		(0x16)
#define MODE_CONTROL_INDEX			(0x17)
#define LINE_COMPARE_INDEX			(0x18)


/* Graphics Control Registers */
#define GC_INDEX_REG	(0x3CE)	/* Graphics Control Index Register */
#define GC_DATA_REG		(0x3CF)	/* Graphics Control Data Register */

#define SET_RESET_INDEX				(0)
#define SET_RESET_ENABLE_INDEX		(1)
#define COLOR_COMPARE_INDEX			(2)
#define DATA_ROTATE_INDEX			(3)
#define READ_MAP_SELECT_INDEX		(4)
#define MODE_INDEX					(5)
#define MISC_INDEX					(6)
#define COLOR_DONT_CARE_INDEX		(7)
#define BIT_MASK_INDEX				(8)

/* Color Registers */
#define COLOR_ADDRESS_WRITE	(0x3C8)
#define COLOR_ADDRESS_READ	(0x3C7)
#define COLOR_DATA			(0x3C9)

/* Macros for setting frequently used VGA registers directly */
#define SetBMR(val)	outport(GC_INDEX_REG, BIT_MASK_INDEX | (val << 8))
#define SetSRR(val)	outport(GC_INDEX_REG, SET_RESET_INDEX | (val << 8))
#define SetSRER(val) outport(GC_INDEX_REG,\
						SET_RESET_ENABLE_INDEX | (val <<8))
#define SetMMR(val)	outport(SEQ_INDEX_REG, MAP_MASK_INDEX | (val << 8))


extern char * VGAMEMORY;
extern UINT16	ScreenWidth;
extern UINT16	ScreenHeight;

class Mode13h {

	protected:
	void SetVGAMode(UINT16 Mode);
	BOOL VerticalRetraceOccuring(void);
	void SetVGAReg(UINT16 IndexReg, UINT8 Index, UINT16 DataReg, UINT8 Data);

	public:
	void SetMode13h();
	void SetTextMode();
	void Clear();
	void Clear(int value);

	void WaitVerticalRetraceStart(void);
	void WaitVerticalRetraceEnd(void);
	void WaitDisplayMode(void);
	void WaitRetraceMode(void);

	BOOL DetectVGA(void);
	Mode13h();
	~Mode13h();

};

extern Mode13h * TheScreen;

#endif


