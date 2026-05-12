#include "Door.h"

CDoor::CDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor& Prototype) : CTrigger(Prototype)
{
}

CDoor::~CDoor()
{
}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{

	m_eEventTrigger = TRIGGER_EVENT::DOOR;
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CDoor::Interaction(shared_ptr<CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger)
{
	if (!m_bTriggerOn) return E_FAIL;

	m_fFrameTick += fTimeDelta;
	if (m_fFrameTick > 0.01f)
	{
		m_fFrameTick = 0.f;
		m_fAngle += m_fRotationArrow;
		Action_Trigger(pTransform);
		++m_fFrameTime;
	}

	if (m_fFrameTime > 20.f)
	{

		m_fFrameTime = 0.f;
		m_fRotationArrow *= -1.f;
		m_fAngle = 0.f;
		m_bTriggerOn = false;
	}
	return S_OK;
}

void CDoor::Action_Trigger(shared_ptr<CTransform> pTransform)
{
	pTransform->Apply_Rotation(_vector{ 0,1,0,0 }, m_fAngle);
}

unique_ptr<CDoor>CDoor::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CDoor>(new CDoor(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CDoor");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CDoor::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CDoor>(new CDoor(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CDoor Clone");
		return nullptr;
	}

	return pInstance;

}