#include "Lever.h"
#include "GameObject.h"
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

	m_eEventTrigger = TRIGGER_EVENT::LEVER;
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CLever::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER))) return E_FAIL;

	m_fFrameTick += fTimeDelta;
	if (m_fFrameTick > 0.01f)
	{
		m_fFrameTick = 0.f;
		m_fAngle += m_fRotationArrow;
		Action_Trigger();
		++m_fFrameTime;
	}

	if (m_fFrameTime > 40.f)
	{
		m_fFrameTime = 0.f;
		m_fRotationArrow *= -1.f;
		m_fAngle = 0.f;
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
	}
	return S_OK;
}
HRESULT CLever::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger )
{
	return S_OK;
}

void CLever::Set_Trigger()
{
	Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
}

void CLever::Action_Trigger()
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;

	pObj->Get_Transform().lock()->Apply_Rotation(_vector{ 1,0,0,0 }, m_fAngle);
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