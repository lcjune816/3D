#include "LightEffect.h"

CLightEffect::CLightEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject{pDevice, pContext }
{

}
CLightEffect::CLightEffect(const CLightEffect& Prototype) : CGameObject(Prototype )
	{

}
CLightEffect::~CLightEffect()
{

}

HRESULT CLightEffect::Initialize(void* pArg)
{
	auto pDesc = static_cast<LIGHTEFFECT_DESC*>(pArg);
	m_eWorldEventType = pDesc->eWorldEventType;
	m_eLocalEventType= pDesc->eLocalEventType;
	m_eHandle = pDesc->eHandle;
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CLightEffect::Initialize_Prototype()
{
	return S_OK;
}

void CLightEffect::Priority_Update(_float fTimeDelta)
{

}

void CLightEffect::Update(_float fTimeDelta)
{

}

void CLightEffect::Late_Update(_float fTimeDelta)
{

}
HRESULT CLightEffect::Render()
{
	return S_OK;
}
