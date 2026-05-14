#include "BlueElectric.h"
#include "GameObject.h"
CBlueElectric::CBlueElectric(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CBlueElectric::CBlueElectric(const CBlueElectric& Prototype) : CTrigger(Prototype)
{
}

CBlueElectric::~CBlueElectric()
{
}

HRESULT CBlueElectric::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlueElectric::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	__super::Initialize(pArg);

	m_eEventTrigger = TRIGGER_EVENT::ELECTRIC;
	m_fRotationArrow = 10.f;
	m_bOtherTrigger = true;
	return S_OK;
}
HRESULT CBlueElectric::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	return S_OK;

}
HRESULT CBlueElectric::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	if (!m_bTriggerOn) return E_FAIL;

	if (m_bTriggerOn && !m_bOtherTrigger)
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
			m_pDstTransform = nullptr;
		}

		return S_OK;
		//여기 전기 파지직
	}

	_vector vPos{};

	if (m_pDstTransform != nullptr && m_bOtherTrigger)
	{
		m_fFrameTick += fTimeDelta;

		if (m_fFrameTick > 0.1f)
		{
			m_fFrameTick = 0.f;
			++m_fFrameTime;
		}
	}

	if (m_fFrameTime > 20.f)
	{
		m_fFrameTime = 0.f;
		m_bOtherTrigger = false;
		m_bTriggerOn = false;
	}

	if (m_iFlag && ETOUI(TRIGGER_FLAG::SHADER))
		Action_Trigger();

	return S_OK;
}
HRESULT CBlueElectric::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지


	return S_OK;
}
_bool CBlueElectric::offsetMatrix(_float4x4* pMatrix)
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj) || ((m_pDstTransform == nullptr) && !m_bOtherTrigger))
		return false;

	_matrix matOffset = XMMatrixIdentity();

	auto pTransform = pObj->Get_Transform().lock();
	_float3 fMax = pTransform->Get_Max();
	_float3 fMin = pTransform->Get_Min();


	//_vector vWorldMax = XMVector3TransformCoord(XMLoadFloat3(&fMax), pTransform->Get_World());
	//_vector vWorldMin = XMVector3TransformCoord(XMLoadFloat3(&fMin), pTransform->Get_World());

	_vector vCenter = (XMLoadFloat3(&fMax)+ XMLoadFloat3(&fMin)) * 0.5f;
	_vector vDstPos = m_pDstTransform->Get_State(STATE::POS);

	vCenter = XMVector3TransformCoord(vCenter, pTransform->Get_World());

	_float Length = XMVectorGetX(XMVector3Length((vCenter - vDstPos)));
	_vector FLook = XMVector3Normalize(vCenter - vDstPos);

	vDstPos += FLook * Length;
	matOffset.r[3] = XMVectorSetW(vDstPos,1.f);

	XMStoreFloat4x4(pMatrix, matOffset);
	return true;
}
void CBlueElectric::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_FALSE(TriggerCheck))
		TriggerCheck->Set_Trigger();

}

unique_ptr<CBlueElectric>  CBlueElectric::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CBlueElectric>(new CBlueElectric(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CBlueElectric");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CBlueElectric::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CBlueElectric>(new CBlueElectric(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CBlueElectric Clone");
		return nullptr;
	}

	return pInstance;

}