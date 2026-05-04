#include "Lever.h"

CLever::CLever(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CLever::CLever(const CLever& Prototype) : CTrigger(Prototype)
{
}

CLever::~CLever()
{
}

HRESULT CLever::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLever::Initialize(void* pArg)
{
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CLever::Interaction(shared_ptr<CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger)
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

void CLever::Action_Trigger(shared_ptr<CTransform> pTransform)
{
	pTransform->Apply_Rotation(_vector{ 1,0,0,0 }, m_fAngle);
}

unique_ptr<CLever>CLever::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CLever>(new CLever(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CLever");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CLever::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CLever>(new CLever(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CLever Clone");
		return nullptr;
	}

	return pInstance;

}