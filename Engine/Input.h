#pragma once
#include <Windows.h>

enum class KEY_TYPE
{
	UP = VK_UP, DOWN = VK_DOWN, LEFT = VK_LEFT, RIGHT = VK_RIGHT,
	PAGEUP = VK_PRIOR, PAGEDOWN = VK_NEXT,
	A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H',
	I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N', O = 'O', P = 'P',
	Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U', V = 'V', W = 'W', X = 'X',
	Y = 'Y', Z = 'Z',
	ADD = VK_ADD, SUBTRACT = VK_SUBTRACT, TAB = VK_TAB,
	KEY_1 = '1', KEY_2 = '2', KEY_3 = '3', KEY_4 = '4',
	LBUTTON = VK_LBUTTON, RBUTTON = VK_RBUTTON,
	SPACE = VK_SPACE,
};

enum class KEY_STATE
{
	NONE, PRESS, DOWN, UP, END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32_t>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32_t>(KEY_STATE::END),
};

struct KeyInfo
{
	KEY_STATE state = KEY_STATE::NONE;
	int64_t lastDownTick = -1;
	int64_t lastUpTick = -1;
};

class Input
{
	DECLARE_SINGLE(Input);

public:
	void Init(HWND hwnd);
	void Update();

	bool GetButton(KEY_TYPE key);
	bool GetButtonDown(KEY_TYPE key);
	bool GetButtonUp(KEY_TYPE key);

	const POINT& GetMousePos() const { return _mousePos; }
	POINT GetMouseDelta() const { return _mouseDelta; }

private:
	KEY_STATE GetState(KEY_TYPE key) const;
private:
	HWND _hwnd = nullptr;
	std::vector<KeyInfo> _keyInfos;

	POINT _mousePos = {};
	POINT _prevMousePos = {};
	POINT _mouseDelta = {};

	int64_t _frameCount = 0;
};
