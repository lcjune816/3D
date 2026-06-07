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
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);
	__super::Initialize(pArg);
	if (pDesc->eWroldEvent == WORLD_EVENT::BATTERY)
	{
		m_eState = TRIGGER_STATE::PAUSE;
		CGameInstance::Get().Add_Observers(WORLD_EVENT::BATTERY, SHARED_THIS(CBattery));
	}
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CBattery::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	
		
	return S_OK;
}
HRESULT CBattery::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		Drop(fTimeDelta);
		break;
	case TRIGGER_STATE::ACTION:
		Attached();
		if (Action_Trigger())
			m_eState = TRIGGER_STATE::PAUSE;
		break;
	case TRIGGER_STATE::RETURN:
		break;

	case TRIGGER_STATE::PAUSE:
		break;
	case TRIGGER_STATE::WORLD:
		parabola(fTimeDelta);
		break;

	}


	
	return S_OK;
}
void CBattery::Attached()
{
	_vector vPos{};
	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj))
		return;
	auto pTransform = pObj->Get_Transform().lock();
	_float3 fScale = pTransform->Get_Scaled();
	_matrix SrcWorld = pTransform->Get_World();
	_matrix DstWorld = pDstTransform->Get_World();

	_vector vWorldSrcPos = SrcWorld.r[3];

	_vector vWorldDstPos = DstWorld.r[3];
	_vector vWorldDstLook = DstWorld.r[2];

	_float3 SrcMax = pTransform->Get_Max();
	_float3 SrcMin = pTransform->Get_Min();

	_vector SrcCenter = (XMLoadFloat3(&SrcMax) + XMLoadFloat3(&SrcMin)) * 0.5f;
	_vector Right =  XMVector3Normalize(pDstTransform->Get_State(STATE::RIGHT));
	_vector Up    =  XMVector3Normalize(pDstTransform->Get_State(STATE::UP));
	_vector Look  =  XMVector3Normalize(pDstTransform->Get_State(STATE::LOOK));

	
	_vector SrcPivot =
		Right * XMVectorGetX(SrcCenter) +
		Up * XMVectorGetY(SrcCenter) +
		Look * XMVectorGetX(SrcCenter);

	vPos = vWorldDstPos - SrcPivot - Look * 3.f;

	vPos = XMVectorSetW(vPos, 1.f);

	pTransform->Set_State(STATE::RIGHT, Right* fScale.x);
	pTransform->Set_State(STATE::UP,    Up * fScale.y );
	pTransform->Set_State(STATE::LOOK, Look * fScale.z);
	pTransform->Set_State(STATE::POS, vPos - XMVectorSet(0, 2, 0, 0));
	
}
void CBattery::Drop(const _float& fTimeDelta)
{
	_vector vPos{};
	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj))
		return;
	auto pTransform = pObj->Get_Transform().lock();

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
void CBattery::parabola(const _float& fTimeDelta)
{

	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj))
		return;
	auto pTransform = pObj->Get_Transform().lock();
	_vector vLook = XMVectorSetY(XMVector3Normalize(pTransform->Get_State(STATE::LOOK)),0.f);
	m_fDropTime += fTimeDelta;
	_float t = m_fDropTime / 0.5f;
	

	_vector FinalPos = pTransform->Get_State(STATE::POS) + vLook * 40.f * fTimeDelta;
	_float Height = m_fHeight + 40 * t * (1 - t);
	pTransform->Set_State(STATE::POS,XMVectorSetW(XMVectorSetY(FinalPos, Height),1.f));
	_float fY = pTransform->Get_Min().y;
	if (XMVectorGetY(FinalPos) < -fY)
	{
		m_fDropTime = 0.f;
		m_eState = TRIGGER_STATE::IDLE;
	}
}
void CBattery::Set_Trigger()
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		m_eState = TRIGGER_STATE::ACTION;
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
	}
		
}
void CBattery::OnNotify(const EVENT& event)
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;
	auto pTransform = pObj->Get_Transform().lock();
	m_eState = TRIGGER_STATE::WORLD;
	m_fHeight = XMVectorGetY(pTransform->Get_State(STATE::POS));

}
_bool CBattery::Action_Trigger()
{
	auto Target = CGameInstance::Get().Find_Trigger(m_iLevel, m_iTargetNumber).lock();
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(Target) || NULL_TRUE(pObj))
		return false;
	Target->Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
	if (SUCCEEDED(static_pointer_cast<CBatteryCase>(Target)->Action_Trigger(m_pParent.lock()->Get_Transform())))
	{
		pObj->Set_EndObject(true);
		Disconnect_Transform();
		Set_Flag(ETOUI(TRIGGER_FLAG::CANCLE), FLAGVALUE::ENABLE);
		return true;
	}
	return false;
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