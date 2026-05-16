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
	m_bOtherTrigger = pDesc->bTrigger;

	return S_OK;
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
