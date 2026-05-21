#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
class CCell final
{
private:
	CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CCell();

public:
	HRESULT Initialize(NAVI Navi, CELL_EVENT eEvent, int32_t iIndex, _float3* pPoints = nullptr);
	

	
	void		Set_Neighbor(LINE eLine, shared_ptr<CCell> pCell)
	{
		m_NaviInfo.iNeighborIndices[ETOUI(eLine)] = pCell->m_NaviInfo.iIndex;
	}
	void		Set_Neighbors(int32_t* pNeighborIndices)
	{
		memcpy(m_NaviInfo.iNeighborIndices, pNeighborIndices, sizeof(int32_t) * 3);
	}
	_vector Get_CellPos(int32_t iPointIndex) { 
		return XMLoadFloat3(&m_NaviInfo.vPoints[iPointIndex]); }

private:
	HRESULT Ready_New(int32_t iIndex, _float3* pPoints);
	HRESULT Ready_Load(NAVI Navi, CELL_EVENT eEvent,int32_t iIndex);
public:
	_bool			CheckAstar(ENGINE_ASTAR& parentsNode,  list<ENGINE_ASTAR>& OpenList, list<ENGINE_ASTAR>& CloseList, vector<shared_ptr<CCell>>& CellList,_fvector vFinalPos, int32_t* iSrcIndex);

	_bool			IsIn(_fvector vResultPos, int32_t* pNeighborIndex);
	_bool			Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_float			Compute_Height(_fvector vPos);
	void			Set_Choice(_bool bCheck) { m_bChoice = bCheck; }
	json			Save_Data();
	CELL_EVENT		Get_Event() { return m_eEvent; }
	NAVI&			Get_NaviInfo() { return m_NaviInfo;}
	void			Set_CellEvent(CELL_EVENT eEvent) {
		m_eEvent = eEvent;}
	
#ifdef _DEBUG
	HRESULT Render(class CShader* pShader);

#endif
private:
	NAVI								m_NaviInfo{};
	CELL_EVENT							m_eEvent{CELL_EVENT::END};
	_bool								m_bChoice{ false };
private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
#ifdef _DEBUG
private:
	shared_ptr<class CTriAngle>		m_pVIBuffer = { nullptr };

	int32_t								m_iCheckSameCell{ -1 };
#endif

public:
	static	shared_ptr<CCell>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, NAVI Navi, CELL_EVENT eEvent, int32_t iIndex , _float3* pPoints = nullptr);
};


NS_END
