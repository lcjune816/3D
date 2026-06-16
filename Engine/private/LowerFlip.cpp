#include "LowerFlip.h"
#include "GameInstance.h"
CLowerFlip::CLowerFlip(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CLowerFlip::CLowerFlip(const CLowerFlip& Prototype) : CTrigger(Prototype)
{
}

CLowerFlip::~CLowerFlip()
{
}

HRESULT CLowerFlip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLowerFlip::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::PUZZLEROT;
	Set_Flag(ETOUI(TRIGGER_FLAG::ATTACHED) | ETOUI(TRIGGER_FLAG::FTRIGGER) | ETOUI(TRIGGER_FLAG::PAUSE) , FLAGVALUE::ENABLE);
	m_eState = TRIGGER_STATE::IDLE;
	return S_OK;
}

HRESULT CLowerFlip::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

HRESULT CLowerFlip::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	//활성화 후 반바퀴
	//손 회수되면 다시 복귀인데	
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::ACTION:
		if (Start_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::PAUSE;
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER) | ETOUI(TRIGGER_FLAG::PAUSE), FLAGVALUE::DISABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::ENABLE);
			
		}
		else
			Action_Trigger();
		break;
	case TRIGGER_STATE::RETURN:
		if (End_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::PAUSE;
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER) | ETOUI(TRIGGER_FLAG::PAUSE), FLAGVALUE::DISABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
		}
		else
			Action_Trigger();
		break;

	case TRIGGER_STATE::PAUSE:
		
		if (Timer_Flag(ETOUI(TRIGGER_FLAG::PAUSE) , FLAGVALUE::ENABLE, fTimeDelta))
				m_eState = TRIGGER_STATE::IDLE;
		
		break;
	}
	
	return S_OK;
}
HRESULT CLowerFlip::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

void CLowerFlip::Set_Trigger()
{
	if (m_eState != TRIGGER_STATE::IDLE)
		return;

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

void CLowerFlip::Action_Trigger()
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;
	pObj->Get_Transform().lock()->Rotation_Origin(XMLoadFloat4(&m_fRotation), m_fAngle);

	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iLevel,m_iTargetNumber).lock();
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);

	}
}
_bool CLowerFlip::offsetMatrix(_float4x4* pMatrix)
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::PAUSE)))
		return false;

	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj) || (NULL_TRUE(pDstTransform) && !m_bOtherTrigger))
		return false;
	auto pTransform = pObj->Get_Transform().lock();

	_float3 Scale = pDstTransform->Get_Scaled();

	_matrix matRot = XMMatrixIdentity();


	_matrix matOffset = XMMatrixIdentity();

	_vector SrcPos = pTransform->Get_World().r[3];
	_vector DstPos = pDstTransform->Get_World().r[3];
	_float3 fMax = pTransform->Get_Max();
	_float3 fMin = pTransform->Get_Min();
	
	_vector vCenter = {};
	_vector vLocal = (XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f;

	_vector vLook = XMVector3Normalize(SrcPos - DstPos);
	_vector vRight = XMVector3Cross(pTransform->Get_World().r[1], vLook);
	_vector vUp    = XMVector3Cross(vLook,vRight);
	vLook = XMVector3Cross(vRight, vUp);
	vCenter = XMVector3TransformCoord(vLocal, pTransform->Get_World());
	matOffset.r[0] = XMVector3Normalize(vRight);
	matOffset.r[1] = XMVector3Normalize(vUp);
	matOffset.r[2] = XMVector3Normalize(vLook);
	matOffset.r[3] = XMVectorSetW(SrcPos - XMVector3Normalize(vLook) * 0.5f,1.f);
	XMStoreFloat4x4(pMatrix, matOffset);
	return true;
}
unique_ptr<CLowerFlip>CLowerFlip::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CLowerFlip>(new CLowerFlip(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CLowerFlip");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CLowerFlip::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CLowerFlip>(new CLowerFlip(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CLowerFlip Clone");
		return nullptr;
	}

	return pInstance;

}