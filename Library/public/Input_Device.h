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
	int8_t Get_DIMouseState(DIMK eMouse)
	{
		return m_tMouseState.rgbButtons[ETOUI(eMouse)];
	}
	
	int8_t Get_DIMouseOneClick(DIMK eMouse)
	{
		if (m_bCur[ETOUI(eMouse)] && !m_bPre[ETOUI(eMouse)])
		{
			m_bPre[ETOUI(eMouse)] = true;
			return true;
		}
			
		return false;

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

	_bool			m_bCur[3]{ false };
	_bool			m_bPre[3]{ false };
public:
	static unique_ptr<CInput_Device> Create(HINSTANCE hInst, HWND hWnd);
};

NS_END

