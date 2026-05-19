#include "TriggerObject.h"
#include "GameInstance.h"
#include "GuiObject.h"
#include "Layer.h"
CTriggerObject::CTriggerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CTriggerObject::CTriggerObject(const CTriggerObject& Prototye) :CGameObject(Prototye)
{
}
CTriggerObject::~CTriggerObject()
{
}

HRESULT CTriggerObject::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CTriggerObject::Initialize(void* pArg)
{
	auto desc = static_cast<GAMEOBJECT_DESC*>(pArg);
	m_TriggerInfo.bOtherTrigger = desc->bTrigger;
	static_cast<CTrigger::TRIGGER_DESC*>(pArg)->bTrigger = m_TriggerInfo.bOtherTrigger;
	 m_TriggerInfo.strTriggerName = desc->strTriggerName; //트리거 밸류용 문자저장

	static_cast<CTrigger::TRIGGER_DESC*>(pArg)->eRot			= m_TriggerInfo.eRot = desc->eRot;
	static_cast<CTrigger::TRIGGER_DESC*>(pArg)->fArrrowRotation = m_TriggerInfo.fArrrowRotation = desc->fArrrowRotation;
	static_cast<CTrigger::TRIGGER_DESC*>(pArg)->fFrameTickTime  = m_TriggerInfo.fFrameTickTime = desc->fFrameTickTime;
	static_cast<CTrigger::TRIGGER_DESC*>(pArg)->fMaxFrameTime   = m_TriggerInfo.fMaxFrameTime = desc->fMaxFrameTime;
	
	if (FAILED(Create_Component(pArg)))
		return E_FAIL;

	if (NULL_FALSE(desc->j) || desc->bCopy)
	{//로드할경우
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
	//desc->FileName.replace(desc->FileName.find_last_of("."), 4, ".bin");

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = m_PathName = desc->FileName;
	importModel.bAllModel = m_TriggerInfo.iModeNumber = desc->iModeNumber;
	importModel.eType = m_eMeshType = desc->eType;


	CGameInstance::Get().ImportModel_NonAnime(importModel,m_pTransform, m_MeshNameList);

	m_pBoxMesh = static_pointer_cast<CCube>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Cube", &pArg));

	if (NULL_TRUE(m_pBoxMesh))
		return E_FAIL;

	if (m_TriggerInfo.iObjectID == 0)
	{
		//오브젝트 ID 안겹치게 하는건데 처음부터 순회해서 찾아서함
		uint32_t iObjectCnt = 0;
		while (true)
		{
			++iObjectCnt;
			if (0 == CGameInstance::Get().Add_Trigger(iObjectCnt, m_pTrigger))
				break;
		}
		m_TriggerInfo.iObjectID = iObjectCnt;
	}
	else CGameInstance::Get().Add_Trigger(m_TriggerInfo.iObjectID, m_pTrigger);

	m_pTrigger->Set_TargetNumber(m_TriggerInfo.iTargetObjectID);
	m_pTrigger->Set_Parent(SHARED_THIS(CTriggerObject));
 //	CGameInstance::Get().Add_NaviMeshInfo(m_pTransform->Get_WorldPtr());
	CGameInstance::Get().Add_LightMtrl(m_PathName);
	return S_OK;
}
void CTriggerObject::Priority_Update(_float fTimeDelta)
{
	
	m_pTrigger->Pirority_Interaction(fTimeDelta);
}
void CTriggerObject::Update(_float fTimeDelta)
{
	m_pTrigger->Interaction(fTimeDelta);
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(CTriggerObject));

}
void CTriggerObject::Late_Update(_float fTimeDelta)
{
	m_pTrigger->Late_Interaction(fTimeDelta);

	if (0 != m_TriggerInfo.iTargetObjectID)
	{
		//타겟이 있는 경우에만
		for (auto iter : CGameInstance::Get().Find_Layer(ETOUI((LEVEL::GAMEPLAY)), L"Layer_TriggerObject")->Get_ObjectList())
		{
			if (static_pointer_cast<CTriggerObject>(iter)->Get_TriggerInfo().iObjectID == m_TriggerInfo.iTargetObjectID)
			{
				//오브젝트끼리 선 긋기
				_vector fSrcPos = iter->Get_Transform().lock()->Get_State(STATE::POS);
				_vector fDstPos = m_pTransform->Get_State(STATE::POS);
				_vector centerPos = (fSrcPos + fDstPos) * 0.5f; //위치 중심 잡기
				_vector Length = XMVector4Length(fSrcPos - fDstPos); // 위치 크기 잡기

				_float3 fAngle{};
				XMStoreFloat3(&fAngle, XMVector3Normalize(fSrcPos - fDstPos)); // Look 방향 구하기

				XMStoreFloat4x4(&m_TargetMatrix, iter->Get_Transform().lock()->Get_World());
				_float3 fRight, fUp, fLook;

				memcpy(&fRight, m_TargetMatrix.m[0], sizeof(_float3));
				memcpy(&fUp,    m_TargetMatrix.m[1], sizeof(_float3));
				memcpy(&fLook,  m_TargetMatrix.m[2], sizeof(_float3));

				XMStoreFloat3(&fRight, XMVectorSet(1, 0, 0, 0));
				XMStoreFloat3(&fUp,	   XMVectorSet(0, 1, 0, 0));
				XMStoreFloat3(&fLook,  XMVectorSet(fAngle.x, fAngle.y, fAngle.z, 0));

				//선처럼 보이게
				XMStoreFloat3(&fRight,	XMLoadFloat3(&fRight) * 0.01f);
				XMStoreFloat3(&fUp,		XMLoadFloat3(&fUp) * 0.01f);
				XMStoreFloat3(&fLook,	XMLoadFloat3(&fLook) * XMVectorGetX(Length));


				memcpy(&m_TargetMatrix.m[0], &fRight,	 sizeof(_float3));
				memcpy(&m_TargetMatrix.m[1], &fUp,		 sizeof(_float3));
				memcpy(&m_TargetMatrix.m[2], &fLook,	 sizeof(_float3));
				memcpy(&m_TargetMatrix.m[3], &centerPos, sizeof(_float4));
				return;
			}
		}
	}

}
HRESULT CTriggerObject::Render()
{
	_float4x4 matWorld{};
	_float4 fColor = { 1.f,0.f,0.f,1.f };

	XMStoreFloat4x4(&matWorld, m_pTransform->Get_World());

	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pTrigger->Bind_Resource(m_pShaderCom, "g_Color");
	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);

		if (pMesh == nullptr)
			continue;

		auto Light = CGameInstance::Get().Find_LightMtrl(m_PathName).lock();
		if (NULL_FALSE(Light))
			m_pShaderCom->Bind_RawValue("g_tagLight", Light.get(), sizeof(LIGHT_VALUE));
		pMesh->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
			
		m_pShaderCom->Begin(0);
		
		pMesh->Bind_Resource();
		pMesh->Render();

	}


	//선긋기용

	_bool bCheck(true);
	m_pBoxShader->Bind_Matrix("g_World", &m_TargetMatrix);
	m_pBoxShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pBoxShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pBoxShader->Bind_RawValue("g_Color", &fColor,sizeof(fColor));
	m_pBoxShader->Bind_RawValue("g_bChoice", &bCheck, sizeof bCheck);

	m_pBoxShader->Begin(0);

	m_pBoxMesh->Bind_Resource();
	m_pBoxMesh->Render();
	fColor = { 0.f,0.f,0.f,0.f };
	m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));

	if (!m_bBoxColor)
		return S_OK;


	//그냥 박스용
	_matrix matW = m_pTransform->Get_World();
	_vector fScale = {};

	_float3 fMax = m_pTransform->Get_Max();
	_float3 fMin = m_pTransform->Get_Min();

	_vector vRight = matW.r[0];
	_vector vUp    = matW.r[1];
	_vector vLook = matW.r[2];
	_vector vPos = matW.r[3];
	fScale = (XMLoadFloat3(&fMax) - XMLoadFloat3(&fMin));

	_vector vPivotPos = ((XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f);

	_vector vRotPos = {};
	vRotPos = (XMVectorGetX(vPivotPos) * XMVector4Normalize(vRight)) +
		(XMVectorGetY(vPivotPos) * XMVector4Normalize(vUp)) +
		(XMVectorGetZ(vPivotPos) * XMVector4Normalize(vLook));

	//중심 다시 계산
	_vector vPosAll[8] = {};
	vPosAll[0] = { fMin.x  , fMin.y  ,fMin.z ,1.f };
	vPosAll[1] = { fMax.x  , fMin.y  ,fMin.z ,1.f };
	vPosAll[2] = { fMin.x  , fMax.y  ,fMin.z ,1.f };
	vPosAll[3] = { fMax.x  , fMax.y  ,fMin.z ,1.f };
	vPosAll[4] = { fMin.x  , fMin.y  ,fMax.z ,1.f };
	vPosAll[5] = { fMax.x  , fMin.y  ,fMax.z ,1.f };
	vPosAll[6] = { fMin.x  , fMax.y  ,fMax.z ,1.f };
	vPosAll[7] = { fMax.x  , fMax.y  ,fMax.z ,1.f };

	_float4 fCheckMax = { -FLT_MAX,-FLT_MAX ,-FLT_MAX ,1.f};
	_float4 fCheckMin = { FLT_MAX,FLT_MAX ,FLT_MAX ,1.f};

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
	vUp    = XMVectorGetY(fScale * XMVector3Length(vUp)) * 0.5f * XMVector4Normalize(vUp);
	vLook  = XMVectorGetZ(fScale * XMVector3Length(vLook)) * XMVector4Normalize(vLook);

	_matrix World = { };
	World.r[0] = vRight;
	World.r[1] = vUp;
	World.r[2] = vLook;
	World.r[3] = vPos;
	fColor = { 0.f,1.f,0.f,1.f };
	XMStoreFloat4x4(&matWorld, World);
	m_pBoxShader->Bind_Matrix("g_World", &matWorld);
	m_pBoxShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pBoxShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pBoxShader->Bind_RawValue("g_Color", &fColor,sizeof(fColor));

	m_pBoxShader->Begin(0);

	m_pBoxMesh->Bind_Resource();
	m_pBoxMesh->Render();


	fColor = { 0.f,0.f,0.f,0.f };
	m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));

	return S_OK;
}


void CTriggerObject::Set_Trigger()
{
	m_pTrigger->Set_Trigger();
}

json CTriggerObject::Save_Data()
{

	nlohmann::json j;

	_float4x4 fWorld = { };

	XMStoreFloat4x4(&fWorld, m_pTransform->Get_World());

	_float3 fRight = { fWorld._11,fWorld._12,fWorld._13 };
	_float3 fUp = { fWorld._21,fWorld._22 ,fWorld._23 };
	_float3 fLook = { fWorld._31, fWorld._32,fWorld._33 };
	_float3 fPos = { fWorld._41,fWorld._42,fWorld._43 };


	j["Right"] = { fWorld._11,fWorld._12,fWorld._13 };
	j["Up"] = { fWorld._21,fWorld._22 ,fWorld._23 };
	j["Look"] = { fWorld._31, fWorld._32,fWorld._33 };
	j["fPos"] = { fWorld._41,fWorld._42,fWorld._43 };
	j["Name"] = m_PathName;
	j["ModeNumber"] = m_TriggerInfo.iModeNumber;
	j["MeshType"] = m_eMeshType;
	j["TriggerValue"] = m_TriggerInfo.strTriggerName;  //이거는 gui에서 Load중
	j["ObjectID"] = m_TriggerInfo.iObjectID;
	j["ObjectTargetID"] = m_TriggerInfo.iTargetObjectID;
	j["OtherTriggerValue"] = m_TriggerInfo.bOtherTrigger; //이거도 gui에서 load

	j["TriggerRot"] = static_cast<int32_t>(m_TriggerInfo.eRot);
	j["TriggerArrow"] = m_TriggerInfo.fArrrowRotation;
	j["FrameTick"] = m_TriggerInfo.fFrameTickTime ;
	j["FrameMaxTime"] = m_TriggerInfo.fMaxFrameTime;

	return j;
}
HRESULT CTriggerObject::Ready_Component()
{

	return S_OK;
}
HRESULT CTriggerObject::Create_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_NonAnime"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"), TEXT("Com_BoxShader"), m_pBoxShader)))
		return E_FAIL;
	
	
	size_t iSize = MultiByteToWideChar(CP_UTF8,0,m_TriggerInfo.strTriggerName.c_str(),ETOUI(m_TriggerInfo.strTriggerName.size()),NULL,0);
	_wstring TriggerName(iSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, m_TriggerInfo.strTriggerName.c_str(), ETOUI(m_TriggerInfo.strTriggerName.size()),TriggerName.data(), iSize);

	m_pTrigger = static_pointer_cast<CTrigger>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), TriggerName, pArg));
	if (nullptr == m_pTrigger) return E_FAIL;
	
	
	return S_OK;
}
void CTriggerObject::Load_Data(void* pDesc, const json& j)
{
	auto desc = static_cast<GAMEOBJECT_DESC*>(pDesc);

	desc->matWorld._11   = j["Right"][0]; desc->matWorld._12 = j["Right"][1]; desc->matWorld._13 = j["Right"][2]; desc->matWorld._14 = 0;
	desc->matWorld._21   = j["Up"]   [0]; desc->matWorld._22 = j["Up"][1];    desc->matWorld._23 = j["Up"][2];    desc->matWorld._24 = 0;
	desc->matWorld._31   = j["Look"] [0]; desc->matWorld._32 = j["Look"][1];  desc->matWorld._33 = j["Look"][2];  desc->matWorld._34 = 0;
	desc->matWorld._41   = j["fPos"] [0]; desc->matWorld._42 = j["fPos"][1];  desc->matWorld._43 = j["fPos"][2];  desc->matWorld._44 = 1;
	m_PathName			 = j["Name"];
	int32_t iModelNumber = j["ModeNumber"];
	desc->FileName       =  m_PathName;
	desc->iModeNumber    = iModelNumber;
	m_TriggerInfo.iObjectID = j["ObjectID"];
	m_TriggerInfo.iTargetObjectID = j["ObjectTargetID"];
	
}
unique_ptr<CTriggerObject> CTriggerObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CTriggerObject>(new CTriggerObject(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed TriggerObject");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CTriggerObject::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CTriggerObject(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed TriggerObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
