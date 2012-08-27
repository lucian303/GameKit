#ifndef MOUSE_H
#define MOUSE_H

#include "..\h\gkit.h"

class Mouse {
	protected:
	int LeftButton, RightButton, CenterButton, x, y;
	UINT16 GetMouseButtonPressInfo(UINT16 Button, UINT16 * Counter, UINT16 * x, UINT16 * y);
	UINT16 GetMouseButtonReleaseInfo(UINT16 Button, UINT16 * Counter, UINT16 * x, UINT16 * y);
	UINT16 PollMouseStatus(UINT16 * x, UINT16 * y);

	public:

	Mouse();
	~Mouse();
	int Reset(void);
	void Show(void);
	void Hide(void);

	void SetPosition(UINT16 x, UINT16 y);
	void SetXLimit(UINT16 MinimumX, UINT16 MaximumX);
	void SetYLimit(UINT16 MinimumY, UINT16 MaximumY);
	void SetCursor(int HotSpotX, int HotSpotY, void far * BitmapPtr);
	void SetSensitivity(UINT16 MickeysPer8X, UINT16 MickeysPer8Y);
	void SetProtectedArea(UINT16 Left, UINT16 Top, UINT16 Right, UINT16 Bottom);
	void SetAccelerationThreshold(UINT16 MickeysPerSecond);

	UINT16 GetInfo(UINT16 * Type, UINT16 * Irq);

	void Update ();
	int GetX ();
	int GetY ();
	int GetCenterButton ();
	int GetLeftButton ();
	int GetRightButtonDown ();

};

extern Mouse * TheMouse;

#endif

