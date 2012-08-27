//MOUSE.CPP
//Yep, mouse routines
#include <assert.h>
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "..\h\gkit.h"
#include "..\h\mouse.h"

/* button state masks */
#define CENTER_BUTTON_MASK	(0x04)
#define RIGHT_BUTTON_MASK	(0x02)
#define LEFT_BUTTON_MASK	(0x01)

/* constants */
#define MOUSE_INT (0x33)

/* mouse driver function codes */
#define FC_RESET_MOUSE				(0x0000)
#define FC_SHOW_CURSOR				(0x0001)
#define FC_HIDE_CURSOR				(0x0002)
#define FC_POLL_STATUS				(0x0003)
#define FC_SET_CURSOR_POSITION		(0x0004)
#define FC_GET_BUTTON_PRESS_INFO	(0x0005)
#define FC_GET_BUTTON_RELEASE_INFO	(0x0006)
#define FC_SET_X_LIMIT				(0x0007)
#define FC_SET_Y_LIMIT				(0x0008)
#define FC_SET_GRAPHICS_CURSOR		(0x0009)
#define FC_SET_TEXT_CURSOR			(0x000A)
#define FC_SET_EVENT_HANDLER		(0x000C)
#define FC_SET_SENSITIVITY			(0x000F)
#define FC_SET_PROTECTED_AREA		(0x0010)
#define FC_SET_ACCEL_THRESHOLD		(0x0013)
#define FC_GET_INFO					(0x0024)

Mouse * TheMouse = new Mouse;

Mouse::Mouse() {
	Reset();
	SetPosition(160, 100);
}

Mouse::~Mouse() {
}

void Mouse::Update () {
	UINT16 Buttons, tx, ty;
	Buttons = PollMouseStatus(&tx, &ty);
	x = tx >> 1; y = ty;
	LeftButton = (Buttons & LEFT_BUTTON_MASK) != 0;
	CenterButton = (Buttons & CENTER_BUTTON_MASK) != 0;
	RightButton = (Buttons & RIGHT_BUTTON_MASK) != 0;
}

int Mouse::GetX () {
	return x;
}

int Mouse::GetY () {
	return y;
}

int Mouse::GetCenterButton () {
	return CenterButton;
}


int Mouse::GetLeftButton () {
	return LeftButton;
}

int Mouse::GetRightButtonDown () {
	return RightButton;
}


/*
	Function: ResetMouse
	Description:
		Resets the mouse driver and returns the number of buttons the mouse
		driver supports (note: not necessarily the number of buttons on the
		mouse hardware itself!) or 0 if the driver is not loaded.
*/
int Mouse::Reset(void)
{
	union REGS regs;

    regs.x.eax = FC_RESET_MOUSE;

    int386(MOUSE_INT, &regs, &regs);

    /* if driver is present, return number of buttons from BX, else 0 */
    if (regs.x.eax == 0xFFFF) {
    	return regs.x.ebx;
    }
    else {
    	return 0;
    }
}

/*
	Function: ShowMouseCursor
	Description:
		Increments the mouse cursor visibility variable.  If the variable
		equals 0, the driver will display the cursor on screen.
*/
void Mouse::Show(void)
{
	union REGS regs;

    regs.x.eax = FC_SHOW_CURSOR;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: HideMouseCursor
	Description:
		Decrements the mouse visibility counter.  If the counter is less than
		0, the driver will hide the mouse cursor.
*/
void Mouse::Hide(void)
{
	union REGS regs;

    regs.x.eax = FC_HIDE_CURSOR;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: PollMouseStatus
	Description:
		Polls the current status of the mouse.  The current X and Y locations
		of the mouse are returned in the variables pointed to by the x and y
		arguments.  The status of the buttons is given by the function
		return value.  The status of the buttons may be tested by ANDing
		the return value with the CENTER_BUTTON_MASK,
		RIGHT_BUTTON_MASK, and LEFT_BUTTON_MASK constants.
*/
UINT16 Mouse::PollMouseStatus(UINT16 * x, UINT16 * y)
{
	union REGS regs;

    regs.x.eax = FC_POLL_STATUS;

    int386(MOUSE_INT, &regs, &regs);

    if (x != NULL) {
		*x = regs.x.ecx;
    }
    if (y != NULL) {
		*y = regs.x.edx;
    }
	return regs.x.ebx;
}

/*
	Function: SetMouseCursorPosition
	Description:
		Sets the current mouse cursor position to (x,y).
*/
void Mouse::SetPosition(UINT16 x, UINT16 y)
{
	union REGS regs;

    regs.x.eax = FC_SET_CURSOR_POSITION;
    regs.x.ecx = x;
    regs.x.edx = y;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: GetMouseButtonPressInfo
	Description:
		Returns information about the last press of the specified button,
		including how many times the button has been pressed since this
		function was last called, the X and Y location of the button
		when it was last pressed, and the current status of all the buttons.
		The button to test can be specified with the constants
		CENTER_BUTTON_MASK, RIGHT_BUTTON_MASK, and LEFT_BUTTON_MASK.
		The current button states are returned directly by the function.
*/
UINT16 Mouse::GetMouseButtonPressInfo
	(
    UINT16   Button,
    UINT16 * Counter,
    UINT16 * x,
    UINT16 * y
    )
{
	union REGS regs;

    regs.x.eax = FC_GET_BUTTON_PRESS_INFO;
    regs.x.ebx = Button;

    int386(MOUSE_INT, &regs, &regs);

    if (Counter != NULL) {
		*Counter = regs.x.ebx;
    }
    if (x != NULL) {
    	*x = regs.x.ecx;
    }
    if (y != NULL) {
		*y = regs.x.edx;
    }
    return regs.x.eax;
}

/*
	Function: GetMouseButtonReleaseInfo
	Description:
		Same as previous except for mouse button release instead of press.
*/
UINT16 Mouse::GetMouseButtonReleaseInfo
	(
    UINT16 Button,
    UINT16 * Counter,
    UINT16 * x,
    UINT16 * y
    )
{
	union REGS regs;

    regs.x.eax = FC_GET_BUTTON_RELEASE_INFO;
	regs.x.ebx = Button;

    int386(MOUSE_INT, &regs, &regs);

    if (Counter != NULL) {
		*Counter = regs.x.ebx;
    }
    if (x != NULL) {
    	*x = regs.x.ecx;
    }
    if (y != NULL) {
		*y = regs.x.edx;
    }
    return regs.x.eax;
}

/*
	Function: 	SetMouseXLimit
	Description:
		Restricts mouse movement between the minimum and maximum in the
		X direction.
*/
void Mouse::SetXLimit(UINT16 MinimumX, UINT16 MaximumX)
{
	union REGS regs;

    regs.x.eax = FC_SET_X_LIMIT;
    regs.x.ecx = MinimumX;
    regs.x.edx = MaximumX;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: 	SetMouseYLimit
	Description:
		Same as above, but in Y direction.
*/
void Mouse::SetYLimit(UINT16 MinimumY, UINT16 MaximumY)
{
	union REGS regs;

    regs.x.eax = FC_SET_Y_LIMIT;
    regs.x.ecx = MinimumY;
    regs.x.edx = MaximumY;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: SetMouseGraphicsCursor
	Description:
		Sets the graphics mode cursor to the image pointed to by the
		BitmapPtr argument.
*/
void Mouse::SetCursor
	(
    int HotSpotX,
    int HotSpotY,
    void far * BitmapPtr
    )
{
	union REGS regs;
	struct SREGS sregs;

    assert((-16 <= HotSpotX) && (HotSpotX <= 16));
    assert(-16 <= HotSpotY && HotSpotY <= 16);

    regs.x.eax = FC_SET_GRAPHICS_CURSOR;
    regs.x.ebx = HotSpotX;
    regs.x.ecx = HotSpotY;
    regs.x.edx = FP_OFF(BitmapPtr);
    sregs.es = FP_SEG(BitmapPtr);

    int386x(MOUSE_INT, &regs, &regs, &sregs);
}

/*
	Function: SetMouseSensitivity
	Description:
		Sets the mouse sensitivity factors.  Sensitivity in each direction is
		given as the number of mickeys necessary to move the cursor eight
		mouse coordinates in either the X or Y direction.
*/
void Mouse::SetSensitivity
	(
    UINT16 MickeysPer8X,
    UINT16 MickeysPer8Y
    )
{
	union REGS regs;

    regs.x.eax = FC_SET_SENSITIVITY;
    regs.x.ecx = MickeysPer8X;
    regs.x.edx = MickeysPer8Y;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: SetMouseProtectedArea
	Description:
		Sets a rectangular area protected from the mouse.
*/
void Mouse::SetProtectedArea
	(
    UINT16 Left,
    UINT16 Top,
    UINT16 Right,
    UINT16 Bottom
    )
{
	union REGS regs;

    regs.x.eax = FC_SET_PROTECTED_AREA;
    regs.x.ecx = Left;
    regs.x.edx = Top;
    regs.x.esi = Right;
    regs.x.edi = Bottom;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: SetMouseAccelerationThreshold
	Description:
		Sets the mouse acceleration threshold to be MickeysPerSecond.
*/
void Mouse::SetAccelerationThreshold(UINT16 MickeysPerSecond)
{
	union REGS regs;

    regs.x.eax = FC_SET_ACCEL_THRESHOLD;
    regs.x.edx = MickeysPerSecond;

    int386(MOUSE_INT, &regs, &regs);
}

/*
	Function: GetMouseInfo
	Description:
		Returns info about the mouse driver.  Directly returns the version
		number of the mouse driver.  Returns the mouse type and IRQ via
		the pointers passed as arguments.
*/
UINT16 Mouse::GetInfo(UINT16 * Type, UINT16 * Irq)
{
	union REGS regs;

    regs.x.eax = FC_GET_INFO;

    int386(MOUSE_INT, &regs, &regs);

    if (Type != NULL) {
		*Type = regs.h.ch;
    }
    if (Irq != NULL) {
    	*Irq  = regs.h.cl;
    }

    return regs.x.ebx;
}


