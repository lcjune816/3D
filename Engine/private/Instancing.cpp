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
	return S_OK;
}

HRESULT CInstancing::Draw_Instancing()
{
	
	return S_OK;
}

_bool CInstancing::Create_Instancing_Desc(INSTANCING_DESC& InstanceData)
{
	if (m_InstancingDatas.empty())
		return false;

	for (auto iter  = m_InstancingDatas.begin(); iter != m_InstancingDatas.end();)
	{
		if (iter->second.matWorlds.size() >= 3)
		{
			INSTANCING_DESC pDesc{};
			
			InstanceData.matWorlds = move(iter->second.matWorlds);
			InstanceData.indices   =	   iter->second.indices;
			InstanceData.Vertices  =	   iter->second.Vertices;
			InstanceData.pIB	   =	   iter->second.pIB;
			InstanceData.pVB	   =	   iter->second.pVB;
			
			for (int32_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
				InstanceData.Textures[i] = iter->second.Textures[i];
			
			
			for (size_t i = 0; i < iter->second.ObjectsTemp.size(); ++i)
			{
				if(auto pObj = iter->second.ObjectsTemp[i].lock())
					pObj->Set_Render(false);
			}

			iter = m_InstancingDatas.erase(iter);
			return true;
		}
		else
		{
			iter = m_InstancingDatas.erase(iter);
		}


	}
	return false;
}

HRESULT CInstancing::Add_Instancing_ObjectData(const uint32_t iIndex, _fmatrix World, shared_ptr<CGameObject> pObj)
{

	auto iter = m_InstancingDatas.find(iIndex);
	
	if (iter != m_InstancingDatas.end())
	{
		_float4x4 matWorld = {};
		XMStoreFloat4x4(&matWorld, World);
		m_InstancingDatas[iIndex].matWorlds.push_back(matWorld);
		m_InstancingDatas[iIndex].ObjectsTemp.push_back(pObj);
	}

	return S_OK;
}

HRESULT CInstancing::Add_Instancing_Data(uint32_t iIndex, INSTANCING_DESC InstanceData)
{
	
		auto iter = m_InstancingDatas.find(iIndex);

		if (iter == m_InstancingDatas.end())
		{
			m_InstancingDatas[iIndex] = InstanceData;
		}
	
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