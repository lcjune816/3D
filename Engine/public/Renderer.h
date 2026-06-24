#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class CRenderer final
{

private:
	CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CRenderer();

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pRenderObject);
	HRESULT Draw();


	HRESULT Add_RenderToParticle(shared_ptr<class CParticleObject> pObj);
	const uint32_t		Get_RanderCall() { return iRanderCall; }

	
private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };
	list<shared_ptr<CGameObject>>	m_RenderObjects[ETOUI(RENDERGROUP::END)];

	uint32_t						iRanderCall = {};
	int32_t							iDebuhRender{};

	_float							m_iSampleing[2]{ 2.f,4.f };
private:
	shared_ptr<class CRect>			 m_pVIBuffer = { nullptr };
	shared_ptr<class CShader>		 m_pShader = { nullptr };
	weak_ptr<class CParticleObject>m_pParticleFog = {};
	_float4x4						 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_float2							m_fViewPortSize{};

	void							SetUp_ViewPort(uint32_t iWidth, uint32_t iHeight);
private:
	HRESULT	Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_LightsBloom();

	HRESULT	Render_Fog();
	HRESULT Render_Combined();
	HRESULT Render_NonLights();
	HRESULT Render_Blend();

	HRESULT Render_BloomBefore(); 
	HRESULT Render_Emissive();
	
	HRESULT Render_BlurHorizontal();
	HRESULT Render_BlurVertical();
	
	HRESULT Render_BloomCombine();
	HRESULT Render_DownSample();
	HRESULT Render_DownSampleBlur();
	HRESULT Render_UpSampleBlur();

	//HRESULT Render_UpSample();

	HRESULT Render_UI();

#ifdef _DEBUG
private:
	HRESULT Render_DEBUG();
#endif

public:
	static unique_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};


NS_END
