#include "BatteryCase.h"
#include "GameObject.h"
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

	m_eEventTrigger = TRIGGER_EVENT::BATTERYCASE;
	m_fRotationArrow = 0.25f;

	return S_OK;
}

HRESULT CBatteryCase::Interaction( _float fTimeDelta,  _bool bOtherTrigger)
{
	if (m_bTriggerOn) return E_FAIL;

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
		_vector vPos = SrcTransform->Get_State(STATE::POS);
		vPos  += XMVectorSet(0,6.f,0,0);
		_vector vSrcLook = SrcTransform->Get_World().r[2];

		_vector vUp = { 0,1,0,0 };
		_vector vRight = { 1,0,0,0 };
		_vector vLook = { 0,0,1,0 };
		_float fAngel = 90.f;
		_float SrcX = XMVectorGetX(SrcTransform->Get_State(STATE::LOOK));
		_float SrcZ = XMVectorGetX(SrcTransform->Get_State(STATE::LOOK));


		_float X{}, Z{}, MX{}, MZ{};
		_matrix matRot{};
		X = XMVectorGetX(XMVector3Dot(XMVector3Length(vPos), vRight));
		Z = XMVectorGetX(XMVector3Dot(XMVector3Length(vPos), vLook));
		MX = XMVectorGetX(XMVector3Dot(XMVector3Length(vPos), -vRight));
		MZ = XMVectorGetX(XMVector3Dot(XMVector3Length(vPos), -vLook));

	
		if (XMVectorGetX(XMVector3Dot(vSrcLook, vLook)) > 0)
			fAngel *= -1;

		matRot = XMMatrixRotationAxis(vRight, XMConvertToRadians(fAngel));
		vUp = XMVector3TransformNormal(vUp, matRot);
		vLook = XMVector3TransformNormal(vLook, matRot);
		
	
	

		vPos = XMVectorSetW(vPos, 1.f);
		DstTransform->Set_State(STATE::RIGHT, vRight);
		DstTransform->Set_State(STATE::UP, vUp);
		DstTransform->Set_State(STATE::LOOK, vLook);

		DstTransform->Set_State(STATE::POS, vPos);

		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);

		return S_OK;
	}

	return E_FAIL;
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