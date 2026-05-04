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


	const uint32_t		Get_RanderCall() { return iRanderCall; }
	_bool  Culling(CGameObject* pObjm, _vector* vec);
private:
	void		Culling_Calcurator(_fvector* vP);
private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };
	list<shared_ptr<CGameObject>>	m_RenderObjects[ETOUI(RENDERGROUP::END)];

	uint32_t						iRanderCall = {};
private:
	HRESULT	Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();
public:
	static unique_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};


NS_END
