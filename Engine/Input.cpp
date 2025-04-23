#include "pch.h"
#include "Input.h"
#include <WindowsX.h>

void Input::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_keyInfos.resize(KEY_TYPE_COUNT);
}

void Input::Update()
{
	_frameCount++;

	if (_hwnd != ::GetActiveWindow())
	{
		for (auto& key : _keyInfos)
			key.state = KEY_STATE::NONE;
		return;
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == FALSE)
		return;

	for (uint32_t key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		bool isPressed = (asciiKeys[key] & 0x80) != 0;
		KeyInfo& info = _keyInfos[key];

		switch (info.state)
		{
		case KEY_STATE::NONE:
			if (isPressed)
			{
				info.state = KEY_STATE::DOWN;
				info.lastDownTick = _frameCount;
			}
			break;

		case KEY_STATE::DOWN:
		case KEY_STATE::PRESS:
			if (isPressed)
			{
				info.state = KEY_STATE::PRESS;
			}
			else
			{
				info.state = KEY_STATE::UP;
				info.lastUpTick = _frameCount;
			}
			break;

		case KEY_STATE::UP:
			if (info.lastUpTick == _frameCount - 1)
				info.state = KEY_STATE::NONE;
			break;
		}
	}

	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);
	_mouseDelta.x = _mousePos.x - _prevMousePos.x;
	_mouseDelta.y = _mousePos.y - _prevMousePos.y;
	_prevMousePos = _mousePos;
}

bool Input::GetButton(KEY_TYPE key)
{
	return GetState(key) == KEY_STATE::PRESS || GetState(key) == KEY_STATE::DOWN;
}

bool Input::GetButtonDown(KEY_TYPE key)
{
	return GetState(key) == KEY_STATE::DOWN;
}

bool Input::GetButtonUp(KEY_TYPE key)
{
	return GetState(key) == KEY_STATE::UP;
}

KEY_STATE Input::GetState(KEY_TYPE key) const
{
	return _keyInfos[static_cast<uint8_t>(key)].state;
}