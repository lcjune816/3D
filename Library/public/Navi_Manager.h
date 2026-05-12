#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CNavi_Manager final
{
private:
	CNavi_Manager();
public:
	~CNavi_Manager();

public:
	HRESULT Initialize();

	void    Check_Neighbor(NAVI_MESH* pSrc, NAVI_MESH* pDst, int32_t SrcIndex, int32_t DstIndex);
	void	Add_NaviMeshInfo(const _float4x4* WorldMatrix);
	void    Set_MeshInfo(vector<VERTEX_NOANIME> mesh, vector<uint32_t>index) { m_MeshInfo = mesh; m_MeshIndexInfo = index; }
	const vector<uint32_t>& Get_MeshIndexInfo() {
		return m_MeshIndexInfo;
	}
	const vector<VERTEX_NOANIME>& Get_MeshInfo(){
		return m_MeshInfo;
	}
private:
	vector<NAVI_MESH>				m_NaviMeshs;
	vector<uint32_t>				m_MeshIndexInfo;
	vector<VERTEX_NOANIME>			m_MeshInfo;

public:
	static unique_ptr<CNavi_Manager>		Create();

};

NS_END