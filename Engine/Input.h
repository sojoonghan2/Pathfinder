#pragma once
#include <Windows.h>
#include <dinput.h>
#include <vector>
#include <cstdint>

#pragma once
#include <dinput.h>

enum class MOUSE_TYPE
{
	LBUTTON = 0,  // DIMOUSESTATE.rgbButtons[0]
	MBUTTON = 2,  // DIMOUSESTATE.rgbButtons[2]
	RBUTTON = 1,  // DIMOUSESTATE.rgbButtons[1]
};

enum class KEY_TYPE
{
	// 방향키
	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,

	// 알파벳
	A = DIK_A,
	B = DIK_B,
	C = DIK_C,
	D = DIK_D,
	E = DIK_E,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	I = DIK_I,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,
	M = DIK_M,
	N = DIK_N,
	O = DIK_O,
	P = DIK_P,
	Q = DIK_Q,
	R = DIK_R,
	S = DIK_S,
	T = DIK_T,
	U = DIK_U,
	V = DIK_V,
	W = DIK_W,
	X = DIK_X,
	Y = DIK_Y,
	Z = DIK_Z,

	// 숫자 키
	KEY_1 = DIK_1,
	KEY_2 = DIK_2,
	KEY_3 = DIK_3,
	KEY_4 = DIK_4,
	KEY_5 = DIK_5,
	KEY_6 = DIK_6,
	KEY_7 = DIK_7,
	KEY_8 = DIK_8,
	KEY_9 = DIK_9,
	KEY_0 = DIK_0,

	// 기능키
	TAB = DIK_TAB,
	SPACE = DIK_SPACE,
	ENTER = DIK_RETURN,
	ESC = DIK_ESCAPE,
	SHIFT = DIK_LSHIFT,
	CTRL = DIK_LCONTROL,
	ALT = DIK_LALT,
	CAPSLOCK = DIK_CAPITAL,

	// 특수 키
	PAGEUP = DIK_PRIOR,
	PAGEDOWN = DIK_NEXT,
	INSERT = DIK_INSERT,
	DELETEKEY = DIK_DELETE,
	HOME = DIK_HOME,
	END = DIK_END,

	// 키패드
	ADD = DIK_NUMPADPLUS,
	SUBTRACT = DIK_NUMPADMINUS,
	MULTIPLY = DIK_MULTIPLY,
	DIVIDE = DIK_DIVIDE,

	// F1 ~ F12
	F1 = DIK_F1,
	F2 = DIK_F2,
	F3 = DIK_F3,
	F4 = DIK_F4,
	F5 = DIK_F5,
	F6 = DIK_F6,
	F7 = DIK_F7,
	F8 = DIK_F8,
	F9 = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32_t>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32_t>(KEY_STATE::END),
};

class Input
{
	DECLARE_SINGLE(Input);

public:
	void Init(HWND hwnd);
	void Shutdown();
	void Update();

	bool GetButton(KEY_TYPE key);
	bool GetButtonDown(KEY_TYPE key);
	bool GetButtonUp(KEY_TYPE key);
	KEY_STATE GetState(KEY_TYPE key) const;

	bool GetButton(MOUSE_TYPE key);
	bool GetButtonDown(MOUSE_TYPE key);
	bool GetButtonUp(MOUSE_TYPE key);
	KEY_STATE GetState(MOUSE_TYPE key) const;

	POINT GetMousePos() const;
	POINT GetMouseDelta() const;
	KEY_STATE GetMouseState(int index) const;

private:
	HWND _hwnd = nullptr;
	HINSTANCE _hInst = nullptr;

	LPDIRECTINPUT8 _directInput = nullptr;
	LPDIRECTINPUTDEVICE8 _keyboard = nullptr;
	LPDIRECTINPUTDEVICE8 _mouse = nullptr;

	DIMOUSESTATE _DIMouseState = {};

	std::vector<KEY_STATE> _keyStates;
	std::vector<KEY_STATE> _mouseStates;

	POINT _mousePos = {};
	POINT _prevMousePos = {};
	POINT _mouseDelta = {};

	int64_t _frameCount = 0;

private:
	bool _lockCursor = true;
	POINT _centerScreenPos = { 0, 0 };
	bool _initializedCenter = false;

public:
	void LockCursor(bool enable);
	bool IsCursorLocked() const;

};