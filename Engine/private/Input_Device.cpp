#include "Input_Device.h"

CInput_Device::CInput_Device(void)
{
    ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
}

CInput_Device::~CInput_Device(void)
{

}

HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Update_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
	

	for (int32_t i = 0; i < ETOUI(DIMKEYINPUT::END); ++i)
	{
		m_bKeyPre[i] = m_bKeyCur[i];
	}
	m_bKeyCur[ETOUI(DIMKEYINPUT::TAB)] = (m_byKeyState[DIK_TAB] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::F5)] = (m_byKeyState[DIK_F5] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::Z)] = (m_byKeyState[DIK_Z] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::F6)] = (m_byKeyState[DIK_F6] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::F7)] = (m_byKeyState[DIK_F7] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::A)] = (m_byKeyState[DIK_A] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::S)] = (m_byKeyState[DIK_S] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::C)] = (m_byKeyState[DIK_C] & 0x80) ? true : false;

	m_bKeyCur[ETOUI(DIMKEYINPUT::F8)] = (m_byKeyState[DIK_F8] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::G)] = (m_byKeyState[DIK_G] & 0x80) ? true : false;

	m_bKeyCur[ETOUI(DIMKEYINPUT::F9)] = (m_byKeyState[DIK_F9] & 0x80) ? true : false;
	m_bKeyCur[ETOUI(DIMKEYINPUT::X)] = (m_byKeyState[DIK_X] & 0x80) ? true : false;

	for(int32_t i=0; i<3 ; ++i)
		m_bPre[i] = m_bCur[i];

	for (int32_t i = 0; i < 3; ++i)
		m_bCur[i] = (m_tMouseState.rgbButtons[i] & 0x80) ? true : false;

}

unique_ptr<CInput_Device> CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	auto	pInstance = unique_ptr<CInput_Device>(new CInput_Device());

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
		MSG_BOX("Failed to Created : CInput_Device");

	return pInstance;
}