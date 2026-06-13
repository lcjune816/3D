#include "Button.h"
#include "GameInstance.h"
CButton::CButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CButton::CButton(const CButton& Prototype) : CTrigger(Prototype)
{
}

CButton::~CButton()
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);

	m_eState = TRIGGER_STATE::IDLE;
	

	return S_OK;
}

HRESULT CButton::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return E_FAIL;
	auto pTransform = pObj->Get_TransformPtr();

	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		XMStoreFloat4(&m_vOriginPos, pTransform->Get_State(STATE::POS));
		break;
	case TRIGGER_STATE::ACTION:
		_float4 FinalPos = {};
		if (StartPos(pTransform->Get_State(STATE::POS), XMLoadFloat4(&m_vOriginPos) - XMVectorSet(0, 0.2f, 0,1),&FinalPos ,0.8f))
		{
			m_eState = TRIGGER_STATE::RETURN;
			XMStoreFloat4(&m_vOriginPos, pTransform->Get_State(STATE::POS));
			TriggerToTrigger();
		}
		else
			Action_Trigger(XMLoadFloat4(&FinalPos));
		break;
	case TRIGGER_STATE::RETURN:
		if (EndPos(pTransform->Get_State(STATE::POS), XMLoadFloat4(&m_vOriginPos) + XMVectorSet(0, 0.2f, 0, 1), &FinalPos, 0.8f))
		{
			m_eState = TRIGGER_STATE::IDLE;
		}
		else
			Action_Trigger(XMLoadFloat4(&FinalPos));
		break;


	case TRIGGER_STATE::PAUSE:

		break;
	case TRIGGER_STATE::WORLD:
		break;
	}
	return S_OK;
}
HRESULT CButton::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

void CButton::TriggerToTrigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iLevel, m_iTargetNumber).lock();
	if (NULL_TRUE(TriggerCheck))
		return;

	TriggerCheck->TriggerToTrigger();
}

void CButton::Set_Trigger()
{
	if (m_eState != TRIGGER_STATE::IDLE)
		return;

	m_eState = TRIGGER_STATE::ACTION;
	
}

void CButton::Action_Trigger(_fvector vPos)
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;

	pObj->Get_Transform().lock()->Set_State(STATE::POS,XMVectorSetW(vPos,1.f));
}

unique_ptr<CButton>CButton::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CButton>(new CButton(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CButton");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CButton::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CButton>(new CButton(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CButton Clone");
		return nullptr;
	}

	return pInstance;

}