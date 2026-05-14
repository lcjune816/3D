#include "Trigger.h"
#include "GameObject.h"
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

HRESULT CTrigger::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}



void CTrigger::Set_Parent(shared_ptr<class CGameObject> pObj)
{
	m_pParent = pObj; 
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

void CTrigger::Set_Flag(TRIGGER_FLAG eFlag, FLAGVALUE eValue)
{

	switch (eValue)
	{
	case FLAGVALUE::ENABLE:

		m_iFlag |= ETOUI(eFlag);
		break;

	case FLAGVALUE::DISABLE:

		m_iFlag &= ~ETOUI(eFlag);
		break;

	case FLAGVALUE::TOGGLE:

		m_iFlag ^= ETOUI(eFlag);
		break;

	case FLAGVALUE::RESET:
		
		m_iFlag = 0 ;
		break;
	
	}
}

_bool CTrigger::offsetMatrix(_float4x4* pMatrix)
{
	return false;
}
