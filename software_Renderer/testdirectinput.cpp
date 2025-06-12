#include <iostream>
#include <windows.h>
#include <functional>
#include <assert.h>

#include "testdirectinput.h"

#define Assert(formula, message) assert(formula && message)
#define Assert(message) assert(0 && message)

#define KEYDOWN(name, key) ((name[key] & 0x80) ? true : false)
#define KEYUP(name, key) ((name[key] & 0x80) ? false : true)

InputSystem::InputSystem()
	:_input(nullptr), _keyboard(nullptr), _mouse(nullptr)
	/*,_screenHeight(0), _screenWidth(0)*/, _mouseX(0), _mouseY(0)
	, _mouseWheelData(0), _prevmouseWheelData(0)
	, _mouseState(), _cKey()
{

}

InputSystem::~InputSystem()
{

}

InputSystem* InputSystem::GetInstance()
{
	static InputSystem* instance;

	if (instance == nullptr)
	{
		instance = new InputSystem();
	}
	return instance;
}

bool InputSystem::Initialize(HINSTANCE instance, HWND hwnd, int screenHeight, int screenWidth)
{
	//_screenHeight = screenHeight;
	//_screenWidth = screenWidth;
	_hWnd = hwnd;

	// 만약 이 함수에 문제가 발생했다면 3단계로 확인 할것.
	// Assert로 인해 메세지로 나타나는 함수의 위치를 확인 하세요.
	// 함수의 위치의 hr 내용을 확인 하세요.
	// 해당 hr 내용을 기반으로 문제에 접근하면 비교적 쉬울겁니다.
	HRESULT hr;

	if (FAILED(hr = DirectInput8Create(instance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&_input, nullptr)))
	{
		Assert("FAILED DirectInput8Create");
		return false;
	}

	if (FAILED(hr = _input->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr)))
	{
		Assert("FAILED CreateDevice");
		return false;
	}


	if (FAILED(hr = _keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		Assert("FAILED SetCooperativeLevel");
		return false;
	}

	if (FAILED(hr = _keyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		Assert("FAILED SetDataFormat");
		return false;
	}

	if (FAILED(hr = _keyboard->Acquire()))
	{
		Assert("FAILED Acquire");
		return false;
	}
	////////////////////// 여기까진 키보드 초기화 //////////////////////

	if (FAILED(hr = _input->CreateDevice(GUID_SysMouse, &_mouse, NULL)))
	{
		Assert("FAILED CreateDevice(Mouse)");
		return false;
	}

	if (FAILED(hr = _mouse->SetDataFormat(&c_dfDIMouse)))
	{
		Assert("FAILED SetDataFormat(Mouse)");
		return false;
	}

	if (FAILED(hr = _mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		Assert("FAILED SetCooperativeLevel(Mouse)");
		return false;
	}

	if (FAILED(hr = _mouse->Acquire()))
	{
		Assert("FAILED Acquire(Mouse)");
		return false;
	}
	////////////////////// 여기까지 마우스 초기화 //////////////////////

	return true;
}

void InputSystem::Finalize()
{
	// 키보드 해제
	if (_mouse)
	{
		_mouse->Unacquire();
		_mouse->Release();
		_mouse = 0;
	}

	// 키보드 해제
	if (_keyboard)
	{
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = 0;
	}

	// DirectInput8 해제
	if (_input)
	{
		_input->Release();
		_input = 0;
	}
}

// 이 함수에서 하는 일은 디바이스와의 연결을 확인하고 연결이 끊어졌다면 다시 연결을 시도합니다.
// 윈도우창이 내려가거나 최상위에 존재하지 못하면 디바이스로스트 상태가 됩니다.
// ASSERT 주석을 해제하고 디버그 할 경우 중단점에 걸릴때마다 Assert에 걸리므로
// 입력관련 디버깅이 목적이 아니라면 주석을 풀지 마세요.
void InputSystem::Update(float fTime)
{
	HRESULT hr;

	if (!GetFocus())
	{
		return;
	}

	// 디바이스의 상태를 확인하는 함수.
	// 1번인자 = 2번인자 파라미터의 버퍼 바이트 사이즈
	// 2번인자 = 장치의 현재상태를 얻어오는 구조체의 주소
	if (FAILED(hr = (_keyboard->GetDeviceState(sizeof(_keybuffer), (LPVOID)&_keybuffer))))
	{
		//Assert("FALID GetDeviceState");

		if (FAILED(_keyboard->Acquire()))
		{
			//Assert("FAILED Acquire");
		}
	}

	_mouse->Poll();

	if (FAILED(hr = (_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState))))
	{
		//Assert("FALID GetDeviceState (Mouse)");

		if (FAILED(_mouse->Acquire()))
		{
			//Assert("FAILED Acquire (Mouse)");
		}
	}

	_prevmouseWheelData = _mouseWheelData;

	// 프레임마다 마우스의 위치를 갱신
	_mouseX += _mouseState.lX;
	_mouseY += _mouseState.lY;
	_mouseWheelData += _mouseState.lZ;
}

bool WINAPI InputSystem::KeyInput(KEY keycode)
{
	int key = (int)keycode;

	if (KEYDOWN(_keybuffer, key))
	{
		return true;
	}

	return false;
}

bool WINAPI InputSystem::KeyUp(KEY keycode)
{
	int key = (int)keycode;

	if (KEYUP(_keybuffer, key) && KEYDOWN(_prevkeybuffer, key))
	{
		_prevkeybuffer[key] = _keybuffer[key];
		return true;
	}

	else if (KEYDOWN(_keybuffer, key) && KEYUP(_prevkeybuffer, key))
	{
		return false;
	}

	else
	{
		_prevkeybuffer[key] = _keybuffer[key];
		return false;
	}
}

bool WINAPI InputSystem::KeyDown(KEY keycode)
{
	int key = (int)keycode;

	if (KEYDOWN(_keybuffer, key) && KEYUP(_prevkeybuffer, key))
	{
		_prevkeybuffer[key] = _keybuffer[key];
		return true;
	}

	else if (KEYUP(_keybuffer, key) && KEYDOWN(_prevkeybuffer, key))
	{
		return false;
	}

	else
	{
		_prevkeybuffer[key] = _keybuffer[key];
		return false;
	}
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

bool __stdcall InputSystem::MouseButtonInput(int button)
{

	if (KEYDOWN(_mouseState.rgbButtons, button))
	{
		return true;
	}

	return false;
}

bool __stdcall InputSystem::MouseButtonUp(int button)
{
	if (KEYUP(_mouseState.rgbButtons, button) && KEYDOWN(_prevrgbButtons, button))
	{
		_prevrgbButtons[button] = _mouseState.rgbButtons[button];
		return true;
	}

	else if (KEYDOWN(_mouseState.rgbButtons, button) && KEYUP(_prevrgbButtons, button))
	{
		return false;
	}

	else
	{
		_prevrgbButtons[button] = _mouseState.rgbButtons[button];
		return false;
	}
}

bool __stdcall InputSystem::MouseButtonDown(int button)
{
	if (KEYDOWN(_mouseState.rgbButtons, button) && KEYUP(_prevrgbButtons, button))
	{
		_prevrgbButtons[button] = _mouseState.rgbButtons[button];
		return true;
	}

	else if (KEYUP(_mouseState.rgbButtons, button) && KEYDOWN(_prevrgbButtons, button))
	{
		return false;
	}

	else
	{
		_prevrgbButtons[button] = _mouseState.rgbButtons[button];
		return false;
	}
}

bool __stdcall InputSystem::WheelUp()
{
	if (_mouseWheelData > _prevmouseWheelData)
	{
		return true;
	}

	return false;
}

bool __stdcall InputSystem::WheelDown()
{

	if (_mouseWheelData < _prevmouseWheelData)
	{
		return true;
	}

	return false;
}

void InputSystem::GetMousePosition(int& xpos, int& ypos)
{
	xpos = _mouseX;
	ypos = _mouseY;
}

void InputSystem::GetWheelState(int& curState, int& prevState)
{
	curState = _mouseWheelData;
	prevState = _prevmouseWheelData;
}

int InputSystem::GetWheelState()
{
	return _mouseWheelData;
}
