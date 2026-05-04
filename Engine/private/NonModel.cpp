#include "NonModel.h"
#include "GameInstance.h"


CNonModel::CNonModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CMeshModel(pDevice, pContext)
{

}
CNonModel::CNonModel(const CNonModel& Prototype) :CMeshModel(Prototype)
{

}
CNonModel::~CNonModel()
{

}
HRESULT CNonModel::Initialize(void* pArg)
{
	return S_OK;
}
HRESULT CNonModel::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CNonModel::Update(_float fTimeDelta)
{
	
	return S_OK;
}
HRESULT CNonModel::Render()
{
	for (auto& iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		auto Shader = m_pShader.lock();
		if (NULL_TRUE(Shader))
			return E_FAIL;

		//pMesh->Bind_ResourceSRV(Shader.get());
		pMesh->Bind_Resource();
		pMesh->Render();
		
	}

	return S_OK;
}


unique_ptr<CNonModel> CNonModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>pContext)
{
	auto pInstance = unique_ptr<CNonModel>(new CNonModel(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CNonModel");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CNonModel::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CNonModel>(new CNonModel(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CNonModel_Clone");
		return nullptr;
	}

	return pInstance;
}
