#include "Instance_WorldObject.h"
#include "GameInstance.h"

CInstance_WorldObject::CInstance_WorldObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CInstance_WorldObject::CInstance_WorldObject(const CInstance_WorldObject& Prototye) :CGameObject(Prototye)
{
}
CInstance_WorldObject::~CInstance_WorldObject()
{
}

HRESULT CInstance_WorldObject::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CInstance_WorldObject::Initialize(void* pArg)
{
	auto pDesc = static_cast<INSTANCING_WORLDOBJECT_DESC*>(pArg);

	m_iMeshNumber = pDesc->iMeshNumber;

	CVIBuffer_Instance_Mesh::MESHINSTANCE_DESC MeshInstance{};
	MeshInstance.insatnceData = move(pDesc->InstancingData);

	if (FAILED(Ready_Component(&MeshInstance)))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}
void CInstance_WorldObject::Priority_Update(_float fTimeDelta)
{
}
void CInstance_WorldObject::Update(_float fTimeDelta)
{
}
void CInstance_WorldObject::Late_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(CInstance_WorldObject));
}
HRESULT CInstance_WorldObject::Render()
{
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pViBufferCom->Bind_ResourceSRV(m_pShaderCom.get(), "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
	m_pViBufferCom->Bind_ResourceSRV(m_pShaderCom.get(), "g_NormalTexture", aiTextureType_NORMALS, 0);
	m_pShaderCom->Begin(0);
	m_pViBufferCom->Bind_Resource();
	m_pViBufferCom->Render();
	

	return S_OK;
}


HRESULT CInstance_WorldObject::Ready_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Instancing_NonAnime"),
		TEXT("Component_Shader"), m_pShaderCom)))
		return E_FAIL;

	
	m_pViBufferCom = static_pointer_cast<CVIBuffer_Instance_Mesh>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_Instance_Mesh", pArg));
	return S_OK;
}

unique_ptr<CInstance_WorldObject> CInstance_WorldObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CInstance_WorldObject>(new CInstance_WorldObject(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed WorldObject");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CInstance_WorldObject::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CInstance_WorldObject(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed WorldObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
