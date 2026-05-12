#include "Navi_Manager.h"

CNavi_Manager::CNavi_Manager()
{
}

CNavi_Manager::~CNavi_Manager()
{
}

HRESULT CNavi_Manager::Initialize()
{
	return S_OK;
}

void CNavi_Manager::Check_Neighbor(NAVI_MESH* pSrc, NAVI_MESH* pDst, int32_t SrcIndex, int32_t DstIndex)
{
	for (uint32_t i = 0; i < 3; ++i)
	{
		for (uint32_t j = 0; j < 3; ++j)
		{
			_vector S0 = XMLoadFloat4(&pSrc->Triangle[i]);
			_vector S1 = XMLoadFloat4(&pSrc->Triangle[(i+1)%3]);

			_vector D0 = XMLoadFloat4(&pDst->Triangle[j]);
			_vector D1 = XMLoadFloat4(&pDst->Triangle[(j + 1) % 3]);

			if (XMVector3NearEqual(S0, D0, XMVectorSet(0.001f, 0.001f, 0.001f, 0.f)) ||
				XMVector3NearEqual(S0, D1, XMVectorSet(0.001f, 0.001f, 0.001f, 0.f)) ||
				XMVector3NearEqual(S1, D0, XMVectorSet(0.001f, 0.001f, 0.001f, 0.f)) ||
				XMVector3NearEqual(S1, D1, XMVectorSet(0.001f, 0.001f, 0.001f, 0.f)))
			{
				pSrc->neighborIndices[i] = DstIndex;
				pDst->neighborIndices[j] = SrcIndex;
			}
		}
	}

}

void CNavi_Manager::Add_NaviMeshInfo(const _float4x4* WorldMatrix)
{
	size_t iIndexOffset = { 0 };
	
	for (size_t j = 0 ; j< m_MeshIndexInfo.size(); j+=3)
	{

		NAVI_MESH tagNavi{};
		
		XMStoreFloat4(&tagNavi.Triangle[0] ,XMVectorSetW(XMLoadFloat3(&m_MeshInfo[m_MeshIndexInfo[j]].fPos),1.f));
		XMStoreFloat4(&tagNavi.Triangle[1], XMVectorSetW(XMLoadFloat3(&m_MeshInfo[m_MeshIndexInfo[j+1]].fPos), 1.f));
		XMStoreFloat4(&tagNavi.Triangle[2], XMVectorSetW(XMLoadFloat3(&m_MeshInfo[m_MeshIndexInfo[j+2]].fPos), 1.f));

		for (uint32_t k = 0; k < 3; ++k)
			XMStoreFloat4(&tagNavi.Triangle[k], XMVector3TransformCoord(XMLoadFloat4(&tagNavi.Triangle[k]), XMLoadFloat4x4(WorldMatrix)));

		_vector Cross = XMVector3Cross((XMLoadFloat4(&tagNavi.Triangle[0]) - XMLoadFloat4(&tagNavi.Triangle[1])),
										(XMLoadFloat4(&tagNavi.Triangle[0]) - XMLoadFloat4(&tagNavi.Triangle[2])));

		if (XMVectorGetX(XMVector3Dot(Cross, XMVectorSet(0.f, 1.f, 0.f, 0.f))) <= 0.5f)
			continue;

		XMStoreFloat3(&tagNavi.Center, (XMLoadFloat4(&tagNavi.Triangle[0]) + XMLoadFloat4(&tagNavi.Triangle[1]) + XMLoadFloat4(&tagNavi.Triangle[2])) * 0.3f);
		m_NaviMeshs.push_back(tagNavi);
	}


	
	for (size_t i = 0; i < m_NaviMeshs.size();++i)
	{
		for (size_t j = i + 1; j < m_NaviMeshs.size(); ++j)
		{
			Check_Neighbor(&m_NaviMeshs[i], &m_NaviMeshs[j], i, j);
		}
	}
	

}

unique_ptr<CNavi_Manager>		CNavi_Manager::Create()
{

	return unique_ptr<CNavi_Manager>(new CNavi_Manager);
}