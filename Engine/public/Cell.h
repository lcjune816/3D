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
	HRESULT Initialize(const _float3* pPoints, int32_t iIndex);
	



#ifdef _DEBUG
	HRESULT Render();
	_float3* Get_CellPos() { return &m_NaviInfo.vPoints[0]; }

#endif
private:
	NAVI						m_NaviInfo{};
private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
#ifdef _DEBUG
private:
	shared_ptr<class CTriAngle>		m_pVIBuffer = { nullptr };

#endif

public:
	static	shared_ptr<CCell>		Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, int32_t iIndex);
};


NS_END
