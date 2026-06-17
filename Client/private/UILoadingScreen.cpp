#include "UILoadingScreen.h"
#include "GameInstance.h"
CUILoadingScreen::CUILoadingScreen(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIObject(pDevice,pContext)
{
}

CUILoadingScreen::CUILoadingScreen(const CUIObject& Prototype) : CUIObject(Prototype)
{
}

CUILoadingScreen::~CUILoadingScreen()
{
}

HRESULT CUILoadingScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUILoadingScreen::Initialize(void* pArg)
{
	m_pVIBufferCom = static_pointer_cast<CRect>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Rect", nullptr));
	if (NULL_TRUE(m_pVIBufferCom)) return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Rect"), TEXT("Com_Shader"), m_pShaderCom))) return E_FAIL;

	return S_OK;
}

void CUILoadingScreen::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUILoadingScreen::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUILoadingScreen::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUILoadingScreen::Render()
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
unique_ptr<CUILoadingScreen>	CUILoadingScreen::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CUILoadingScreen>(new CUILoadingScreen(pDevice,pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed UILoadingScreen");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CUILoadingScreen::Clone(void* pArg)
{

	auto pInstance = shared_ptr<CUILoadingScreen>(new CUILoadingScreen(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed UILoadingScreen");
		return nullptr;
	}
	return pInstance;
}
