#include "pch.h"
#include "Input.h"
#include "Engine.h"
#include <WindowsX.h>

void Input::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_hInst = (HINSTANCE)::GetWindowLongPtr(_hwnd, GWLP_HINSTANCE);
	_mouseStates.resize(3, KEY_STATE::NONE);
	_keyStates.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
	_frameCount = 0;
	_mousePos = { 0, 0 };
	_prevMousePos = { 0, 0 };
	_mouseDelta = { 0, 0 };

	// DirectInput 초기화
	HRESULT hr = S_OK;
	if (FAILED(hr = DirectInput8Create(_hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(&_directInput),
		nullptr)))
	{
		return;
	}

	// 키보드 초기화
	if (FAILED(hr = _directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr)))
		return;

	if (FAILED(hr = _keyboard->SetDataFormat(&c_dfDIKeyboard)))
		return;

	if (FAILED(hr = _keyboard->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return;

	if (FAILED(hr = _keyboard->Acquire()))
		return;

	// 마우스 초기화
	if (FAILED(hr = _directInput->CreateDevice(GUID_SysMouse, &_mouse, nullptr)))
		return;

	if (FAILED(hr = _mouse->SetDataFormat(&c_dfDIMouse)))
		return;

	if (FAILED(hr = _mouse->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return;

	if (FAILED(hr = _mouse->Acquire()))
		return;
}

void Input::Shutdown()
{
	if (_mouse)
	{
		_mouse->Unacquire();
		_mouse->Release();
		_mouse = nullptr;
	}

	if (_keyboard)
	{
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = nullptr;
	}

	if (_directInput)
	{
		_directInput->Release();
		_directInput = nullptr;
	}
}

void Input::Update()
{
	_frameCount++;

	if (_hwnd != ::GetActiveWindow())
	{
		for (auto& state : _keyStates)
			state = KEY_STATE::NONE;
		for (auto& state : _mouseStates)
			state = KEY_STATE::NONE;
		return;
	}

	HRESULT hr;
	if (_mouse == nullptr || _keyboard == nullptr) return;

	if (FAILED(hr = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), &_DIMouseState)))
	{
		while (_mouse->Acquire() == DIERR_INPUTLOST);
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (FAILED(hr = _keyboard->GetDeviceState(KEY_TYPE_COUNT, &asciiKeys)))
	{
		while (_keyboard->Acquire() == DIERR_INPUTLOST);
	}

	// 마우스 버튼 처리
	for (uint32 i = 0; i < 3; i++)
	{
		if (_DIMouseState.rgbButtons[i] & 0x80)
		{
			if (_mouseStates[i] == KEY_STATE::PRESS || _mouseStates[i] == KEY_STATE::DOWN)
				_mouseStates[i] = KEY_STATE::PRESS;
			else
				_mouseStates[i] = KEY_STATE::DOWN;
		}
		else
		{
			if (_mouseStates[i] == KEY_STATE::PRESS || _mouseStates[i] == KEY_STATE::DOWN)
				_mouseStates[i] = KEY_STATE::UP;
			else
				_mouseStates[i] = KEY_STATE::NONE;
		}
	}

	// 마우스 위치 및 델타
	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);
	_mouseDelta.x = _mousePos.x - _prevMousePos.x;
	_mouseDelta.y = _mousePos.y - _prevMousePos.y;
	_prevMousePos = _mousePos;

	// 키보드 키 처리
	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		if (asciiKeys[key] & 0x80)
		{
			if (_keyStates[key] == KEY_STATE::PRESS || _keyStates[key] == KEY_STATE::DOWN)
				_keyStates[key] = KEY_STATE::PRESS;
			else
				_keyStates[key] = KEY_STATE::DOWN;
		}
		else
		{
			if (_keyStates[key] == KEY_STATE::PRESS || _keyStates[key] == KEY_STATE::DOWN)
				_keyStates[key] = KEY_STATE::UP;
			else
				_keyStates[key] = KEY_STATE::NONE;
		}
	}
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
	return _keyStates[static_cast<uint8_t>(key)];
}

bool Input::GetButton(MOUSE_TYPE key)
{
	return GetState(key) == KEY_STATE::PRESS || GetState(key) == KEY_STATE::DOWN;
}

bool Input::GetButtonDown(MOUSE_TYPE key)
{
	return GetState(key) == KEY_STATE::DOWN;
}

bool Input::GetButtonUp(MOUSE_TYPE key)
{
	return GetState(key) == KEY_STATE::UP;
}

KEY_STATE Input::GetState(MOUSE_TYPE key) const
{
	return _mouseStates[static_cast<uint8_t>(key)];
}

POINT Input::GetMousePos() const
{
	return _mousePos;
}

POINT Input::GetMouseDelta() const
{
	return _mouseDelta;
}

KEY_STATE Input::GetMouseState(int index) const
{
	if (index >= 0 && index < _mouseStates.size())
		return _mouseStates[index];
	return KEY_STATE::NONE;
}