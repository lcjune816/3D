#include "RollupDoor.h"
#include "GameObject.h"
CRollupDoor::CRollupDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CRollupDoor::CRollupDoor(const CRollupDoor& Prototype) : CTrigger(Prototype)
{
}

CRollupDoor::~CRollupDoor()
{
}

HRESULT CRollupDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollupDoor::Initialize(void* pArg)
{
	m_eEventTrigger = TRIGGER_EVENT::ROLLUPDOOR;
	m_fRotationArrow = 10.f;
	return S_OK;
}

HRESULT CRollupDoor::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	if (!m_bTriggerOn) return E_FAIL;

	m_fFrameTick += fTimeDelta;
	if (m_fFrameTick > 0.03f)
	{
		m_fFrameTick = 0.f;
		Action_Trigger(fTimeDelta);
		++m_fFrameTime;
	}

	if (m_fFrameTime > 60.f)
	{
		m_fFrameTime = 0.f;
		m_fRotationArrow *= -1.f;
		m_bTriggerOn = false;
	}
	return S_OK;
}

HRESULT CRollupDoor::Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger)
{
	return S_OK;
}

void CRollupDoor::Action_Trigger(_float fTimeDelta)
{
	auto pObj = m_pParent.lock();
	if(NULL_TRUE(pObj))
		return;
	pObj->Get_Transform().lock()->Go_Up(fTimeDelta, m_fRotationArrow);
}

unique_ptr<CRollupDoor>CRollupDoor::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CRollupDoor>(new CRollupDoor(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CRollupDoor");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CRollupDoor::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CRollupDoor>(new CRollupDoor(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CRollupDoor Clone");
		return nullptr;
	}

	return pInstance;

}