#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Player.h"
#include "Level_Loading.h"
#include "NaviObject.h"
#include "Loader_Defines.h"
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
	if (FAILED(Ready_Lights()))
		return E_FAIL;
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
	
	if(FAILED(Ready_Layer_WorldObjectInstance(L"Layer_GameObject")))
		return E_FAIL;

	if(FAILED(Ready_Layer_Gui(L"Layer_Gui")))
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
HRESULT CLevel_GamePlay::Ready_Lights()
{
	//LIGHT_DESC			LightDesc{};
	//
	//LightDesc.eType = LIGHT::DIRECTIONAL;
	//LightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(CGameInstance::Get().Add_Light(LightDesc)))
	//	return E_FAIL;
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	auto pObj = static_cast<CPlayer*>(CGameInstance::Get().Get_ObjectPtr(ETOUI(LEVEL::GAMEPLAY), L"Layer_Player", "Player"));
 	if (NULL_TRUE(pObj))
		return E_FAIL;
	uint32_t index = pObj->GetAnimator()->Find_Key("JNT_Camera");
	_float4x4 Matrix = pObj->GetAnimator()->Find_Matrix(index);

	CCamera::FREECAM_DESC FreeDesc{};

	FreeDesc.ParentsMatrix = pObj->Get_Transform().lock()->Get_WorldPtr();
	XMStoreFloat4x4(&FreeDesc.CamBoneMatrix, XMLoadFloat4x4(&Matrix));

	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY),
		TEXT("OBJ_Camera"), ETOUI(LEVEL::GAMEPLAY), L"Layer_Camera", &FreeDesc)))
		return E_FAIL;
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC objDesc;
	objDesc.iLevel = ETOUI(LEVEL::GAMEPLAY);
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Player"),
		ETOUI(LEVEL::GAMEPLAY), strLayerTag,&objDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Boss(const _wstring& strLayerTag)
{
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Teacher"),
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

	CGameObject::GAMEOBJECT_DESC objDesc;
	objDesc.iLevel = ETOUI(LEVEL::GAMEPLAY);
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Terrian"),
		ETOUI(LEVEL::GAMEPLAY), strLayerTag,&objDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_WorldObject(const _wstring& strLayerTag)
{
	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_NaviMesh"),
	//	CNaviMesh::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Navi"),
	//	CNaviObject::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	//	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::STATIC), TEXT("OBJ_Navi"),
	//		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//	
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_TriggerObject(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}
HRESULT CLevel_GamePlay::Ready_Layer_WorldObjectInstance(const _wstring& strLayerTag)
{
	while (true)
	{
		CInstance_WorldObject::INSTANCING_WORLDOBJECT_DESC InstanceData;
	
 		if (false == CGameInstance::Get().Create_Instancing_Desc(InstanceData.InstancingData))
			return S_OK;
		
			if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Instancing_WorldObject"),
				ETOUI(LEVEL::GAMEPLAY), strLayerTag, &InstanceData)))
				return E_FAIL;
	
	}
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_ProtoType()
{
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Gui(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC objDesc;
	objDesc.iLevel = ETOUI(LEVEL::GAMEPLAY);
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::STATIC), TEXT("OBJ_Gui"),
		ETOUI(LEVEL::GAMEPLAY), strLayerTag, &objDesc)))
		return E_FAIL;

	return S_OK;
}
unique_ptr<CLevel_GamePlay> CLevel_GamePlay::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CLevel_GamePlay>(new CLevel_GamePlay(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CLevel_GamePlay"), nullptr, MB_OK);

	return pInstance;
}
