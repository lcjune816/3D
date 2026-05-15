#include "WorldObject.h"
#include "GameInstance.h"
#include "GuiObject.h"

CWorldObject::CWorldObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CWorldObject::CWorldObject(const CWorldObject& Prototye) :CGameObject(Prototye)
{
}
CWorldObject::~CWorldObject()
{
}

HRESULT CWorldObject::Initialize_Prototype()
{
	
	return S_OK;
}
HRESULT CWorldObject::Initialize(void* pArg)
{
	if (FAILED(Create_Component()))
		return E_FAIL;

	auto desc = static_cast<GAMEOBJECT_DESC*>(pArg);
	
	if (NULL_FALSE(desc->j) || desc->bCopy)
	{//로드할경우 또는 복사할경우
		if(NULL_FALSE(desc->j))
			Load_Data(desc, desc->j);
		
		static_cast<CTransform::TRANSFORM_DESC*>(pArg)->matWorld = desc->matWorld;
		static_cast<CTransform::TRANSFORM_DESC*>(pArg)->bWorldCheck = false;
	}
	else
		static_cast<CTransform::TRANSFORM_DESC*>(pArg)->bWorldCheck = true;
	
	if (NULL_TRUE(desc))
		return E_FAIL;
	
	//카메라 앞에 오브젝트 생성
	if (!desc->bFrontCamera)
		static_cast<CTransform::TRANSFORM_DESC*>(pArg)->matWorld = desc->matWorld;

	static_cast<CTransform::TRANSFORM_DESC*>(pArg)->m_fRotationPerSec = 0.f;
	static_cast<CTransform::TRANSFORM_DESC*>(pArg)->m_fSpeedPerSec = 0.f;

	if (FAILED(__super::Initialize(desc)))
		return E_FAIL;

//filesystem::path Name = desc->FileName;
//
//desc->FileName = "../../Resoruce/" + Name.filename().string();
//
//size_t i = desc->FileName.find_last_of(".");
//
//	desc->FileName.replace(desc->FileName.find_last_of("."), 4, ".bin");
	
	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile		  = m_PathName    = desc->FileName;
	importModel.bAllModel	  = m_iModeNumber = desc->iModeNumber;
	importModel.eType		  = m_eMeshType   = desc->eType;
	
	CGameInstance::Get().ImportModel_NonAnime(importModel, m_pTransform, m_MeshNameList);

	m_pBoxMesh= static_pointer_cast<CCube>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Cube", &pArg));
	
	if (NULL_TRUE(m_pBoxMesh))
		return E_FAIL;

	CGameInstance::Get().Add_LightMtrl(m_PathName);
	INSTANCING_DATA Data;
	XMStoreFloat4x4(&Data.matWorld,m_pTransform->Get_World());
	//CGameInstance::Get().Add_NaviMeshInfo(m_pTransform->Get_WorldPtr());
	//CGameInstance::Get().Add_Instancing_Data(m_MeshNameList, Data);
	return S_OK;
}
void CWorldObject::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(CWorldObject));
}
void CWorldObject::Update(_float fTimeDelta)
{
	_matrix mat = m_pTransform->Get_World();
	_float4 at{};
	XMStoreFloat4(&at, mat.r[3]);

	if (at.w == 0)
		int a = 0;
}
void CWorldObject::Late_Update(_float fTimeDelta)
{

}
HRESULT CWorldObject::Render()
{

	_float4x4 matWorld{};
	_float4 fColor = {};
	XMStoreFloat4x4(&matWorld, m_pTransform->Get_World());

	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

	if (m_bCopyLayerCheck)
		fColor = { 1.f,1.f,1.f,0.5f };
	else fColor = { 1.f,1.f,1.f,1.f };

	m_pShaderCom->Bind_RawValue("g_Color",&fColor, sizeof(_float4));

	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		auto Light = CGameInstance::Get().Find_LightMtrl(m_PathName).lock();
		if (pMesh == nullptr)
			continue;
	
		if (NULL_FALSE(Light))
			m_pShaderCom->Bind_RawValue("g_tagLight", Light.get(), sizeof(LIGHT_VALUE));

		pMesh->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
		

		m_pShaderCom->Begin(0);
		pMesh->Bind_Resource();
		pMesh->Render();
	}
	

	_matrix matW = m_pTransform->Get_World();


	if (!m_bBoxColor)
		return S_OK;

	_vector fScale = {};

	_float3 fMax = m_pTransform->Get_Max();
	_float3 fMin = m_pTransform->Get_Min();

	_vector vRight = matW.r[0];
	_vector vUp    = matW.r[1];
	_vector vLook  = matW.r[2];
	_vector vPos   = matW.r[3];
	fScale = (XMLoadFloat3(&fMax) - XMLoadFloat3(&fMin));

	_vector vPivotPos = ((XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f);

	_vector vRotPos = {};
	vRotPos = (XMVectorGetX(vPivotPos) * XMVector4Normalize(vRight)) +
		(XMVectorGetY(vPivotPos) * XMVector4Normalize(vUp)) +
		(XMVectorGetZ(vPivotPos) * XMVector4Normalize(vLook));

	//중심 다시 계산
	_vector vPosAll[8] = {};
	vPosAll[0] = { fMin.x , fMin.y  ,fMin.z ,1.f };
	vPosAll[1] = { fMax.x , fMin.y  ,fMin.z ,1.f };
	vPosAll[2] = { fMin.x , fMax.y  ,fMin.z ,1.f };
	vPosAll[3] = { fMax.x , fMax.y  ,fMin.z ,1.f };
	vPosAll[4] = { fMin.x , fMin.y  ,fMax.z ,1.f };
	vPosAll[5] = { fMax.x , fMin.y  ,fMax.z ,1.f };
	vPosAll[6] = { fMin.x , fMax.y  ,fMax.z ,1.f };
	vPosAll[7] = { fMax.x , fMax.y  ,fMax.z ,1.f };

	_float4 fCheckMax = { -FLT_MAX,-FLT_MAX ,-FLT_MAX ,1.f };
	_float4 fCheckMin = { FLT_MAX,FLT_MAX ,FLT_MAX ,1.f };

	for (uint32_t i = 0; i < 8; ++i)
	{
		_float3 fCheck;
		vPosAll[i] = XMVector3TransformCoord(vPosAll[i], matW);
		XMStoreFloat3(&fCheck, vPosAll[i]);

		fCheckMax.x = max(fCheckMax.x, fCheck.x);
		fCheckMax.y = max(fCheckMax.y, fCheck.y);
		fCheckMax.z = max(fCheckMax.z, fCheck.z);

		fCheckMin.x = min(fCheckMin.x, fCheck.x);
		fCheckMin.y = min(fCheckMin.y, fCheck.y);
		fCheckMin.z = min(fCheckMin.z, fCheck.z);

	}

	_vector vChck = (XMLoadFloat4(&fCheckMax) + XMLoadFloat4(&fCheckMin)) * 0.5f;
	vPos = vChck;

	vRight = XMVectorGetX(fScale * XMVector3Length(vRight)) * XMVector4Normalize(vRight);
	vUp = XMVectorGetY(fScale * XMVector3Length(vUp)) * 0.5f * XMVector4Normalize(vUp);
	vLook = XMVectorGetZ(fScale * XMVector3Length(vLook)) * XMVector4Normalize(vLook);

	_matrix World = { };
	World.r[0] = vRight;
	World.r[1] = vUp;
	World.r[2] = vLook;
	World.r[3] = vPos;


	XMStoreFloat4x4(&matWorld, World);
	 fColor = { 1.f,0.f,0.f,1.f };
	 _bool bCheck(true);
	m_pBoxShader->Bind_Matrix("g_World", &matWorld);
	m_pBoxShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pBoxShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));
	m_pBoxShader->Bind_RawValue("g_bChoice", &bCheck, sizeof bCheck);
	m_pBoxShader->Begin(0);

	m_pBoxMesh->Bind_Resource();
	m_pBoxMesh->Render();

	fColor = { 0.f,0.f,0.f,0.f };
	m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));
	return S_OK;
}


json CWorldObject::Save_Data()
{

	nlohmann::json j;

	_float4x4 fWorld = { };

	XMStoreFloat4x4(&fWorld, m_pTransform->Get_World());

	_float3 fRight = { fWorld._11,fWorld._12,fWorld._13 };
	_float3 fUp = { fWorld._21,fWorld._22 ,fWorld._23 };
	_float3 fLook = { fWorld._31, fWorld._32,fWorld._33 };
	_float3 fPos = { fWorld._41,fWorld._42,fWorld._43 };
	
	
	j["Right"]			  =  { fWorld._11,fWorld._12,fWorld._13 };
	j["Up"]				  =  { fWorld._21,fWorld._22 ,fWorld._23 };
	j["Look"]			  =  { fWorld._31, fWorld._32,fWorld._33 };
	j["fPos"]			  =  { fWorld._41,fWorld._42,fWorld._43 };
	j["Name"]			  = m_PathName;
	j["ModeNumber"]		  = m_iModeNumber;
	j["MeshType"]		  = m_eMeshType;
	return j;
}
HRESULT CWorldObject::Ready_Component()
{
	
 	return S_OK;
}
HRESULT CWorldObject::Create_Component()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_NonAnime"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"), TEXT("Com_BoxShader"), m_pBoxShader)))
		return E_FAIL;

	return S_OK;
}
void CWorldObject::Load_Data(void* pDesc ,const json& j)
{
	auto desc = static_cast<GAMEOBJECT_DESC*>(pDesc);

	desc->matWorld._11  = j["Right"][0];desc->matWorld._12 = j["Right"][1]; desc->matWorld._13 = j["Right"][2]; desc->matWorld._14 = 0;
	desc->matWorld._21  = j["Up"][0];   desc->matWorld._22 = j["Up"][1];    desc->matWorld._23 = j["Up"][2];    desc->matWorld._24 = 0;
	desc->matWorld._31  = j["Look"][0]; desc->matWorld._32 = j["Look"][1];  desc->matWorld._33 = j["Look"][2];  desc->matWorld._34 = 0;
	desc->matWorld._41  = j["fPos"][0]; desc->matWorld._42 = j["fPos"][1];  desc->matWorld._43 = j["fPos"][2];  desc->matWorld._44 = 1;
	m_PathName          =  j["Name"];
	int32_t iModelNumber = j["ModeNumber"];
	desc->FileName = m_PathName;
	desc->iModeNumber = iModelNumber;

}
unique_ptr<CWorldObject> CWorldObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CWorldObject>(new CWorldObject(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed WorldObject");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CWorldObject::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CWorldObject(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed WorldObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
