#include "RightHand_Effect_Particle.h"

CRightHand_Effect_Particle::CRightHand_Effect_Particle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CPLayer_RightHand(pDevice, pContext)
{
}

CRightHand_Effect_Particle::CRightHand_Effect_Particle(const CRightHand_Effect_Particle& Prototype)
	:CPLayer_RightHand(Prototype)
{
}

CRightHand_Effect_Particle::~CRightHand_Effect_Particle()
{
}

HRESULT CRightHand_Effect_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRightHand_Effect_Particle::Initialize(void* pArg)
{
	auto pDesc = static_cast<RHAND_PARTICLE_DESC*>(pArg);
	m_eType = pDesc->eRightEffectType;
	m_pParentMatirx = pDesc->ParentsMatrix;
	m_piFlag = pDesc->piFlag;
	CGameObject::Initialize(pArg);

	switch (pDesc->eRightEffectType)
	{
	case RIGHTEFFECT::SPARK1:
		CGameInstance::Get().Add_Decal_Texture("../../Particles/GreenHand/T_EnergyHop_1.dds");
		m_iTextureId[ETOUI(TEXTUREID::DIFFUSE)] = CGameInstance::Get().Find_TextueId("../../Particles/GreenHand/T_EnergyHop_1.dds");
		if (m_iTextureId[ETOUI(TEXTUREID::DIFFUSE)] == -1)
			return E_FAIL;

		m_pVIBuffer = static_pointer_cast<CVIBuffer_Particle_Point>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC),
			TEXT("Component_Buffer_Particle_Spark2"), nullptr));
		if (NULL_TRUE(m_pVIBuffer))
			return E_FAIL;

		break;
	case RIGHTEFFECT::SPARK2:
		CGameInstance::Get().Add_Decal_Texture("../../Particles/GreenHand/T_EnergyFlick_1_BaseColor.dds");
		m_iTextureId[ETOUI(TEXTUREID::DIFFUSE)] = CGameInstance::Get().Find_TextueId("../../Particles/GreenHand/T_EnergyFlick_1_BaseColor.dds");
		if (m_iTextureId[ETOUI(TEXTUREID::DIFFUSE)] == -1)
			return E_FAIL;

		m_pVIBuffer = static_pointer_cast<CVIBuffer_Particle_Point>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC),
			TEXT("Component_Buffer_Particle_Spark3"), nullptr));
		if (NULL_TRUE(m_pVIBuffer))
			return E_FAIL;
		break;
	}


	CGameInstance::Get().Add_Decal_Texture("../../Resource/Character/Effect/T_Noise4.dds");
	m_iTextureId[ETOUI(TEXTUREID::NOISE)] = CGameInstance::Get().Find_TextueId("../../Resource/Character/Effect/T_Noise4.dds");
	if (m_iTextureId[ETOUI(TEXTUREID::NOISE)] == -1)
		return E_FAIL;
	_matrix mat = XMMatrixIdentity();


	if (FAILED(Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Instancing_Spark"), TEXT("Component_Shader"), m_pShaderCom, nullptr)))
		return E_FAIL;
	
	_matrix offset = XMMatrixScaling(pDesc->vOffsetScale.x, pDesc->vOffsetScale.y, pDesc->vOffsetScale.z) * XMMatrixRotationY(XMConvertToRadians(90.f))
		* XMMatrixTranslation(pDesc->vOffsetPos.x, pDesc->vOffsetPos.y, pDesc->vOffsetPos.z);
	XMStoreFloat4x4(&m_OffsetMatrix, offset);

	return S_OK;
}

void CRightHand_Effect_Particle::Priority_Update(_float fTimeDelta)
{
	m_iStateFlag = *m_piFlag;

}

void CRightHand_Effect_Particle::Update(_float fTimeDelta)
{
	if (!Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
		return;
	m_fTime += fTimeDelta;

	_float4 fTest = *CGameInstance::Get().ColorTester();

	_matrix ParentWorldMatrix = XMLoadFloat4x4(m_pParentMatirx);

	for (uint32_t i = 0; i < 3; ++i)
		ParentWorldMatrix.r[i] = XMVector3Normalize(ParentWorldMatrix.r[i]);

	XMStoreFloat4x4(&m_FinalMatrix, XMLoadFloat4x4(&m_OffsetMatrix) * ParentWorldMatrix);
}

void CRightHand_Effect_Particle::Late_Update(_float fTimeDelta)
{
	if (!Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
		return;

	m_fTimeTick += fTimeDelta;
	switch (m_eType)
	{
	case RIGHTEFFECT::SPARK1:
		m_pVIBuffer->Spark1(fTimeDelta);
		break;

	case RIGHTEFFECT::SPARK2:
		m_pVIBuffer->Spark1(fTimeDelta);
		break;
	}

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLOOM_BEFORE, SHARED_THIS(CRightHand_Effect_Particle));
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLOOM, SHARED_THIS(CRightHand_Effect_Particle));

}

HRESULT CRightHand_Effect_Particle::Render()
{
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_FinalMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureId[ETOUI(TEXTUREID::DIFFUSE)]));
	m_pShaderCom->Bind_RawValue("g_vCamePosition", CGameInstance::Get().Get_CamPosition(), sizeof _float4);
	m_pShaderCom->Begin(2);

	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRightHand_Effect_Particle::Render_Bloom()
{
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_FinalMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureId[ETOUI(TEXTUREID::DIFFUSE)]));
	m_pShaderCom->Bind_RawValue("g_vCamePosition", CGameInstance::Get().Get_CamPosition(),sizeof _float4 );
	m_pShaderCom->Begin(1);
	

	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render();

	return S_OK;
}

unique_ptr<CRightHand_Effect_Particle> CRightHand_Effect_Particle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CRightHand_Effect_Particle>(new CRightHand_Effect_Particle(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed RightHand Effect");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<CPrototype> CRightHand_Effect_Particle::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CRightHand_Effect_Particle>(new CRightHand_Effect_Particle(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed RightHand Effect");
		return nullptr;
	}

	return pInstance;
}
