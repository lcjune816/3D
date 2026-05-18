#include "LowerFlip_Flip.h"
#include "GameObject.h"
CLowerFlip_Flip::CLowerFlip_Flip(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CLowerFlip_Flip::CLowerFlip_Flip(const CLowerFlip_Flip& Prototype) : CTrigger(Prototype)
{
}

CLowerFlip_Flip::~CLowerFlip_Flip()
{
}

HRESULT CLowerFlip_Flip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLowerFlip_Flip::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::PUZZLEROT;

	Set_Flag( ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
	m_eState = TRIGGER_STATE::IDLE;
	return S_OK;
}

HRESULT CLowerFlip_Flip::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

HRESULT CLowerFlip_Flip::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	//활성화 후 반바퀴
		//손 회수되면 다시 복귀인데	
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		if (Check_Flag(ETOUI(TRIGGER_FLAG::PAUSE)))
		{

		}
		break;
	case TRIGGER_STATE::ACTION:
		if (!Start_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::IDLE;
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::ENABLE);
		}
		else
			Action_Trigger();
		break;
	case TRIGGER_STATE::RETURN:
		if (!End_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::IDLE;
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::DISABLE);
		}
		else
			Action_Trigger();
		break;

	case TRIGGER_STATE::PAUSE:
		break;
	}
	return S_OK;
}
HRESULT CLowerFlip_Flip::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

void CLowerFlip_Flip::Set_Trigger()
{

	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		m_eState = TRIGGER_STATE::ACTION;
		return;
	}
	else if (Check_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER)))
	{
		m_eState = TRIGGER_STATE::RETURN;
		return;
	}
}

void CLowerFlip_Flip::Action_Trigger()
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;

	pObj->Get_Transform().lock()->Apply_Rotation(XMLoadFloat4(&m_fRotation), m_fAngle);
}

unique_ptr<CLowerFlip_Flip>CLowerFlip_Flip::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CLowerFlip_Flip>(new CLowerFlip_Flip(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CLowerFlip_Flip");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CLowerFlip_Flip::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CLowerFlip_Flip>(new CLowerFlip_Flip(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CLowerFlip_Flip Clone");
		return nullptr;
	}

	return pInstance;

}