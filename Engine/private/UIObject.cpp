#include "UIObject.h"
#include "GameInstance.h"
CUIObject::CUIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice,pContext)
{
}

CUIObject::~CUIObject()
{
}


HRESULT CUIObject::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CUIObject::Initialize(void* pArg)
{
	auto		pDesc = static_cast<UIOBJECT_DESC*>(pArg);
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_float2		vViewportSize = CGameInstance::Get().Get_ViewportSize();
	
	Update_Transfrom();

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH(vViewportSize.x, vViewportSize.y, 0.f, 1.f));
	return S_OK;
}
void    CUIObject::Priority_Update(_float fTimeDelta)
{

}
void    CUIObject::Update(_float fTimeDelta)
{

}
void    CUIObject::Late_Update(_float fTimeDelta)
{

	Update_Transfrom();
}
HRESULT CUIObject::Render()
{
	return S_OK;
}

void	CUIObject::Update_Transfrom()
{
	_float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

	m_pTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransform->Set_State(STATE::POS,
		XMVectorSet(m_fX - vViewportSize.x * 0.5f, -m_fY + vViewportSize.y * 0.5f, 0.f, 1.f));

}