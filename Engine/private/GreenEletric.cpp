#include "GreenEletric.h"
#include "GameObject.h"
CGreenElectric::CGreenElectric(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CGreenElectric::CGreenElectric(const CGreenElectric& Prototype) : CTrigger(Prototype)
{
}

CGreenElectric::~CGreenElectric()
{
}

HRESULT CGreenElectric::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CGreenElectric::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	
	__super::Initialize(pArg);
	uint32_t iFlag =  ETOUI(TRIGGER_FLAG::ATTACHED);

	m_eEventTrigger = TRIGGER_EVENT::GELECTRIC;
	m_fRotationArrow = 10.f;
	
	if (m_bOtherTrigger)
		iFlag |= ETOUI(TRIGGER_FLAG::SHADER) | ETOUI(TRIGGER_FLAG::FTRIGGER) | ETOUI(TRIGGER_FLAG::OTHERTRIGGER);
	
	m_BindValue.fColor = { 0,1,0,1 };

	Set_Flag(iFlag, FLAGVALUE::ENABLE);
	return S_OK;
}
HRESULT CGreenElectric::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger )
{
	if(!Check_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER)))
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER) | ETOUI(TRIGGER_FLAG::SHADER), FLAGVALUE::DISABLE);
	return S_OK;

}
HRESULT CGreenElectric::Interaction(_float fTimeDelta,  _bool bOtherTrigger)
{
	Action_Trigger();
	return S_OK;
}
_bool CGreenElectric::offsetMatrix(_float4x4* pMatrix)
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
void CGreenElectric::Set_Trigger()
{
	Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER) | ETOUI(TRIGGER_FLAG::SHADER), FLAGVALUE::ENABLE);
}
HRESULT CGreenElectric::Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger )
{//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지

	
	return S_OK;
}
void CGreenElectric::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();

	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
		m_BindValue.fColor = { 0,1,0,1 };
	}
	else m_BindValue.fColor = { 1,1,1,1 };
	

}	

unique_ptr<CGreenElectric>  CGreenElectric::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CGreenElectric>(new CGreenElectric(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CGreenElectric");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CGreenElectric::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CGreenElectric>(new CGreenElectric(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CGreenElectric Clone");
		return nullptr;
	}

	return pInstance;

}