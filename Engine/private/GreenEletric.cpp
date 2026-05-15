#include "GreenEletric.h"
#include "GameObject.h"
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

	m_eEventTrigger = TRIGGER_EVENT::ELECTRIC;
	m_fRotationArrow = 10.f;
	m_bOtherTrigger = true;
	return S_OK;
}
HRESULT CGreenElectric::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger )
{
	
	return S_OK;

}
HRESULT CGreenElectric::Interaction(_float fTimeDelta,  _bool bOtherTrigger)
{
	auto DstTransform = m_pDstTransform.lock();

	if (!m_bOtherTrigger)
	{
		m_fFrameTick += fTimeDelta;

		if (m_fFrameTick > 0.1f)
		{
			m_fFrameTick = 0.f;
			++m_fFrameTime;
		}
		if (m_fFrameTime > 5.f)
		{
			m_fFrameTime = 0.f;
			m_bOtherTrigger = true;
			Disconnect_Transform();
		}

		return S_OK;
		//여기 전기 파지직
	}

	_vector vPos{};

	if (NULL_FALSE(DstTransform)&& m_bOtherTrigger)
	{
		auto pObj = m_pParent.lock();
		if (NULL_TRUE(pObj))
			return E_FAIL;
		auto pTransform = pObj->Get_Transform().lock();

		m_bOtherTrigger = true;
		m_fFrameTick += fTimeDelta;

		if (m_fFrameTick > 0.1f)
		{
			m_fFrameTick = 0.f;
			++m_fFrameTime;
		}
		_float3 fMax = pTransform->Get_Max();
		_float3 fMin = pTransform->Get_Min();
		_vector vCenter = (XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f;
		_vector Pos = DstTransform->Get_World().r[3];
		vCenter = XMVector3TransformCoord(vCenter, pTransform->Get_World());

		_vector Look = XMVector3Normalize(XMVectorSetY(XMVectorSetX(Pos, 0.f), 0.f) - XMVectorSetY(vCenter, 0.f));
		vCenter += Look * 8.f;
		XMVectorSetW(vCenter, 1.f);
		DstTransform->Set_State(STATE::POS, vCenter);
	}

	if (m_fFrameTime > 20.f)
	{
		m_fFrameTime = 0.f;
		//m_bOtherTrigger = false;
		//m_pDstTransform = nullptr;
	}

	if (!m_bTriggerOn) return E_FAIL;
	Action_Trigger();
	m_bTriggerOn = false;
}
HRESULT CGreenElectric::Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger )
{//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지

	
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