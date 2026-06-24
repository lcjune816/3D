#include "CWorldParticle.h"
#include "GameInstance.h"

CWorldParticle::CWorldParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CParticleObject(pDevice, pContext)
{

}
CWorldParticle::CWorldParticle(const CWorldParticle& Prototye) :CParticleObject(Prototye)
{
}
CWorldParticle::~CWorldParticle()
{
}

HRESULT CWorldParticle::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CWorldParticle::Initialize(void* pArg)
{
	auto desc = static_cast<WORLDPARTICLE_DESC*>(pArg);
	
	if (desc->eFileType == FILE_MODE::LOAD)
		Load_Data(desc, desc->j);
	
	m_eParticleType		= desc->eParticleType;
	m_eParticleEmitTypeOrigin = m_eParticleEmitType = desc->eParticleEmit;
	m_iLevelIndex		= desc->iLevel;
	m_strPathName[ETOUI(PATHNAME::SHADER)]  = desc->PathName[ETOUI(PATHNAME::SHADER)];
	m_strPathName[ETOUI(PATHNAME::BUFFER)]  = desc->PathName[ETOUI(PATHNAME::BUFFER)];
	m_strPathName[ETOUI(PATHNAME::TEXTURE)] = desc->PathName[ETOUI(PATHNAME::TEXTURE)];

	switch (desc->eParticleType)
	{
	case WORLD_EVENT::GENERATOR:
		CGameInstance::Get().Add_Observers(WORLD_EVENT::GENERATOR, SHARED_THIS(CWorldParticle));
		m_eParticleEmitType = PARTICLE::END;
		break;
	case WORLD_EVENT::BOSS_SPAWN:
		CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_SPAWN, SHARED_THIS(CWorldParticle));
		CGameInstance::Get().Add_RenderToParticle(SHARED_THIS(CWorldParticle));
		m_eParticleEmitType = PARTICLE::FOG_CONTROLLER;;
		break;
	}
	if (desc->eParticleEmit == PARTICLE::FOG)
		m_iPassIndex = 1;
	else m_iPassIndex = 0;
	if (desc->eParticleEmit != PARTICLE::FOG_CONTROLLER)
	{
		if (FAILED(Ready_Component()))
			return E_FAIL;

		__super::Initialize(desc);
	}
	return S_OK;
}
void CWorldParticle::Priority_Update(_float fTimeDelta)
{

	
}
void CWorldParticle::Update(_float fTimeDelta)
{
	switch (m_eParticleEmitType)
	{
	case PARTICLE::FOG:
		m_bEndCycle = m_pVIBufferCom->Fog_Spread(fTimeDelta);

		break;
	case PARTICLE::SPARK:
		m_pVIBufferCom->Spark(fTimeDelta);
		break;
	case PARTICLE::FOG_SMALL:
		m_bEndCycle = m_pVIBufferCom->Steam(fTimeDelta);
		break;
	case PARTICLE::FOG_CONTROLLER:
		Fog_Controller(fTimeDelta);
		break;

	case PARTICLE::END:
		break;
	}

}
void CWorldParticle::Late_Update(_float fTimeDelta)
{
	if (m_eParticleEmitType != PARTICLE::FOG_CONTROLLER)
	{
		if (m_eParticleEmitType != PARTICLE::SPARK)
			CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(CWorldParticle));
		else
			CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLOOM_BEFORE, SHARED_THIS(CWorldParticle));
	}
		
}
HRESULT CWorldParticle::Render()
{
	_float4x4 matWorld{};
	_float4 fColor = { 0.5,0.5f,0.5f,1.f };

	XMStoreFloat4x4(&matWorld, m_pTransform->Get_World());

	m_pTransform->Bind_Matrix(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_RawValue("g_vCamePosition", CGameInstance::Get().Get_CamPosition(), sizeof _vector);
	_float4 fText = *CGameInstance::Get().ColorTester();
	m_pShaderCom->Bind_RawValue("g_Color",&fText, sizeof _float4);
	if(m_iTextureID != -1)
		m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureID));
	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();
	
	
	//if (m_bBoxColor)
	//	fColor = { 1.f,0.f,0.f,1.f };
	//m_pBoxShader->Bind_Matrix("g_World", &matWorld);
	//m_pBoxShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	//m_pBoxShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	//m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));
	//m_pBoxShader->Begin(0);
	//
	//m_pBoxMesh->Bind_Resource();
	//m_pBoxMesh->Render();
	//
	//
	//fColor = { 0.f,0.f,0.f,0.f };
	//m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));

	return S_OK;
}



json CWorldParticle::Save_Data()
{

	nlohmann::json j;

	_float4x4 fWorld = { };

	XMStoreFloat4x4(&fWorld, m_pTransform->Get_World());

	_float3 fRight = { fWorld._11,fWorld._12,fWorld._13 };
	_float3 fUp = { fWorld._21,fWorld._22 ,fWorld._23 };
	_float3 fLook = { fWorld._31, fWorld._32,fWorld._33 };
	_float3 fPos = { fWorld._41,fWorld._42,fWorld._43 };


	j["Right"] = { fWorld._11,fWorld._12,fWorld._13 };
	j["Up"] = { fWorld._21,fWorld._22 ,fWorld._23 };
	j["Look"] = { fWorld._31, fWorld._32,fWorld._33 };
	j["fPos"] = { fWorld._41,fWorld._42,fWorld._43 };
	string Path[ETOUI(PATHNAME::END)]{};
	 CGameInstance::Get().MultiByteWstringToChar(m_strPathName[ETOUI(PATHNAME::SHADER)] ,Path[ETOUI(PATHNAME::SHADER)] );
	 CGameInstance::Get().MultiByteWstringToChar(m_strPathName[ETOUI(PATHNAME::BUFFER)] ,Path[ETOUI(PATHNAME::BUFFER)] );
	 CGameInstance::Get().MultiByteWstringToChar(m_strPathName[ETOUI(PATHNAME::TEXTURE)],Path[ETOUI(PATHNAME::TEXTURE)]);
	 j["ShaderPath"]  = Path[ETOUI(PATHNAME::SHADER)];
	 j["BufferPath"]  = Path[ETOUI(PATHNAME::BUFFER)];
	 j["TexturePath"] = Path[ETOUI(PATHNAME::TEXTURE)];
	uint32_t	iParticleTypeWorldEvent = static_cast<uint32_t>(m_eParticleType);
	uint32_t	iParticleTypeEmit = static_cast<uint32_t>(m_eParticleEmitTypeOrigin);
	j["WroldEvent"] = iParticleTypeWorldEvent;
	j["ParticleEmit"] = iParticleTypeEmit;

	return j;
}
HRESULT CWorldParticle::Bind_Resource(const _char* pConstantName, shared_ptr<class CShader> pShader)
{
	
	pShader->Bind_RawValue("g_vLightAmbient", &m_vFog, sizeof m_vFog);
	return pShader->Bind_RawValue(pConstantName,&m_fFogDistance, sizeof m_fFogDistance);
}
void CWorldParticle::OnNotify(const EVENT& eEvent)
{
	switch (eEvent.eEvent)
	{
	case WORLD_EVENT::GENERATOR:
		m_eParticleEmitType = m_eParticleEmitTypeOrigin;
		break;

	case WORLD_EVENT::BOSS_SPAWN:
		m_bStart = true;
		break;
	case WORLD_EVENT::BOSS_LIGHT_OFF:
		m_bReset = true;
		break;
	}
}
HRESULT CWorldParticle::Render_Bloom()
{
	_float4x4 matWorld{};
	_float4 fColor = { 0.5,0.5f,0.5f,1.f };

	XMStoreFloat4x4(&matWorld, m_pTransform->Get_World());

	m_pTransform->Bind_Matrix(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_RawValue("g_vCamePosition", CGameInstance::Get().Get_CamPosition(), sizeof _vector);
	_float4 fText = *CGameInstance::Get().ColorTester();
	m_pShaderCom->Bind_RawValue("g_Color", &fText, sizeof _float4);
	if (m_iTextureID != -1)
		m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureID));
	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();

	return S_OK;
}
HRESULT CWorldParticle::Ready_Component()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strPathName[ETOUI(PATHNAME::SHADER)], TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"), TEXT("Com_BoxShader"), m_pBoxShader)))
		return E_FAIL;

	if(NULL_TRUE(m_pBoxMesh = static_pointer_cast<CCube>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Cube", nullptr))))
		return E_FAIL;

	
	if (NULL_TRUE(m_pVIBufferCom = static_pointer_cast<CVIBuffer_Particle_Point>(CGameInstance::Get().Clone_Prototype(m_iLevelIndex, m_strPathName[ETOUI(PATHNAME::BUFFER)], nullptr))))
			return E_FAIL;
	

	string TexturePath{};
	CGameInstance::Get().MultiByteWstringToChar(m_strPathName[ETOUI(PATHNAME::TEXTURE)], TexturePath);
		
	CGameInstance::Get().Add_Decal_Texture(TexturePath);
	m_iTextureID = CGameInstance::Get().Find_TextueId(TexturePath);
	return S_OK;
}

void CWorldParticle::Fog_Controller(const _float& fTimeDelta)
{
	if (m_bReset)
	{
		m_fTick += fTimeDelta;
		_float t = min(m_fTick / 15.f, 1.f);
		LIGHT_DESC Desc = *CGameInstance::Get().Find_LightMtrl(LIGHT::DIRECTIONAL);
		
		_float4 vAmbient = {};
		XMStoreFloat4(&Desc.vAmbient,XMVectorLerp(XMLoadFloat4(&m_fLightAmbient), XMVectorSet(0.18, 0.18, 0.18, 0.f), t));
		
		CGameInstance::Get().Set_LightDesc(Desc);
		XMStoreFloat4(&m_vFog, XMVectorLerp(XMVectorSet(0.45f, 0.01f, 0.01f, 0.f), XMVectorSet(0, 0, 0, 0) , t));
		m_fFogDistance = m_fCurFogDistance + (2000 - m_fCurFogDistance) * t;
		
		if (t >= 1.f)
		{
			m_fTick = 0.f;
			m_bReset = false;
		}
	}

	if (!m_bStart)
		return;
	m_fTick += fTimeDelta;

	_float t = min(m_fTick / 4.f,1.f);
	
	
	XMStoreFloat4(&m_vFog, XMVectorLerp(XMVectorSet(0, 0, 0, 0), XMVectorSet(0.45f, 0.01f, 0.01f, 0.f),t));
	m_fFogDistance = 2000 - (350 + 2000) * t;
	
	if (t >= 1.f)
	{
		CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_LIGHT_OFF, SHARED_THIS(CWorldParticle));
		m_fTick = 0.f;
		m_bStart = false;
		m_fCurFogDistance = m_fFogDistance = 350;
		LIGHT_DESC Desc = *CGameInstance::Get().Find_LightMtrl(LIGHT::DIRECTIONAL);
		m_fLightAmbient = Desc.vAmbient;
	}

	if (m_fFogDistance <= 350.f)
		m_fFogDistance = 350.f;
}

void CWorldParticle::Load_Data(void* pDesc, const json& j)
{
	auto desc = static_cast<WORLDPARTICLE_DESC*>(pDesc);


	desc->matWorld._11 = j["Right"][0]; desc->matWorld._12 = j["Right"][1]; desc->matWorld._13 = j["Right"][2]; desc->matWorld._14 = 0;
	desc->matWorld._21 = j["Up"][0]; desc->matWorld._22 = j["Up"][1];    desc->matWorld._23 = j["Up"][2];    desc->matWorld._24 = 0;
	desc->matWorld._31 = j["Look"][0]; desc->matWorld._32 = j["Look"][1];  desc->matWorld._33 = j["Look"][2];  desc->matWorld._34 = 0;
	desc->matWorld._41 = j["fPos"][0]; desc->matWorld._42 = j["fPos"][1];  desc->matWorld._43 = j["fPos"][2];  desc->matWorld._44 = 1;
	desc->bWorldCheck = false;
	string Path[3] = {};
	Path[0] = j["ShaderPath"];
	Path[1] = j["BufferPath"];
	Path[2] = j["TexturePath"];
	_wstring DestPath[3] = {};
	
	CGameInstance::Get().MultiByteCharToWstring(Path[ETOUI(PATHNAME::SHADER)], DestPath[ETOUI(PATHNAME::SHADER)]);
	CGameInstance::Get().MultiByteCharToWstring(Path[ETOUI(PATHNAME::BUFFER)], DestPath[ETOUI(PATHNAME::BUFFER)]);
	CGameInstance::Get().MultiByteCharToWstring(Path[ETOUI(PATHNAME::TEXTURE)], DestPath[ETOUI(PATHNAME::TEXTURE)]);
	
	wcscpy_s(&desc->PathName[ETOUI(PATHNAME::SHADER)][0], 256 , DestPath[ETOUI(PATHNAME::SHADER)].c_str());
	wcscpy_s(&desc->PathName[ETOUI(PATHNAME::BUFFER)][0], 256, DestPath[ETOUI(PATHNAME::BUFFER)].c_str());
	wcscpy_s(&desc->PathName[ETOUI(PATHNAME::TEXTURE)][0], 256, DestPath[ETOUI(PATHNAME::TEXTURE)].c_str());

	uint32_t	iParticleTypeWorldEvent = j["WroldEvent"];
	uint32_t	iParticleTypeEmit =  j["ParticleEmit"];
	desc->eParticleType = static_cast<WORLD_EVENT>(iParticleTypeWorldEvent);
	desc->eParticleEmit = static_cast<PARTICLE>(iParticleTypeEmit);

	
}
unique_ptr<CWorldParticle> CWorldParticle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CWorldParticle>(new CWorldParticle(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed TriggerObject");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CWorldParticle::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CWorldParticle(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed TriggerObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
