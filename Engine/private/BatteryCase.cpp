#include "BatteryCase.h"
#include "GameInstance.h"
CBatteryCase::CBatteryCase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CBatteryCase::CBatteryCase(const CBatteryCase& Prototype) : CTrigger(Prototype)
{
}

CBatteryCase::~CBatteryCase()
{
}

HRESULT CBatteryCase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBatteryCase::Initialize(void* pArg)
{

	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);
	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::BATTERYCASE;
 	m_eRot = pDesc->eRot;
	m_fRotationArrow = 0.25f;
	if (WORLD_EVENT::BOSS_SPAWN == pDesc->eWroldEvent)
	{
		m_eState = TRIGGER_STATE::WORLD;
	}
	return S_OK;
}

HRESULT CBatteryCase::Interaction( _float fTimeDelta,  _bool bOtherTrigger)
{
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::WORLD:
		Action_Event();
		break;
	}

	return S_OK;
}
HRESULT CBatteryCase::Late_Interaction( _float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}
void CBatteryCase::Set_Trigger()
{
}

HRESULT CBatteryCase::Action_Trigger(weak_ptr<class CTransform> pTransform)
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		return E_FAIL;

	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return E_FAIL;
	auto SrcTransform = pObj->Get_Transform().lock();
	auto DstTransform = pTransform.lock();

	if (NULL_TRUE(SrcTransform) || NULL_TRUE(DstTransform))
		return E_FAIL;
	
	if(CGameInstance::Get().Only_AABB_Collision(SrcTransform, DstTransform))
	{//충돌하면 배터리를 집어 넣으라
		if (m_eState == TRIGGER_STATE::WORLD)
			Set_Flag(ETOUI(TRIGGER_FLAG::WORLD_EVENT), FLAGVALUE::ENABLE);

		_vector vPos = SrcTransform->Get_State(STATE::POS);
		vPos  += XMVectorSet(0,6.f,0,0);
		_vector vSrcLook = SrcTransform->Get_World().r[2];

		_vector vUp = { 0,1,0,0 };
		_vector vRot = { 1,0,0,0 };
		_vector vRight = { 1,0,0 ,0};
		_vector vLook = { 0,0,1,0 };
		_float fAngel = 90.f;
		_float3 fScale = { 2.f,2.f,2.f };

		_matrix matRot{};


		if (m_eRot == TRIGGER_ROT::X)
		{
			vRot = { 0,0,1,0 };

			if (XMVectorGetX(XMVector3Dot(vSrcLook, vRight)) < 0)
				fAngel *= -1;

			matRot = XMMatrixRotationAxis(vRot, XMConvertToRadians(fAngel));
			vUp = XMVector3TransformNormal(vUp, matRot);
			vRight = XMVector3TransformNormal(vRight, matRot);

			DstTransform->Set_State(STATE::RIGHT, vRight * fScale.x);
			DstTransform->Set_State(STATE::UP, vUp * fScale.y);
			DstTransform->Set_State(STATE::LOOK, vRot * fScale.z);

		}
		if (m_eRot == TRIGGER_ROT::Z)
		{
			vRot = { 1,0,0,0 };
			if (XMVectorGetX(XMVector3Dot(vSrcLook, vLook)) < 0)
				fAngel *= -1;
			matRot = XMMatrixRotationAxis(vRot, XMConvertToRadians(fAngel));
			vUp = XMVector3TransformNormal(vUp, matRot);
			vLook = XMVector3TransformNormal(vLook, matRot);

			DstTransform->Set_State(STATE::RIGHT, vRot * fScale.x);
			DstTransform->Set_State(STATE::UP,     vUp * fScale.y);
			DstTransform->Set_State(STATE::LOOK, vLook * fScale.z);
		}
		
	





		vPos = XMVectorSetW(vPos, 1.f);
		DstTransform->Set_State(STATE::POS, vPos);

		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);

		auto Target = CGameInstance::Get().Find_Trigger(m_iLevel, m_iTargetNumber).lock();

		if (NULL_FALSE(Target))
		{
			Target->TriggerToTrigger();
		}
		return S_OK;
	}

	return E_FAIL;
}

void CBatteryCase::Action_Event()
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::WORLD_EVENT)))
		return;

	auto pObj = static_pointer_cast<CTrigger>(CGameInstance::Get().Find_Trigger(m_iLevel,m_iTargetNumber).lock());

	if (NULL_TRUE(pObj))
		return;
	pObj->Set_Trigger();

	EVENT eEvent{};
	eEvent.eEvent = WORLD_EVENT::BOSS_SPAWN;
	CGameInstance::Get().Notify(WORLD_EVENT::BOSS_SPAWN, eEvent);
	m_eState = TRIGGER_STATE::IDLE;
}

unique_ptr<CBatteryCase>CBatteryCase::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CBatteryCase>(new CBatteryCase(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CBatteryCase");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CBatteryCase::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CBatteryCase>(new CBatteryCase(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CBatteryCase Clone");
		return nullptr;
	}

	return pInstance;

}