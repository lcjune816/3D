#include "Trigger.h"

CTrigger::CTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CComponent{pDevice, pContext}
{
}

CTrigger::CTrigger(const CTrigger& Prototype) : CComponent(Prototype)
{
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);
	m_bOtherTrigger = pDesc->bTrigger;

	return S_OK;
}



_bool				CTrigger::Set_DstTransform(CTransform* pTransform)
{
	if (NULL_TRUE(m_pDstTransform))
	{
		m_pDstTransform = pTransform;
		return true;
	}

	return false;
}