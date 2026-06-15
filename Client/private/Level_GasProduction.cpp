#include "Level_GasProduction.h"
#include "GameInstance.h"
#include "Loader_Defines.h"
#include "Camera.h"
#include "Player.h"
CLevel_GasProduction::CLevel_GasProduction(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_GasProduction::~CLevel_GasProduction()
{
}

HRESULT CLevel_GasProduction::Initialize()
{

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Decal(L"Layer_Decal")))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_GameObject")))
		return E_FAIL;

	if (FAILED(Ready_Layer_WorldObject(L"Layer_WorldObject")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Boss(L"Layer_Boss")))
		return E_FAIL;

	if (FAILED(Ready_ProtoType()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Gui(L"Layer_Gui")))
		return E_FAIL;

	if (FAILED(Ready_Partilce()))
		return E_FAIL;
	S_OK;
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
 	auto pObj = static_cast<CPlayer*>(CGameInstance::Get().Get_ObjectPtr(ETOUI(LEVEL::GASZONE), L"Layer_Player", "Player"));
	uint32_t index = pObj->GetAnimator()->Find_Key("JNT_Camera");
	_float4x4 Matrix = pObj->GetAnimator()->Find_Matrix(index);

	CCamera::FREECAM_DESC FreeDesc{};

	FreeDesc.ParentsMatrix = pObj->Get_Transform().lock()->Get_WorldPtr();
	XMStoreFloat4x4(&FreeDesc.CamBoneMatrix, XMLoadFloat4x4(&Matrix));

	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE),
		TEXT("OBJ_Camera"), ETOUI(LEVEL::GASZONE), L"Layer_Camera", &FreeDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC objDesc;
	objDesc.iLevel = ETOUI(LEVEL::GASZONE);

	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Player"),
		ETOUI(LEVEL::GASZONE), strLayerTag,&objDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_Boss(const _wstring& strLayerTag)
{
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Cat"),
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

	CGameObject::GAMEOBJECT_DESC objDesc;
	objDesc.iLevel = ETOUI(LEVEL::GASZONE);
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Terrian"),
		ETOUI(LEVEL::GASZONE), strLayerTag,&objDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GasProduction::Ready_Layer_WorldObject(const _wstring& strLayerTag)
{
	while (true)
	{
		CInstance_WorldObject::INSTANCING_WORLDOBJECT_DESC InstanceData;
	
		if (false == CGameInstance::Get().Create_Instancing_Desc(InstanceData.InstancingData))
			return S_OK;
	
		if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Instancing_WorldObject"),
			ETOUI(LEVEL::GASZONE), strLayerTag, &InstanceData)))
			return E_FAIL;
	
	}
	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_TriggerObject(const _wstring& strLayerTag)
{
	return S_OK;
}
HRESULT CLevel_GasProduction::Ready_Layer_Gui(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC objDesc;
	objDesc.iLevel = ETOUI(LEVEL::GASZONE);
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::STATIC), TEXT("OBJ_Gui"),
		ETOUI(LEVEL::GASZONE), strLayerTag, &objDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GasProduction::Ready_Partilce()
{
	CWorldParticle::WORLDPARTICLE_DESC Desc{};
	Desc.iLevel = ETOUI(LEVEL::GASZONE);
	Desc.eParticleType = WORLD_EVENT::BOSS_SPAWN;
	Desc.eParticleEmit = PARTICLE::FOG_CONTROLLER;
	CGameInstance::Get().Add_ParticleToPool(L"OBJ_Particle", ETOUI(LEVEL::GASZONE), ETOUI(LEVEL::GASZONE), &Desc);

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
