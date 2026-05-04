#include "Level_GasProduction.h"
#include "GameInstance.h"
#include "Camera.h"
CLevel_GasProduction::CLevel_GasProduction(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_GasProduction::~CLevel_GasProduction()
{
}

HRESULT CLevel_GasProduction::Initialize()
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

void CLevel_GasProduction::Update(_float fTimeDelta)
{
	uint32_t iData = 10;
}

HRESULT CLevel_GasProduction::Render()
{


	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera::CAMERA_DESC		FreeDesc{};
	FreeDesc.vEye = _float4{ 0.f,10.f,-5.f,1.f };
	FreeDesc.vAt = _float4{ 0.f,0.f,0.f,1.f };
	FreeDesc.fFovy = XMConvertToRadians(60.f);
	FreeDesc.fNear = 0.1f;
	FreeDesc.fFar = 1000.f;
	FreeDesc.m_fSpeedPerSec = 10.f;
	
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE),
		TEXT("OBJ_Camera"), ETOUI(LEVEL::GASZONE), strLayerTag, &FreeDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Player"),
		ETOUI(LEVEL::GASZONE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_Decal(const _wstring& strLayerTag)
{

	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_BackGround"),
		ETOUI(LEVEL::GASZONE), strLayerTag)))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Terrian"),
		ETOUI(LEVEL::GASZONE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GasProduction::Ready_Layer_WorldObject(const _wstring& strLayerTag)
{

	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_TriggerObject(const _wstring& strLayerTag)
{
	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_ProtoType()
{

	return S_OK;
}
unique_ptr<CLevel_GasProduction> CLevel_GasProduction::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CLevel_GasProduction>(new CLevel_GasProduction(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CLevel_GasProduction"), nullptr, MB_OK);

	return pInstance;
}
