#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CInput_Device final
{
private:
	CInput_Device(void);
public:
	~CInput_Device(void);

public:
	int8_t	Get_DIKeyState(uint8_t byKeyID)
	{
		return m_byKeyState[byKeyID];
	}
	int8_t Get_DIKeyOneState(DIMKEYINPUT byKeyID)
	{
		if (m_bKeyCur[ETOUI(byKeyID)] && !m_bKeyPre[ETOUI(byKeyID)])
		{
			return true;
		}

		return false;

	}
	int8_t Get_DIMouseState(DIMK eMouse)
	{
		return m_tMouseState.rgbButtons[ETOUI(eMouse)];
	}
	
	int8_t Get_DIMouseOneClick(DIMK eMouse, ENGINE_MOUSE eState = ENGINE_MOUSE::HOLD )
	{
		switch (eState)
		{
		case ENGINE_MOUSE::A_CLICK:
			return m_bCur[ETOUI(eMouse)] && !m_bPre[ETOUI(eMouse)];

		case ENGINE_MOUSE::B_CLICK:
			return !m_bCur[ETOUI(eMouse)] && m_bPre[ETOUI(eMouse)];
		case ENGINE_MOUSE::HOLD:
			return m_bCur[ETOUI(eMouse)] && m_bPre[ETOUI(eMouse)];

		}

	}
	int32_t Get_DIMouseMove(DIMM eMouseState)
	{
		return *((reinterpret_cast<int32_t*>(&m_tMouseState)) + ETOUI(eMouseState));
	}

public:
	HRESULT		Initialize(HINSTANCE hInst, HWND hWnd);
	void		Update_InputDev(void);
private:
	ComPtr<IDirectInput8>		m_pInputSDK = { nullptr };
private:
	ComPtr<IDirectInputDevice8>		m_pKeyBoard = { nullptr };
	ComPtr<IDirectInputDevice8>		m_pMouse = { nullptr };

private:
	int8_t			m_byKeyState[256];
	DIMOUSESTATE	m_tMouseState;

	_bool			m_bKeyCur[ETOUI(DIMKEYINPUT::END)];
	_bool			m_bKeyPre[ETOUI(DIMKEYINPUT::END)];

	_bool			m_bCur[3]{ false };
	_bool			m_bPre[3]{ false };
public:
	static unique_ptr<CInput_Device> Create(HINSTANCE hInst, HWND hWnd);
};

NS_END

