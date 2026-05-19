#include "Trigger.h"
#include "GameObject.h"
CTrigger::CTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CComponent{pDevice, pContext}
{
}

CTrigger::CTrigger(const CTrigger& Prototype) : CComponent(Prototype)
{
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);
	m_bOtherTrigger  = pDesc->bTrigger;
	m_fEndAngle = pDesc->fFrameTickTime;
	m_fMaxFrameTime  = pDesc->fMaxFrameTime;
	m_fRotationArrow = pDesc->fArrrowRotation;
	switch (pDesc->eRot)
	{
	case TRIGGER_ROT::X:
		m_fRotation = { 1.f,0.f,0,0 };
		break;
	case TRIGGER_ROT::Y:
		m_fRotation = { 0.f,1.f,0,0 };
		break;
	case TRIGGER_ROT::Z:
		m_fRotation = { 0.f,0.f,1.f,0 };
		break;
	}
	return S_OK;
}
_bool						CTrigger::Start_Rotation(const _float& fTimeDelta )
{

	m_fFrameTick += fTimeDelta;

	_float t = min(1.f, m_fFrameTick / m_fMaxFrameTime);
	m_fAngle = m_fStartAngle + (m_fEndAngle - m_fStartAngle) * t;

	
	if (t >= 1.f)
	{
		m_fFrameTick = 0.f;
		m_fAngle = m_fEndAngle;
		return true;
	}
	return false;
}
_bool						CTrigger::End_Rotation(const _float& fTimeDelta)
{
	m_fFrameTick += fTimeDelta;

	_float t = min(1.f, m_fFrameTick / m_fMaxFrameTime);
	m_fAngle = m_fEndAngle + (m_fStartAngle - m_fEndAngle) * t;

	if (t >= 1.f)
	{
		m_fFrameTick = 0.f;
		m_fAngle = m_fStartAngle;
		return true;
	}

	return false;
}
_bool CTrigger::Timer_Flag(TRIGGER_FLAG flag, FLAGVALUE eValue , const _float& fTimeDelta)
{
	m_fFrameTick += fTimeDelta;

	if (m_fFrameTick >= 0.2f)
	{
		m_fFrameTick = 0.f;
		++m_fFlagCnt;
		return false;
	}

	if (m_fFlagCnt > 5)
	{
		m_fFlagCnt = 0;
		m_fFrameTick = 0;
		Set_Flag(ETOUI(flag), eValue);
		return true;
	}

	return false;
}
HRESULT CTrigger::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}



void CTrigger::Set_Parent(shared_ptr<class CGameObject> pObj)
{
	m_pParent = pObj; 
}

_bool				CTrigger::Set_DstTransform(shared_ptr<CTransform> pTransform)
{
	auto Transform = m_pDstTransform.lock();
	if (NULL_TRUE(Transform))
	{
		m_pDstTransform = pTransform;
		return true;
	}

	return false;
}

void CTrigger::Set_Flag(uint32_t iFlag, FLAGVALUE eValue)
{

	switch (eValue)
	{
	case FLAGVALUE::ENABLE:

		m_iFlag |= iFlag;
		break;

	case FLAGVALUE::DISABLE:

		m_iFlag &= ~iFlag;
		break;

	case FLAGVALUE::TOGGLE:

		m_iFlag ^= iFlag;
		break;

	case FLAGVALUE::RESET:
		
		m_iFlag = 0 ;
		break;
	
	}
}

_bool CTrigger::Check_Flag(uint32_t iFlag)
{
	if (m_iFlag & iFlag)
		return true;

	return false;
}

void CTrigger::TriggerToTrigger()
{
}

void CTrigger::Bind_Resource(shared_ptr<CShader> pShader, const _char* pConstantName)
{
	_float4 fColor = { 1,1,1,1 };
	if(ETOUI(TRIGGER_FLAG::SHADER) & m_iFlag)
		pShader->Bind_RawValue(pConstantName, &m_BindValue.fColor, sizeof m_BindValue.fColor);
	else
		pShader->Bind_RawValue(pConstantName, &fColor, sizeof m_BindValue.fColor);

}

_bool CTrigger::offsetMatrix(_float4x4* pMatrix)
{
	return false;
}

