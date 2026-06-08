#include "RollupDoor.h"
#include "GameInstance.h"
CRollupDoor::CRollupDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CRollupDoor::CRollupDoor(const CRollupDoor& Prototype) : CTrigger(Prototype)
{
}

CRollupDoor::~CRollupDoor()
{
}

HRESULT CRollupDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollupDoor::Initialize(void* pArg)
{
	m_eEventTrigger = TRIGGER_EVENT::ROLLUPDOOR;
	m_fRotationArrow = 15.f;
	
	__super::Initialize(pArg);
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);
	
	m_eState = TRIGGER_STATE::ACTION;
	if (WORLD_EVENT::ROLLUP_DOOR == pDesc->eWroldEvent)
	{
 		m_eState = TRIGGER_STATE::WORLD;
	}
	
	Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
	return S_OK;
}

HRESULT CRollupDoor::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

HRESULT CRollupDoor::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	switch (m_eState)
	{
	case	TRIGGER_STATE::IDLE:
		break;

	case	TRIGGER_STATE::ACTION:
			Down_Door(fTimeDelta);
		break;
	case	TRIGGER_STATE::WORLD:
		Down_DeadDoor(fTimeDelta);
		break;
	
	}
	
	return S_OK;
}

HRESULT CRollupDoor::Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger)
{
	return S_OK;
}

void CRollupDoor::Set_Trigger()
{
}

void CRollupDoor::TriggerToTrigger()
{

	Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
	if (m_eState == TRIGGER_STATE::WORLD)
	{
		OnNotify({});
	}
}

void CRollupDoor::OnNotify(const EVENT& event)
{
	auto pObj = CGameInstance::Get().Get_ObjectPtr(3, L"Layer_Boss", "Boss_Teacher");
	Set_DstTransform(pObj->Get_Transform().lock());
	auto pParent= m_pParent.lock();
	if (NULL_TRUE(pParent))
		return;
	auto pTransform = pParent->Get_Transform().lock();
	if (NULL_TRUE(pTransform))
		return;
	auto pDestTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pDestTransform))
		return;

	_vector vPos = pTransform->Get_State(STATE::POS);
	_vector vSrcPos = XMVectorSetY(vPos, 0.f);
	_vector vDestPos = pDestTransform->Get_State(STATE::POS);
	 m_fCurHeight = XMVectorGetY(vPos);
	m_fSrcLength = XMVectorGetX(XMVector3Length(vSrcPos));
	m_fDestLength = XMVectorGetX(XMVector3Length(vSrcPos - vDestPos));

	EVENT eEvent;
	eEvent.pArg = &m_fWorldTime;
	CGameInstance::Get().Notify(WORLD_EVENT::BOSS_DEAD, eEvent);
	XMStoreFloat3(&m_fDestPos, vDestPos);
}

void CRollupDoor::Down_Door(const _float& fTimeDelta)
{

	if (!Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		return;

		m_fFrameTick += fTimeDelta;
	if (m_fFrameTick > 0.001)
	{
		m_fFrameTick = 0.f;
		Action_Trigger(fTimeDelta);
		++m_fFrameTime;
	}

	if (m_fFrameTime > 300.f)
	{
		m_eState = TRIGGER_STATE::IDLE;
		m_fFrameTime = 0.f;
		Set_Flag(ETOUI(TRIGGER_FLAG::CANCLE), FLAGVALUE::ENABLE);
	}
}

void CRollupDoor::Down_DeadDoor(const _float& fTimeDelta)
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;
	auto pTransform = pObj->Get_Transform().lock();
	if (NULL_TRUE(pTransform))
		return;
	auto pDestTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pDestTransform))
		return;


	_vector vPos = pTransform->Get_State(STATE::POS);
	_vector vSrcPos = XMVectorSetY(vPos,0.f);
	_vector vDestPos = pDestTransform->Get_State(STATE::POS);
	_float3 fScale = pDestTransform->Get_Scaled();

	m_fFrameTick += fTimeDelta;
	 
	
	m_fWorldTime = min(1.f, m_fFrameTick / 1.4f);
	_vector vPosLerp = XMVectorLerp(XMLoadFloat3(&m_fDestPos), vSrcPos, m_fWorldTime);
	_float Height = m_fCurHeight + ( 1- m_fCurHeight) * m_fWorldTime;
		
	pDestTransform->Set_State(STATE::POS, XMVectorSetW(vPosLerp,1.f));
	pTransform->Set_State(STATE::POS,XMVectorSetW(XMVectorSetY(vPos,Height),1.f));


	if (m_fWorldTime >= 1.f)
	{
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_DEAD, {});
		pTransform->Set_State(STATE::POS, XMVectorSetY(vPos, 5));
		m_eState = TRIGGER_STATE::PAUSE;
	}
}

void CRollupDoor::Action_Trigger(_float fTimeDelta)
{
	auto pObj = m_pParent.lock();
	if(NULL_TRUE(pObj))
		return;
	pObj->Get_Transform().lock()->Go_Up(fTimeDelta, m_fRotationArrow);
}

unique_ptr<CRollupDoor>CRollupDoor::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CRollupDoor>(new CRollupDoor(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CRollupDoor");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CRollupDoor::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CRollupDoor>(new CRollupDoor(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CRollupDoor Clone");
		return nullptr;
	}

	return pInstance;

}