#include "ElectricPannel.h"
#include "GameInstance.h"
CElectricPannel::CElectricPannel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CElectricPannel::CElectricPannel(const CElectricPannel& Prototype) : CTrigger(Prototype)
{
}

CElectricPannel::~CElectricPannel()
{
}

HRESULT CElectricPannel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectricPannel::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	__super::Initialize(pArg);

	m_eEventTrigger = TRIGGER_EVENT::PANNEL;
	m_fRotationArrow = 10.f;

	return S_OK;
}
HRESULT CElectricPannel::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	m_BindValue.fColor = { 1,1,1,1 };
	return S_OK;

}
HRESULT CElectricPannel::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	if (Check_Flag(ETOUI(TRIGGER_FLAG::CANCLE)))
		return S_OK;

	Action_Trigger();
	return S_OK;
}
_bool CElectricPannel::offsetMatrix(_float4x4* pMatrix)
{
	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj) || (NULL_TRUE(pDstTransform) && !m_bOtherTrigger))
		return false;
	auto pTransform = pObj->Get_Transform().lock();

	_matrix matOffset = XMMatrixIdentity();

	_vector SrcPos = pTransform->Get_World().r[3];
	_vector DstPos = pDstTransform->Get_World().r[3];
	_float3 fMax = pTransform->Get_Max();
	_float3 fMin = pTransform->Get_Min();

	_vector vCenter = (XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f;
	_vector vLook = pDstTransform->Get_World().r[2];

	vCenter = XMVector3TransformCoord(vCenter, pTransform->Get_World());


	SrcPos = vCenter + vLook * 3.f;

	matOffset.r[3] = SrcPos;
	XMStoreFloat4x4(pMatrix, matOffset);
	return true;
}
HRESULT CElectricPannel::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}
void CElectricPannel::Set_Trigger()
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		return;

	Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::ENABLE);

}
void CElectricPannel::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iLevel,m_iTargetNumber).lock();
	if (Check_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER)))
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->TriggerToTrigger();

	}
	else
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Flag(Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)), FLAGVALUE::DISABLE);
	}
	
}

unique_ptr<CElectricPannel>  CElectricPannel::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CElectricPannel>(new CElectricPannel(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CElectricPannel");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CElectricPannel::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CElectricPannel>(new CElectricPannel(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CElectricPannel Clone");
		return nullptr;
	}

	return pInstance;

}