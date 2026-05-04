#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Level_Loading.h"
#include "Loader.h"
CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_GamePlay::~CLevel_GamePlay()
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Decal(L"Layer_Decal")))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_GameObject")))
		return E_FAIL;

	if (FAILED(Ready_Layer_WorldObject(L"Layer_WorldObject")))
		return E_FAIL;

	if (FAILED(Ready_ProtoType()))
		return E_FAIL;



	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if ((CGameInstance::Get().Get_DIKeyState(DIK_Q) & 0x80) && (CGameInstance::Get().Get_DIKeyState(DIK_F1) & 0x80))
	{
		CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING), 
				CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GASZONE));
	}
	uint32_t iData = 10;
}

HRESULT CLevel_GamePlay::Render()
{


	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera::CAMERA_DESC		FreeDesc{};
	FreeDesc.vEye = _float4{ 0.f,10.f,-5.f,1.f };
	FreeDesc.vAt = _float4{ 0.f,0.f,0.f,1.f };
	FreeDesc.fFovy = XMConvertToRadians(60.f);
	FreeDesc.fNear = 0.1f;
	FreeDesc.fFar = 1000.f;
	FreeDesc.m_fSpeedPerSec = 2.f;
	
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY),
		TEXT("OBJ_Camera"), ETOUI(LEVEL::GAMEPLAY), strLayerTag, &FreeDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Player"),
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Decal(const _wstring& strLayerTag)
{

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_BackGround"),
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Terrian"),
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_WorldObject(const _wstring& strLayerTag)
{

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_TriggerObject(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}
HRESULT CLevel_GamePlay::Ready_ProtoType()
{
	return S_OK;
}
unique_ptr<CLevel_GamePlay> CLevel_GamePlay::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CLevel_GamePlay>(new CLevel_GamePlay(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CLevel_GamePlay"), nullptr, MB_OK);

	return pInstance;
}
