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
	
	m_eParticleType		= desc->eParticleType;
	m_eParticleEmitType = desc->eParticleEmit;
	m_iLevelIndex		= desc->iLevelIndex;
	m_strPathName[ETOUI(PATHNAME::SHADER)] = desc->PathName[ETOUI(PATHNAME::SHADER)];
	m_strPathName[ETOUI(PATHNAME::BUFFER)] = desc->PathName[ETOUI(PATHNAME::BUFFER)];
	m_strPathName[ETOUI(PATHNAME::TEXTURE)] = desc->PathName[ETOUI(PATHNAME::TEXTURE)];

	if (FAILED(Ready_Component()))
		return E_FAIL;

	__super::Initialize(desc);
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
		m_pVIBufferCom->Fog_Spread(fTimeDelta);
		break;
	case PARTICLE::SPARK:
		m_pVIBufferCom->Spark(fTimeDelta);
		break;
	}
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(CWorldParticle));

}
void CWorldParticle::Late_Update(_float fTimeDelta)
{

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
	m_pShaderCom->Bind_RawValue("g_Color", CGameInstance::Get().ColorTester(), sizeof _float4);
	m_pShaderCom->Bind_SRV("g_Texture", CGameInstance::Get().Find_Decal_Texture(m_iTextureID));
	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();
	
	
	if (m_bBoxColor)
		fColor = { 1.f,0.f,0.f,1.f };
	m_pBoxShader->Bind_Matrix("g_World", &matWorld);
	m_pBoxShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pBoxShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));
	m_pBoxShader->Begin(0);
	
	m_pBoxMesh->Bind_Resource();
	m_pBoxMesh->Render();


	fColor = { 0.f,0.f,0.f,0.f };
	m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof(fColor));

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
	j["Name"] = m_PathName;

	return j;
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
	

	char TexturePath[256] = {};
	_bool		bMove = false;
	WideCharToMultiByte(CP_ACP, 0, m_strPathName[ETOUI(PATHNAME::TEXTURE)].data(), -1, TexturePath, sizeof(char) * 256, NULL, NULL);

	CGameInstance::Get().Add_Decal_Texture(TexturePath);
	m_iTextureID = CGameInstance::Get().Find_TextueId(TexturePath);
	return S_OK;
}

void CWorldParticle::Load_Data(void* pDesc, const json& j)
{
	auto desc = static_cast<GAMEOBJECT_DESC*>(pDesc);

	desc->matWorld._11 = j["Right"][0]; desc->matWorld._12 = j["Right"][1]; desc->matWorld._13 = j["Right"][2]; desc->matWorld._14 = 0;
	desc->matWorld._21 = j["Up"][0]; desc->matWorld._22 = j["Up"][1];    desc->matWorld._23 = j["Up"][2];    desc->matWorld._24 = 0;
	desc->matWorld._31 = j["Look"][0]; desc->matWorld._32 = j["Look"][1];  desc->matWorld._33 = j["Look"][2];  desc->matWorld._34 = 0;
	desc->matWorld._41 = j["fPos"][0]; desc->matWorld._42 = j["fPos"][1];  desc->matWorld._43 = j["fPos"][2];  desc->matWorld._44 = 1;
	m_PathName = j["Name"];
	int32_t iModelNumber = j["ModeNumber"];
	desc->FileName = m_PathName;
	desc->iModeNumber = iModelNumber;

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
