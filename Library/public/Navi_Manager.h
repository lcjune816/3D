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
	HRESULT Initialize();

	_bool	Check_NeraPos(_float3* fPos);
	_bool	Check_First() { if (m_Cells.empty())return true;		return false; }
	void	Add_NaviMeshInfo(_float3* fPos);
	void    Save_Navi(json& j);
	HRESULT Render();
#ifdef _DEBUG
private:
	shared_ptr<class CShader>				m_pShader = { nullptr };
#endif
private:
	vector<shared_ptr<class CCell>>		m_Cells;

	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
public:
	static unique_ptr<CNavi_Manager>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END