#include "WorldLight.h"
#include "GameInstance.h"

CWorldLight::CWorldLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CLightEffect(pDevice, pContext)
{

}
CWorldLight::CWorldLight(const CWorldLight& Prototye) :CLightEffect(Prototye)
{
}
CWorldLight::~CWorldLight()
{
}

HRESULT CWorldLight::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CWorldLight::Initialize(void* pArg)
{
	auto pDesc = static_cast<WORLDLIGHT_DESC*>(pArg);


	if (FAILED(Ready_Component()))
		return E_FAIL;

	__super::Initialize(pArg);

	LIGHT_DESC* pLIghtDesc = CGameInstance::Get().Get_LightToHandle(m_eHandle.iIndex, m_eHandle.iHandle);
	if (NULL_TRUE(pDesc) || NULL_TRUE(pLIghtDesc))
		return E_FAIL;

	m_LightOrigin = *pLIghtDesc;

	switch (pDesc->eWorldEventType)
	{
	case WORLD_EVENT::BOSS_LIGHT_FLICK:
		m_eLocalEventType = LIGHT_STATE::NONE;
		pLIghtDesc->vAmbient = _float4(0, 0, 0, 0);
		pLIghtDesc->vDiffuse = _float4(0, 0, 0, 0);
		pLIghtDesc->bLightStop = false;
		CGameInstance::Get().Add_Observers(pDesc->eWorldEventType, SHARED_THIS(CWorldLight));
		break;
	case WORLD_EVENT::BOSS_LIGHT_ON:
		m_eLocalEventType = LIGHT_STATE::NONE;
		CGameInstance::Get().Add_Observers(pDesc->eWorldEventType, SHARED_THIS(CWorldLight));
		break;
	case WORLD_EVENT::BOSS_LIGHT_OFF:
		m_eLocalEventType = LIGHT_STATE::NONE;
		CGameInstance::Get().Add_Observers(pDesc->eWorldEventType, SHARED_THIS(CWorldLight));
		break;
	case WORLD_EVENT::GENERATOR:
		m_eLocalEventType = LIGHT_STATE::NONE;
		CGameInstance::Get().Add_Observers(pDesc->eWorldEventType, SHARED_THIS(CWorldLight));
		break;
	case WORLD_EVENT::BOSS_TP:
		CGameInstance::Get().Add_Observers(pDesc->eWorldEventType, SHARED_THIS(CWorldLight));
		break;
	case WORLD_EVENT::END:
		break;
	}
	return S_OK;
}
void CWorldLight::Priority_Update(_float fTimeDelta)
{


}
void CWorldLight::Update(_float fTimeDelta)
{
}
void CWorldLight::Late_Update(_float fTimeDelta)
{
	_bool bUpdate{ false };
	LIGHT_DESC* pDesc = CGameInstance::Get().Get_LightToHandle(m_eHandle.iIndex, m_eHandle.iHandle);
	LIGHT_DESC* pDescOrigin = CGameInstance::Get().Get_LightToHandleOrigin(m_eHandle.iIndex, m_eHandle.iHandle);

	if (NULL_TRUE(pDesc) || NULL_TRUE(pDescOrigin))
		return;

	bUpdate = pDesc->bLightStop;
	if (!bUpdate)
	{
		switch (m_eLocalEventType)
		{
		case LIGHT_STATE::NONE:
			break;

		case LIGHT_STATE::LIGHT_BLINK1:
			Light_Blink(pDesc, fTimeDelta);
			break;

		case LIGHT_STATE::LIGHT_BLINK2:
			Light_Blink2(pDesc, fTimeDelta);
			break;

		case LIGHT_STATE::LIGHT_BLINK3:
			Light_Blink3(pDesc, fTimeDelta);
			break;

		case LIGHT_STATE::LIGHT_BLINK4:
			break;
		case LIGHT_STATE::LIGHT_ON:
			Light_ON(pDesc, fTimeDelta);
			break;		
		case LIGHT_STATE::LIGHT_OFF:
			Light_OFF(pDesc, fTimeDelta);
			break;
		case LIGHT_STATE::LIGHT_WORLD:
			Light_Blink_EVENT(pDesc, fTimeDelta);
			break;
		}
	}
	else
	{
		m_LightOrigin = *pDesc;
	}

}
HRESULT CWorldLight::Render()
{
	

	return S_OK;
}



json CWorldLight::Save_Data()
{

	nlohmann::json j;

	_float4x4 fWorld = { };

	XMStoreFloat4x4(&fWorld, m_pTransform->Get_World());

	_float3 fRight = { fWorld._11,fWorld._12,fWorld._13 };
	_float3 fUp = { fWorld._21,fWorld._22 ,fWorld._23 };
	_float3 fLook = { fWorld._31, fWorld._32,fWorld._33 };
	_float3 fPos = { fWorld._41,fWorld._42,fWorld._43 };


	return j;
}
void CWorldLight::OnNotify(const EVENT& eEvent)
{
	switch (eEvent.eEvent)
	{
	case WORLD_EVENT::BOSS_LIGHT_FLICK:
	{
		LIGHT_DESC* pLIghtDesc = CGameInstance::Get().Get_LightToHandle(m_eHandle.iIndex, m_eHandle.iHandle);
		*pLIghtDesc = m_LightOrigin;
		pLIghtDesc->bLightStop = false;
		m_eLocalEventType = LIGHT_STATE::LIGHT_WORLD;

		CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_LIGHT_OFF, SHARED_THIS(CWorldLight));
		break;
	}
	case WORLD_EVENT::BOSS_LIGHT_ON:
		m_eLocalEventType = LIGHT_STATE::LIGHT_ON;
		break;
	case WORLD_EVENT::BOSS_LIGHT_OFF:
	{
		CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_LIGHT_ON, SHARED_THIS(CWorldLight));
		LIGHT_DESC* pLIghtDesc = CGameInstance::Get().Get_LightToHandle(m_eHandle.iIndex, m_eHandle.iHandle);

		XMStoreFloat4(&m_LightOrigin.vAmbient, XMLoadFloat4(&pLIghtDesc->vAmbient) * 0.5f);
		XMStoreFloat4(&m_LightOrigin.vDiffuse, XMLoadFloat4(&pLIghtDesc->vDiffuse) * 0.5f);
		m_eLocalEventType = LIGHT_STATE::LIGHT_OFF;
		pLIghtDesc->bLightStop = false;
	}
		break;
	case WORLD_EVENT::GENERATOR:
		m_eLocalEventType = LIGHT_STATE::LIGHT_WORLD;
		break;

	case WORLD_EVENT::BOSS_TP:
		m_eLocalEventType = LIGHT_STATE::LIGHT_WORLD;
		break;

	}
}
void CWorldLight::Light_Blink(LIGHT_DESC* pDesc, const _float& fTimeDelta)
{
	m_fTick += fTimeDelta;
	m_fTickTwo += fTimeDelta;
	_float t = m_fTick / 1.f;
	if (!m_bControl)
	{
		XMStoreFloat4(&pDesc->vAmbient ,XMVectorLerp(XMLoadFloat4(&pDesc->vAmbient ),XMVectorSet(0,0,0,1.f),t));

	}
	else
	{
		XMStoreFloat4(&pDesc->vAmbient, XMVectorLerp(XMLoadFloat4(&pDesc->vAmbient), XMLoadFloat4(&m_LightOrigin.vAmbient), t));

	}

	if (t >= 1)
	{

		++m_iTickCnt;
	}

	if (m_iTickCnt >= 1)
	{
		m_bControl = !m_bControl;
		m_iTickCnt = 0;
		m_fTick = 0.f;
	}
	
}
void CWorldLight::Light_Blink2(LIGHT_DESC* pDesc,const _float& fTimeDelta)
{
	m_fTick += fTimeDelta;
	m_fTickTwo += fTimeDelta;
	_float t = m_fTick / 0.5f;
	if (!m_bControl)
	{
		XMStoreFloat4(&pDesc->vDiffuse, XMVectorLerp(XMLoadFloat4(&pDesc->vDiffuse), XMVectorSet(0, 0, 0, 1.f), t));

	}
	else
	{
		XMStoreFloat4(&pDesc->vDiffuse, XMVectorLerp(XMLoadFloat4(&pDesc->vDiffuse), XMLoadFloat4(&m_LightOrigin.vDiffuse), t));

	}

	if (t >= 1)
	{

		++m_iTickCnt;
	}

	if (m_iTickCnt >= 1)
	{
		m_bControl = !m_bControl;
		m_iTickCnt = 0;
		m_fTick = 0.f;
	}
}
void CWorldLight::Light_Blink3(LIGHT_DESC* pDesc,const _float& fTimeDelta)
{
}
void CWorldLight::Light_Blink4(LIGHT_DESC* pDesc, const _float& fTimeDelta)
{
}
void CWorldLight::Light_ON(LIGHT_DESC* pDesc, const _float& fTimeDelta)
{
	m_fTick += fTimeDelta;
	m_fTickTwo += fTimeDelta;
	_float t = m_fTick / 1.2f;
	if (!m_bControl)
	{
		XMStoreFloat4(&pDesc->vAmbient, XMVectorLerp(XMVectorSet(0, 0, 0, 1.f), XMLoadFloat4(&m_LightOrigin.vAmbient), t));
		XMStoreFloat4(&pDesc->vDiffuse, XMVectorLerp(XMVectorSet(0, 0, 0, 1.f), XMLoadFloat4(&m_LightOrigin.vDiffuse), t));

	}

	if (t >= 1)
	{
		m_eLocalEventType == LIGHT_STATE::NONE;
	}
}
void CWorldLight::Light_OFF(LIGHT_DESC* pDesc, const _float& fTimeDelta)
{
	pDesc->vAmbient = _float4(0, 0, 0, 1);
	pDesc->vDiffuse = _float4(0, 0, 0, 1);
	m_eLocalEventType = LIGHT_STATE::NONE;
}
void CWorldLight::Light_Blink_EVENT(LIGHT_DESC* pDesc, const _float& fTimeDelta)
{
	m_fTick += fTimeDelta;

	if (m_fTick > 0.2f)
	{
		++m_iTickCnt;
		m_fTick = 0.f;
	}
	
	if (m_iTickCnt < m_LightPatternTable.size())
	{
		uint32_t iSize = m_LightPatternTable.size() - 1;
		uint32_t iPattern = min(m_iTickCnt, iSize);

		if (m_LightPatternTable[iPattern] == true)
		{
			pDesc->vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
			pDesc->vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
		}
		else
		{
			pDesc->vDiffuse = m_LightOrigin.vDiffuse;
			pDesc->vAmbient = m_LightOrigin.vAmbient;
		}
	}
	else
	{
		m_fTickTwo += fTimeDelta;
		_float t = m_fTickTwo / 0.5f;
		if (!m_bControl)
		{
			XMStoreFloat4(&pDesc->vAmbient, XMVectorLerp(XMLoadFloat4(&m_LightOrigin.vAmbient), XMVectorSet(0, 0, 0, 1.f), t));

			XMStoreFloat4(&pDesc->vDiffuse, XMVectorLerp(XMLoadFloat4(&m_LightOrigin.vDiffuse), XMVectorSet(0, 0, 0, 1.f), t));
			if (pDesc->vAmbient.x <= 0)
				m_bControl = true;
		}
		else
		{
			XMStoreFloat4(&pDesc->vAmbient, XMVectorLerp(XMVectorSet(0, 0, 0, 1.f), XMLoadFloat4(&m_LightOrigin.vAmbient),t));

			XMStoreFloat4(&pDesc->vDiffuse, XMVectorLerp(XMVectorSet(0, 0, 0, 1.f), XMLoadFloat4(&m_LightOrigin.vDiffuse), t));
			if (XMVectorGetX(XMVector4Length(XMLoadFloat4(&pDesc->vAmbient))) >= XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_LightOrigin.vAmbient))))
			{
				m_fTickTwo = m_iTickCnt = 0;
				m_bControl = false;
			}
				
		}
		
	}

}
HRESULT CWorldLight::Ready_Component()
{
	
	return S_OK;
}

void CWorldLight::Load_Data(void* pDesc, const json& j)
{
	


}
unique_ptr<CWorldLight> CWorldLight::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CWorldLight>(new CWorldLight(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed WorldLight");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CWorldLight::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CWorldLight(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed TriggerObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
