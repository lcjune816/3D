#pragma once
#include "Engine_Defines.h"
enum class CELL_EVENT{};
NS_BEGIN(Engine)
class CCell final
{
private:
	CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CCell();

public:
	HRESULT Initialize(const _float3* pPoints, int32_t iIndex);
	

	json Save_Data();

#ifdef _DEBUG
	HRESULT Render(class CShader* pShader);
	_float3* Get_CellPos() { return &m_NaviInfo.vPoints[0]; }
	void		Set_Choice(_bool bCheck) { m_bChoice = bCheck; }
#endif
private:
	NAVI						m_NaviInfo{};
	_bool						m_bChoice{ false };
private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
#ifdef _DEBUG
private:
	shared_ptr<class CTriAngle>		m_pVIBuffer = { nullptr };

#endif

public:
	static	shared_ptr<CCell>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, int32_t iIndex);
};


NS_END
