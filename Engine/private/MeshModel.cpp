#include "MeshModel.h"
#include "GameInstance.h"
CMeshModel::CMeshModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CComponent(pDevice, pContext)
{

}
CMeshModel::CMeshModel(const CMeshModel& Prototype) :CComponent(Prototype)
{

}
CMeshModel::~CMeshModel()
{

}
void CMeshModel::Set_Shader(shared_ptr<class CShader> pShader)
{
	m_pShader = pShader;
}
HRESULT CMeshModel::Initialize(void* pArg)
{
	return S_OK;
}
HRESULT CMeshModel::Initialize_Prototype()
{
	return S_OK;
}


void CMeshModel::Add_Mesh(shared_ptr<CVIBuffer> pMesh)
{
	m_pMeshList.push_back(pMesh);
}

void CMeshModel::Add_MeshName(string Name)
{
	//m_MeshNameList.push_back(Name);
}

HRESULT CMeshModel::Update(_float fTimeDelta)
{

	return S_OK;
}
HRESULT CMeshModel::Render()
{
	
	return S_OK;
}

unique_ptr<CMeshModel> CMeshModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>pContext)
{
	auto pInstance = unique_ptr<CMeshModel>(new CMeshModel(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed 3DModel");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CMeshModel::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CMeshModel>(new CMeshModel(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("3DMdoel Create Failed");
		return nullptr;
	}

	return pInstance;
}
