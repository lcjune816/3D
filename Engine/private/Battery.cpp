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
	
		
	return S_OK;
}
HRESULT CBattery::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	if (m_bOtherTrigger) return E_FAIL;

	_vector vPos{};
	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj))
		return E_FAIL;
	auto pTransform = pObj->Get_Transform().lock();

	if (NULL_FALSE(pDstTransform))
	{
		_matrix SrcWorld = pTransform->Get_World();
		_matrix DstWorld = pDstTransform->Get_World();
		
		_vector vWorldSrcPos  = SrcWorld.r[3];

		_vector vWorldDstPos  = DstWorld.r[3];
		_vector vWorldDstLook = DstWorld.r[2];

		_float3 SrcMax = pTransform->Get_Max();
		_float3 SrcMin = pTransform->Get_Min();
	
		_vector SrcCenter = (XMLoadFloat3(&SrcMax) + XMLoadFloat3(&SrcMin)) * 0.5f;
		_vector Right = XMVector3Normalize(pDstTransform->Get_State(STATE::RIGHT));
		_vector Up    = XMVector3Normalize(pDstTransform->Get_State(STATE::UP));
		_vector Look = XMVector3Normalize(pDstTransform->Get_State(STATE::LOOK));

		Right *= -1.f;
		_vector SrcPivot = 
							Right * XMVectorGetX(SrcCenter) +
						   Up * XMVectorGetY(SrcCenter) +
						   Look * XMVectorGetX(SrcCenter);
		
		vPos = vWorldDstPos - SrcPivot - Look *3.f;

		vPos = XMVectorSetW(vPos, 1.f);

		pTransform->Set_State(STATE::RIGHT, Right);
		pTransform->Set_State(STATE::UP, Up);
		pTransform->Set_State(STATE::LOOK, Look);
		pTransform->Set_State(STATE::POS, vPos);

	}
	else
	{
		vPos = pTransform->Get_State(STATE::POS);
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

		vPos = XMVectorSetW(XMLoadFloat3(&Pos), 1.f);

		pTransform->Set_State(STATE::POS, vPos);
	}



	auto Target = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_TRUE(Target))
		return E_FAIL;
	Target->Set_OtherTrigger(true);
	if (0 == static_pointer_cast<CBatteryCase>(Target)->Action_Trigger(m_pParent.lock()->Get_Transform()))
	{
		pObj->Set_EndObject(true);
		Disconnect_Transform();
		m_bOtherTrigger = true;

		m_bTriggerOn = false;
		m_iFlag |= ETOUI(TRIGGER_FLAG::END);
	}
	return S_OK;
}
void CBattery::Action_Trigger()
{
}

unique_ptr<CBattery>CBattery::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
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