#include "PlayerAim.h"
#include "GameInstance.h"
CPlayerAim::CPlayerAim(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIObject{ pDevice, pContext }
{
}
CPlayerAim::CPlayerAim(const CPlayerAim& Prototype): CUIObject(Prototype)
{
}
CPlayerAim::~CPlayerAim()
{
}

HRESULT CPlayerAim::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CPlayerAim::Ready_Component()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_UI"), TEXT("Com_Shader"), m_pShader)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Rect"), TEXT("Buff_Rect"), m_pBuffer)))
		return E_FAIL;

	const _char* pPathName = "../../Resource/Aim/CrossHair.dds";
	CGameInstance::Get().Add_Decal_Texture(pPathName);
	m_iTextureID = CGameInstance::Get().Find_TextueId(pPathName);

}
HRESULT CPlayerAim::Initialize(void* pArg)
{
	PLAYERAIM_DESC pDesc = {};

	pDesc.fSizeX = 40.f;
	pDesc.fSizeY = 40.f;

	pDesc.fX = g_iWinSizeX * 0.5f;
	pDesc.fY = g_iWinSizeY * 0.5f;

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	return S_OK;
}

void CPlayerAim::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayerAim::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPlayerAim::Late_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI, SHARED_THIS(CPlayerAim));

	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayerAim::Render()
{
	m_pTransform->Bind_Matrix(m_pShader, "g_World");
	m_pShader->Bind_Matrix("g_View", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_Projection", &m_ProjMatrix);
	m_pShader->Bind_SRV("g_Diffuse", CGameInstance::Get().Find_Decal_Texture(m_iTextureID));
	
	m_pShader->Begin(0);
	m_pBuffer->Bind_Resource();
	m_pBuffer->Render();

	return S_OK;
}

shared_ptr<CPrototype> CPlayerAim::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<CGameObject>(new CPlayerAim(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerAim");
		return nullptr;
	}

	return pInstance;
}

unique_ptr<CPlayerAim> CPlayerAim::Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CPlayerAim>(new CPlayerAim(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		return nullptr;
	}

	return pInstance;
	
}