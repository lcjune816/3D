#include "Player_Light.h"
#include "WorldLight.h"
CPlayer_Light::CPlayer_Light(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CPlayer(pDevice,pContext)
{
}

CPlayer_Light::CPlayer_Light(const CPlayer& CPrototype) : CPlayer(CPrototype)
	
{
}

CPlayer_Light::~CPlayer_Light()
{
}

HRESULT CPlayer_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Light::Initialize(void* pArg)
{

	auto pDesc = static_cast<PLAYER_LIGHT_DESC*>(pArg);
	
	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(Ready_Component(pArg, pDesc->iLevel))) return E_FAIL;

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	CGameObject::Initialize(pArg);
	return S_OK;
}

void CPlayer_Light::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_Light::Update(_float fTimeDelta)
{
	CombineMatrix();
}

void CPlayer_Light::Late_Update(_float fTimeDelta)
{
	m_pLight->Late_Update(fTimeDelta);
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(CPlayer_Light));
}

HRESULT CPlayer_Light::Render()
{
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureID));
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();

	return S_OK;
}
void CPlayer_Light::CombineMatrix()
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pParentMatrix);
	m_pTransform->Set_Matrix(WorldMatrix);
	LIGHT_DESC* pDesc = m_pLight->Get_LightDescPtr();
	if (NULL_TRUE(pDesc))
		return;

	XMStoreFloat4(&pDesc->vPos, m_pTransform->Get_State(STATE::POS));
}
HRESULT CPlayer_Light::Ready_Component(void* pArg, uint32_t iLevelIndex)
{
	m_pVIBufferCom = static_pointer_cast<CRect>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Rect", nullptr));
	if (NULL_TRUE(m_pVIBufferCom)) return E_FAIL;
	
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Rect"), TEXT("Com_Shader"), m_pShaderCom)))


	CGameInstance::Get().Add_Decal_Texture("../../Particles/Flipbooks/HSHDoorway/T_HSHSmoke_dda.dds");
	m_iTextureID = CGameInstance::Get().Find_TextueId("../../Particles/Flipbooks/HSHDoorway/T_HSHSmoke_dda.dds");
        	if (m_iTextureID == -1) return E_FAIL;
	
	CWorldLight::WORLDLIGHT_DESC Light{};
	Light.eType = INIT_TYPE::OBJECT;
	Light.LightDesc.eUseType = USETYPE::CLIENT;
	Light.LightDesc.eType = LIGHT::SPOTLIGHT;
	Light.LightDesc.eLocalEventType = LIGHT_STATE::NONE;
	Light.LightDesc.eWorldEventType = WORLD_EVENT::END;
	Light.LightDesc.fRange = _float2(30.f, 0.f);
	Light.LightDesc.vDiffuse = _float4(1.2f, 1.8f, 5.f, 1.f);
	Light.LightDesc.vAmbient = _float4(0.5f, 0.5f, 1.f, 1.f);
	Light.LightDesc.vSpecular = _float4(1.f, 1.f, 0.8f, 1.f);
	XMStoreFloat4(&Light.LightDesc.vPos, m_pTransform->Get_State(STATE::POS) + XMVectorSet(0, 55, 0, 0));
	m_pLight = static_pointer_cast<CWorldLight>(CGameInstance::Get().Clone_Prototype(iLevelIndex, L"OBJ_Light", &Light));
	if (NULL_TRUE(m_pLight)) return E_FAIL;

	return S_OK;
}
unique_ptr<CPlayer_Light> CPlayer_Light::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPlayer_Light>(new CPlayer_Light(pDevice,pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CPlayerLight");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CPlayer_Light::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CPlayer_Light>(new CPlayer_Light(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CPlayerLight");
		return nullptr;
	}
	return pInstance;
}
