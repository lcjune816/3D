#include "Door.h"
#include "GameObject.h"
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
	Set_Flag(ETOUI(TRIGGER_FLAG::CANCLE), FLAGVALUE::ENABLE);
	return S_OK;
}

HRESULT CDoor::Interaction( _float fTimeDelta,  _bool bOtherTrigger)
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

	if (m_fFrameTime > 20.f)
	{

		m_fFrameTime = 0.f;
		m_fRotationArrow *= -1.f;
		m_fAngle = 0.f;

		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
	}
	return S_OK;
}
HRESULT CDoor::Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger)
{
	return S_OK;
}
void CDoor::Set_Trigger()
{
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		return;

	Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
}
void CDoor::Action_Trigger()
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;

	pObj->Get_Transform().lock()->Apply_Rotation(_vector{0,1,0,0}, m_fAngle);
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