#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct strnavigationdesc
	{
		CELL_EVENT eEvent;
		int32_t iIndex;
	}NAVIGATION_DESC;
private:
	CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CNavigation();

public:
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Initialize_Prototype(const _wstring& FilePath, const _char* pName);
	HRESULT Ready_Neightbors();
	HRESULT Render();

	void   Make_NaviToTerrain();
	_bool  InMove(_fvector vResultPos);
	_vector SetUp_OnNavigation(_fvector vPos, _float offsetY);
public:
	_bool	AStartAlgorithm(const uint32_t endLayerIndex,  const _wstring& LayerName, const _char* tagName,_fvector SrcPos);
	_vector	MoveToAstar(_fvector vPos, const _float& fSpeed, const _float& fTimeDelta, _float3* vLook);


	_bool	Check_NeraPos(_float3* fPos);
	_bool	Check_First() { if (m_Cells.empty())return true;		return false; }
	void	Add_NaviMeshInfo(_float3* fPos, CELL_EVENT eEvent);
	void	Undo_Cell();
	void	Reset_Astar();
	void    Dead_Check();
	_vector Find_CellPos(int32_t index);
private:
	const _vector Get_CurrentCell_Info(int32_t* iDestIndex);
	shared_ptr<class CGameObject> Find_Object(const uint32_t endLayerIndex, const _wstring& LayerName, const _char* tagName);

public:
	HRESULT								Save_Navi(const _wstring& FilePath, const _char* pName);
	HRESULT								Load_Navi(const _wstring& FilePath, const _char* pName);
	shared_ptr<class CCell>				Select_TriAngle(_fvector vOrigin, _fvector vDir);
#ifdef _DEBUG
private:
	shared_ptr<class CShader>				m_pShader = { nullptr };
#endif
private:

	int32_t								m_iDestIndex = { -1 };
	int32_t								m_iCurretnCellindex = { -1 };
	list<ENGINE_ASTAR>				m_AstarOpenList;
	list<ENGINE_ASTAR>				m_AstarCloseList;

	list<ENGINE_ASTAR>				m_MoveToList;
	vector<shared_ptr<class CCell>>		m_Cells;
	
public:
	static unique_ptr<CNavigation>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _wstring& FilePath, const _char* pName);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END