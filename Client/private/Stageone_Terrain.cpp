#include "Stageone_Terrain.h"
#include "GameInstance.h"
#include "Terrain.h"
CStageone_Terrain::CStageone_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext):
	CGameObject(pDevice, pContext)
{

}
CStageone_Terrain::CStageone_Terrain(const CStageone_Terrain& Prototye) : CGameObject(Prototye)
{
}
CStageone_Terrain::~CStageone_Terrain()
{
};


HRESULT CStageone_Terrain::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CStageone_Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	__super::Initialize(pArg);
	m_pTerrain = CTerrain::Create(m_pDevice,m_pContext);
	if (NULL_TRUE(m_pTerrain))
		return E_FAIL;

	return S_OK;
}
void CStageone_Terrain::Priority_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(CStageone_Terrain));
}
void CStageone_Terrain::Update(_float fTimeDelta)
{

}
void CStageone_Terrain::Late_Update(_float fTimeDelta)
{

}
HRESULT CStageone_Terrain::Render()
{
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	//m_pShaderCom->Bind_SRV("diffTexture", nullptr);
	_float4 fColor = { 0.f,0.f,0.f,1.f };

	//m_pShaderCom->Bind_RawValue("g_Color", &fColor, sizeof(_float4));

	m_pShaderCom->Begin(0);
	
	m_pTerrain->Bind_Resource();
	m_pTerrain->Render();

	return S_OK;
}
unique_ptr<CStageone_Terrain> CStageone_Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CStageone_Terrain>(new CStageone_Terrain(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed BackGround");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CStageone_Terrain::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CStageone_Terrain>(new CStageone_Terrain(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed BackGround_Clone ");
		return pInstance;
	}
	return pInstance;
}
