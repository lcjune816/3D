#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLight final
{
private:
	CLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLight();

public:
	HRESULT	Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Initialize_Load(json& j);


	HRESULT Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer);

	HRESULT Debug_Render(shared_ptr<class CShader> pShader, shared_ptr<class CCube> pViBuffer);
	_bool	Dead_Check() { if (m_bDead)return true;    return false; }
	LIGHT_DESC*				Get_LightDesc(const LIGHT& eLightType) {
		if (eLightType == m_LightDesc.eType)
			return &m_LightDesc;

		return nullptr;
	}
	LIGHT_DESC* Get_LightDescNoCheckType() { return &m_LightDesc; }

	LIGHT_DESC* Get_LightDescOrigin() { return &m_LightOrigin; }
	void					Set_LightDesc(LIGHT_DESC Desc);
	void						Set_Dead(_bool bDead) { m_bDead = bDead; }
public:
	json						Save_Data();

	_bool						ComParePos(_fvector vOriginPos, _fvector vOriginDir);
private:
	ComPtr<ID3D11Device>		m_pDevice = { };
	ComPtr<ID3D11DeviceContext>	m_pContext = {};
	
	uint32_t						m_iIndex = {1};

	_float4						m_fColor = { 1,0,0,1 };
	_bool						m_bDead = {false};
	LIGHT_DESC	m_LightDesc{};
	LIGHT_DESC  m_LightOrigin{};

public:
	static		shared_ptr<CLight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc);
	static		shared_ptr<CLight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, json& LoadJson);
};

NS_END

