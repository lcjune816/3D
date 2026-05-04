#include "GreenEletric.h"
#include "GameInstance.h"

CGreenElectric::CGreenElectric(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CGreenElectric::CGreenElectric(const CGreenElectric& Prototype) : CTrigger(Prototype)
{
}

CGreenElectric::~CGreenElectric()
{
}

HRESULT CGreenElectric::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGreenElectric::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	__super::Initialize(pArg);

	m_fRotationArrow = 10.f;
	return S_OK;
}

HRESULT CGreenElectric::Interaction(shared_ptr<CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger)
{
	//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지
	if (m_bOtherTrigger)
	{
		//여기 전기 파지직
	}
	else return E_FAIL;

	if (!m_bTriggerOn) return E_FAIL;
		Action_Trigger();
		m_bTriggerOn = false;

	return S_OK;
}

void CGreenElectric::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_FALSE(TriggerCheck))
		TriggerCheck->Set_Trigger();
}	

unique_ptr<CGreenElectric>  CGreenElectric::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CGreenElectric>(new CGreenElectric(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CGreenElectric");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CGreenElectric::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CGreenElectric>(new CGreenElectric(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CGreenElectric Clone");
		return nullptr;
	}

	return pInstance;

}