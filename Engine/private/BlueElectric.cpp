#include "BlueElectric.h"
#include "GameObject.h"
CBlueElectric::CBlueElectric(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CBlueElectric::CBlueElectric(const CBlueElectric& Prototype) : CTrigger(Prototype)
{
}

CBlueElectric::~CBlueElectric()
{
}

HRESULT CBlueElectric::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlueElectric::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	__super::Initialize(pArg);
	uint32_t iFlag{0};
	m_eEventTrigger = TRIGGER_EVENT::BELECTRIC;
	if (m_bOtherTrigger)
			iFlag = ETOUI(TRIGGER_FLAG::ATTACHED) | ETOUI(TRIGGER_FLAG::FTRIGGER) | ETOUI(TRIGGER_FLAG::SHADER);
	
	m_BindValue.fColor = { 0,0,1,1 };
	Set_Flag(iFlag, FLAGVALUE::ENABLE);
	m_fRotationArrow = 10.f;
	m_bOtherTrigger = true;
	return S_OK;
}
HRESULT CBlueElectric::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	return S_OK;

}
HRESULT CBlueElectric::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		Action_Trigger();

	return S_OK;
}
HRESULT CBlueElectric::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지


	return S_OK;
}
_bool CBlueElectric::offsetMatrix(_float4x4* pMatrix)
{
	auto pObj = m_pParent.lock();
	auto pDstTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj) || (NULL_TRUE(pDstTransform) && !m_bOtherTrigger))
		return false;
	auto pTransform = pObj->Get_Transform().lock();

	_matrix matOffset = XMMatrixIdentity();

	_vector SrcRight = XMVector3Normalize(pTransform->Get_World().r[0]);
	_vector SrcLook  = XMVector3Normalize(pTransform->Get_World().r[2]);
	_vector SrcPos = pTransform->Get_World().r[3];
	
	_vector DstPos = pDstTransform->Get_World().r[3];
	_float3 fMax = pTransform->Get_Max();
	_float3 fMin = pTransform->Get_Min();

	_vector vCenter = (XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f;
	_vector vLook = XMVector3Normalize(DstPos - SrcPos);

	if (fabsf(XMVectorGetX(XMVector3Dot(SrcLook,vLook))) > fabsf(XMVectorGetX(XMVector3Dot(SrcRight, vLook))))
		vLook = XMVectorSetX(XMVectorSetY(vLook, 0.f), 0.f);
	else
		vLook = XMVectorSetZ(XMVectorSetY(vLook, 0.f), 0.f);

	vCenter = XMVector3TransformCoord(vCenter, pTransform->Get_World());


	SrcPos = vCenter + vLook * 5.f;

	matOffset.r[3] = SrcPos;
	XMStoreFloat4x4(pMatrix, matOffset);
	return true;
}
void CBlueElectric::Set_Trigger()
{
	Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::ENABLE);
}
void CBlueElectric::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_FALSE(TriggerCheck) && ETOUI(TRIGGER_FLAG::OTHERTRIGGER))
		TriggerCheck->Set_Trigger();

}

unique_ptr<CBlueElectric>  CBlueElectric::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CBlueElectric>(new CBlueElectric(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CBlueElectric");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CBlueElectric::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CBlueElectric>(new CBlueElectric(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CBlueElectric Clone");
		return nullptr;
	}

	return pInstance;

}