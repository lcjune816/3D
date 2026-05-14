#include "Battery.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "BatteryCase.h"
CBattery::CBattery(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CBattery::CBattery(const CBattery& Prototype) : CTrigger(Prototype)
{
}

CBattery::~CBattery()
{
}

HRESULT CBattery::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattery::Initialize(void* pArg)
{

	m_eEventTrigger = TRIGGER_EVENT::BATTERY;
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CBattery::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	if (!m_bTriggerOn) return E_FAIL;
	_vector vPos{};
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return E_FAIL;
	auto pTransform = pObj->Get_Transform().lock();

	if (m_pDstTransform != nullptr)
	{

		_vector vSrcPos = pTransform->Get_State(STATE::POS);
		_vector vDstPos = m_pDstTransform->Get_State(STATE::POS);

		_float3 fMax = pTransform->Get_Max();
		_float3 fMin = pTransform->Get_Min();

		_float3 fCenter{};
		//스케일 반지름 중심에서 
		XMStoreFloat3(&fCenter,(XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f);
	
		_float Radius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&fCenter)- (XMLoadFloat3(&fMin))));
		_vector vCenter{};
		_float3 vHandPos = {};
		//원래 플레이어의 손 위치랑 offset만큼 다시 위치에 
	
		vCenter = XMVector3TransformCoord(XMLoadFloat3(&fCenter), pTransform->Get_World());

		XMStoreFloat3(&vHandPos, vDstPos - vCenter);
		_vector vLook = XMVector3Normalize(vDstPos - vCenter);
		//새로구한 좌표 기준으로 원래 dst 위치랑 빼서 차이 구하고
		//원래 위치에 그만큼 더하기

		//반지름만큼 함 밀고
		vPos = (vSrcPos + XMLoadFloat3(&vHandPos)) - vLook * Radius * 4.5f;

	}
	else
	{
		vPos =  pTransform->Get_State(STATE::POS);
		_float3 Pos{};
		XMStoreFloat3(&Pos, vPos);

		_float fY = pTransform->Get_Min().y;
		//배터리는 자기 Pivot Y축 만큼 땅 위로 올리기
		m_fDropTime += 9.8f * fTimeDelta;
		if (Pos.y < -fY)
		{
			Pos.y = -fY;
			m_fDropTime = 0;
		}
		else if (Pos.y > -fY)
			Pos.y -= m_fDropTime;

		vPos = XMVectorSetW(XMLoadFloat3(&Pos),1.f);
	}


	pTransform->Set_State(STATE::POS, vPos);

	auto Target = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_TRUE(Target))
		return E_FAIL;

	if (0 == static_pointer_cast<CBatteryCase>(Target)->Action_Trigger(m_pParent.lock()->Get_Transform()))
	{
		m_pDstTransform = nullptr;
		m_bTriggerOn = false;
	}
		
	return S_OK;
}
HRESULT CBattery::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}
void CBattery::Action_Trigger()
{
}

unique_ptr<CBattery>CBattery::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CBattery>(new CBattery(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CBattery");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CBattery::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CBattery>(new CBattery(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CBattery Clone");
		return nullptr;
	}

	return pInstance;

}