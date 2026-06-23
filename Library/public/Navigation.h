#pragma once
#include "Component.h"

NS_BEGIN(Engine)

enum class OWNER{PLAYER,BOSS,END};
class ENGINE_DLL CNavigation final : public CComponent
{
//	XMFLOAT3& operator=(const XMFLOAT3&) = default;
	struct PunnelArrow
	{
		PunnelArrow(_float3* pArrow) { Arrow[0] = pArrow[0]; Arrow[1] = pArrow[1]; }
		_float3 Arrow[2];
};
public:
	typedef struct strnavigationdesc
	{
		OWNER eOwner;
		CELL_EVENT eEvent;
		SPAWN	   eSpawn;
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

	_bool  InMove(_fvector vResultPos, _float3* fDir = nullptr);
	_vector SetUp_OnNavigation(_fvector vPos, _float offsetY);
	_bool	AStartAlgorithm(const uint32_t endLayerIndex, const _wstring& LayerName, const _char* tagName, _fvector SrcPos, _float3* fPos);
	void	PunnelAlgorithm(_fvector vSrcPos);
	_vector	MoveToAstar(_fvector vPos, const _float& fSpeed, const _float& fTimeDelta, _float3* vLook);
	void	Event_Check(CELL_EVENT eCellEvent);
	void	Reset_Astar();
	void    Set_CurrentIndex(int32_t index) { m_iCurretnCellindex = index; }
	_vector Get_CellEventPos(CELL_EVENT eType);

	void	ReSearchCell(_fvector vPos);
	void	InMove_Control(_bool   bMove) { m_bIsIn = bMove; }

	_bool	Is_CurrentCell(int32_t iIndex) { if (m_iCurretnCellindex == iIndex) return true;   return false; }
public:
//GUI¿ë
	_bool	Check_NeraPos(_float3* fPos); 
	_bool	Check_First() { if (m_Cells.empty())return true;		return false; }
	void	Add_NaviMeshInfo(_float3* fPos, CELL_EVENT eEvent);
	void	Undo_Cell();
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
	int32_t								m_iPreCellIndex;
	int32_t								m_iCurretnCellindex = { -1 };
	list<ENGINE_ASTAR>					m_AstarOpenList;
	list<ENGINE_ASTAR>					m_AstarCloseList;
	list<ENGINE_ASTAR>					m_MoveToList;

	list<PunnelArrow>					m_PunnelList;
	_bool								m_bIsIn{true};
	OWNER								m_eOwner;
	CELL_EVENT							m_eEvent{};
	vector<shared_ptr<class CCell>>		m_Cells;
	
public:
	static unique_ptr<CNavigation>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _wstring& FilePath, const _char* pName);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END