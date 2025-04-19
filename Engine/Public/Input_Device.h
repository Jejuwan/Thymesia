#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum KEYSTATE { KEY_NONE,KEY_DOWN, KEY_PRESSING, KEY_UP, KEY_END };

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	void Tick();

	_byte Get_DIKeyState(_ubyte uKey)
	{
		return m_byKeyState[uKey];
	}
	_byte Get_DIMouseState(MOUSEKEYSTATE eMouseID)
	{
		return m_MouseState.rgbButtons[eMouseID];
	}

	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID)
	{
		return *(((_long*)&m_MouseState) + eMouseMoveID);
	}

	_bool Mouse_Down(MOUSEKEYSTATE eMouseID)
	{
		if (m_MouseKey[eMouseID].second == KEY_DOWN)return true;
		return false;
	}

	_bool Mouse_Pressing(MOUSEKEYSTATE eMouseID)
	{
		if (m_MouseKey[eMouseID].second == KEY_PRESSING)return true;
		return false;
	}

	_bool Mouse_Up(MOUSEKEYSTATE eMouseID)
	{
		if (m_MouseKey[eMouseID].second == KEY_UP)return true;
		return false;
	}

	_bool Keyboard_Down(_int iKey)
	{
		if (m_KeyboardKey[iKey].second == KEY_DOWN)return true;
		return false;
	}

	_bool Keyboard_Pressing(_int iKey)
	{
		if (m_KeyboardKey[iKey].second == KEY_PRESSING)return true;
		return false;
	}

	_bool Keyboard_Up(_int iKey)
	{
		if (m_KeyboardKey[iKey].second == KEY_UP)return true;
		return false;
	}

public:
	HRESULT			Ready_DInput(HINSTANCE hInst, HWND hWnd);
	void			Update_DInput(void);

private:
	LPDIRECTINPUT8				m_pInputSDK = { nullptr };

	LPDIRECTINPUTDEVICE8		m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8		m_pMouse = { nullptr };
	LPDIRECTINPUTDEVICE8		m_pJoypad = { nullptr };

private:
	/* 키보드의 모든 키에 대한 입력 상태를 저장한다. */
	_byte					m_byKeyState[256];

	/* 마우스의 모든 키에 대한 입력 상태를 저장한다. */
	 DIMOUSESTATE			m_MouseState;

	 pair<_bool, KEYSTATE> m_MouseKey[5];
	 pair<_bool, KEYSTATE> m_KeyboardKey[255];

public:
	virtual void		Free(void);

};

END