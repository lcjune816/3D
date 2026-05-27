#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CNavi_Manager final
{
private:
	CNavi_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CNavi_Manager();

public:
	HRESULT								Initialize();
public:
	_bool								Check_NeraPos(_float3* fPos);
	_bool								Check_First();
	void								Add_NaviMeshInfo(_float3* fPos, CELL_EVENT eEvent);
	HRESULT								Save_Navi(const _wstring& FilePath, const _char* pName);
	HRESULT								Load_Navi(const _wstring& FilePath, const _char* pName);
	void								Undo_Cell();
	void								Render();
	void								Connect_Navigaion(shared_ptr<class CNavigation> pNavigation);
	void								Ready_Neightbors();
	shared_ptr<class CCell>				Select_TriAngle(_fvector vOrigin, _fvector vDir);
#ifdef _DEBUG
private:
	shared_ptr<class CShader>				m_pShader = { nullptr };
#endif
private:
	weak_ptr<class CNavigation>				m_pNavigations;
	ComPtr<ID3D11Device>					m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>				m_pContext = { nullptr };
public:
	static unique_ptr<CNavi_Manager>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END