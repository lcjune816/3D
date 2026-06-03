#include "Cat_Fog.h"
#include "GameInstance.h"
CCat_Fog::CCat_Fog(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CCat_Fog::CCat_Fog(const CCat_Fog& pPrototype) : CGameObject(pPrototype)
{
}

CCat_Fog::~CCat_Fog()
{
}

HRESULT CCat_Fog::Ready_Component()
{
	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Particle_Point>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::GASZONE), L"Component_Particle_Point",nullptr));
	if (NULL_TRUE(m_pVIBufferCom))
		return E_FAIL;

	if (FAILED(Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Instancing_Pog"), TEXT("Com_Pog"), m_pShaderCom, nullptr)))
		return E_FAIL;

	CGameInstance::Get().Add_Decal_Texture("../../Resource/Boss/Flipbooks/HSHDoorway/T_HSHSmoke_dda.dds");
	m_iTextureID = CGameInstance::Get().Find_TextueId("../../Resource/Boss/Flipbooks/HSHDoorway/T_HSHSmoke_dda.dds");
}

void CCat_Fog::CombinedMatrix()
{
	
	_matrix myWorld = m_pTransform->Get_World();	
	XMStoreFloat4x4(&m_CombinedMatrix, myWorld * XMLoadFloat4x4(m_pParentMatrix));

}


HRESULT CCat_Fog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCat_Fog::Initialize(void* pArg)
{
	auto pDesc = static_cast<CATFOG_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CCat_Fog::Priority_Update(_float fTimeDelta)
{
}

void CCat_Fog::Update(_float fTimeDelta)
{
	CombinedMatrix();
	m_pVIBufferCom->Fog_Spread(fTimeDelta);
}

void CCat_Fog::Late_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(CCat_Fog));
}

HRESULT CCat_Fog::Render()
{
	m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedMatrix);
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_RawValue("g_vCamePosition", CGameInstance::Get().Get_CamPosition(),sizeof _vector);
	m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureID));

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();
	return S_OK;
}


unique_ptr<CCat_Fog> CCat_Fog::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CCat_Fog>(new CCat_Fog(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CCat_Fog");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CCat_Fog::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CCat_Fog>(new CCat_Fog(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CCat_Fog_Clone ");
		return pInstance;
	}
	return pInstance;
}
