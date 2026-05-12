#include "Instancing.h"
#include "GameInstance.h"
CInstancing::CInstancing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CComponent{pDevice,pContext}
{

}

CInstancing::CInstancing(const CInstancing& Prototype) : CComponent{ Prototype }
{

}
CInstancing::~CInstancing()
{
}

HRESULT CInstancing::Initialize()
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (FAILED(m_pTransform->Initialize_Prototype()))
		return E_FAIL;
}

HRESULT CInstancing::Draw_Instancing()
{

	for (auto& PathName : m_OnlyOneDrawCallList)
	{
		//auto TextueList = m_InstancingDataToTextures.find(PathName);
		
		//if (TextueList == m_InstancingDataToTextures.end())
		//	continue;

		auto WorldMatrixs = m_InstancingDatas.find(PathName);

		if (WorldMatrixs == m_InstancingDatas.end())
			continue;

		const auto& matWorlds = WorldMatrixs->second.matWorlds;

		m_pShader->Bind_Matrix_Array("g_World", matWorlds.data(), matWorlds.size());
		m_pShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
		m_pShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

		//for (auto MeshList : TextueList->second)
		//{
			CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(PathName);
		
			pMesh->Bind_ResourceSRV(m_pShader.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
			m_pShader->Begin(0);
		
			pMesh->Bind_Resource();

			pMesh->Render_Array(matWorlds.size());
		//}

	}
	
	return S_OK;
}

HRESULT CInstancing::Add_Instancing_Data(vector<uint32_t>& meshindex, INSTANCING_DATA Data)
{
	//인덱스로 인스턴싱 데이터 찾기
	for (size_t i = 0; i < meshindex.size(); ++i)
	{
		auto iter = m_InstancingDatas.find(meshindex[i]);

		if (iter == m_InstancingDatas.end())
		{
			INSTANCING_DESC desc;
			desc.matWorlds.push_back(Data.matWorld);

			m_InstancingDatas.insert({ meshindex[i], desc });


			m_OnlyOneDrawCallList.push_back(meshindex[i]);
		}
		else
		{
			//중복 매쉬가 있으면 행렬 푸쉬백
			m_InstancingDatas[meshindex[i]].matWorlds.push_back(Data.matWorld);
		}
	}
	
	return S_OK;
}

HRESULT CInstancing::Add_Instancing_Shader(shared_ptr<CShader> pShader)
{
	
	m_pShader = pShader;
	if (NULL_TRUE(m_pShader))
		return E_FAIL;

	return S_OK;
}

const INSTANCING_DESC* CInstancing::Find_Instancing_Data(const uint32_t meshindex)
{
	auto iter = m_InstancingDatas.find(meshindex);

	if (iter != m_InstancingDatas.end())
		return &iter->second;

	return nullptr;
}


unique_ptr<CInstancing> CInstancing::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CInstancing>(new CInstancing(pDevice, pContext));
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed Instancing");
		return nullptr;
	}
	return pInstance;
}
 shared_ptr<CPrototype> CInstancing::Clone(void* pArg)
{
	return nullptr;
};