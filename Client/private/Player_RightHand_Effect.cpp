#include "Player_RightHand_Effect.h"

CPlayer_RightHand_Effect::CPlayer_RightHand_Effect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CPLayer_RightHand(pDevice,pContext)
{
}

CPlayer_RightHand_Effect::CPlayer_RightHand_Effect(const CPlayer_RightHand_Effect& Prototype)
	:CPLayer_RightHand(Prototype)
{
}

CPlayer_RightHand_Effect::~CPlayer_RightHand_Effect()
{
}

HRESULT CPlayer_RightHand_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_RightHand_Effect::Initialize(void* pArg)
{
	auto pDesc = static_cast<RHAND_EFFECT_DESC*>(pArg);
	m_eType			= pDesc->eRightEffectType;
	m_pParentMatirx = pDesc->ParentsMatrix;
	m_piFlag	    = pDesc->piFlag;
	CGameObject::Initialize(pArg);

	Engine::IMPORTMODEL_DESC importModel;
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::NONANIME;

	switch (pDesc->eRightEffectType)
	{
	case RIGHTEFFECT::SPARK1:
		importModel.pFile = "../../Resource/Character/Effect/Effect.fbx";
		m_iElectricPattern = { true,false,true,false,false,true,true,true};
		break;
	case RIGHTEFFECT::SPARK2:
		importModel.pFile = "../../Resource/Character/Effect/Effect2.fbx";
		break;
	}
	

	CGameInstance::Get().Add_Decal_Texture("../../Resource/Character/Effect/T_Noise4.dds");
	m_iTextureId = CGameInstance::Get().Find_TextueId("../../Resource/Character/Effect/T_Noise4.dds");
	if (m_iTextureId == -1)
		return E_FAIL;
	_matrix mat = XMMatrixIdentity();

	CGameInstance::Get().ImportModel_NonAnime(importModel,m_pTransform, m_MeshNameList);

	 
	if(FAILED(Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Effect"), TEXT("Component_Shader"), m_pShaderCom, nullptr)))
		return E_FAIL;
	
	_matrix offset =  XMMatrixScaling(pDesc->vOffsetScale.x, pDesc->vOffsetScale.y, pDesc->vOffsetScale.z) *  XMMatrixRotationY(XMConvertToRadians(90.f))
		* XMMatrixTranslation(pDesc->vOffsetPos.x, pDesc->vOffsetPos.y, pDesc->vOffsetPos.z);
	XMStoreFloat4x4(&m_OffsetMatrix, offset);

	return S_OK;
}

void CPlayer_RightHand_Effect::Priority_Update(_float fTimeDelta)
{
	m_iStateFlag = *m_piFlag;

}

void CPlayer_RightHand_Effect::Update(_float fTimeDelta)
{
	if (!Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
		return;
	m_fTime +=  fTimeDelta;

	_float4 fTest = *CGameInstance::Get().ColorTester();
	
	_matrix ParentWorldMatrix = XMLoadFloat4x4(m_pParentMatirx);

	for (uint32_t i = 0; i < 3; ++i)
		ParentWorldMatrix.r[i] = XMVector3Normalize(ParentWorldMatrix.r[i]);

	XMStoreFloat4x4(&m_FinalMatrix, XMLoadFloat4x4(&m_OffsetMatrix) * ParentWorldMatrix); //XMLoadFloat4x4(&m_OffsetMatrix) * ParentWorldMatrix);

}

void CPlayer_RightHand_Effect::Late_Update(_float fTimeDelta)
{
	if (!Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
		return;

	m_fTimeTick += fTimeDelta;
	switch (m_eType)
	{
	case RIGHTEFFECT::SPARK1:
		if (m_fTimeTick > 0.2f)
		{
			++m_iPatternCnt;
			m_fTimeTick = 0.f;
		}
		
		if (m_iPatternCnt == m_iElectricPattern.size())
			m_iPatternCnt = 0;

		if (m_iElectricPattern[m_iPatternCnt] == false)
			return;

		break;
	
	case RIGHTEFFECT::SPARK2:
		break;
	}

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLOOM_BEFORE, SHARED_THIS(CPlayer_RightHand_Effect));
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLOOM, SHARED_THIS(CPlayer_RightHand_Effect));

}

HRESULT CPlayer_RightHand_Effect::Render()
{
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_FinalMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

	m_pShaderCom->Bind_RawValue("g_Time", &m_fTime,sizeof m_fTime);

	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		if (pMesh == nullptr)
			continue;

		pMesh->Bind_ResourceSRV(m_pShaderCom.get(), "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
		m_pShaderCom->Begin(1);
		pMesh->Bind_Resource();
		pMesh->Render();
	}

	return S_OK;
}

HRESULT CPlayer_RightHand_Effect::Render_Bloom()
{
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_FinalMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_RawValue("g_Time", &m_fTime, sizeof m_fTime);
	m_pShaderCom->Bind_SRV("g_NoiseTexture", CGameInstance::Get().Find_Decal_Texture(m_iTextureId));

	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		if (pMesh == nullptr)
			continue;

		pMesh->Bind_ResourceSRV(m_pShaderCom.get(), "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
		m_pShaderCom->Begin(0);
		pMesh->Bind_Resource();
		pMesh->Render();
	}


	return S_OK;
}

unique_ptr<CPlayer_RightHand_Effect> CPlayer_RightHand_Effect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPlayer_RightHand_Effect>(new CPlayer_RightHand_Effect(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed RightHand Effect");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<CPrototype> CPlayer_RightHand_Effect::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CPlayer_RightHand_Effect>(new CPlayer_RightHand_Effect(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed RightHand Effect");
		return nullptr;
	}

	return pInstance;
}
