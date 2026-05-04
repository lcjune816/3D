#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
namespace Engine
{
	class CTerrain;
}

NS_BEGIN(Client)

class CStageone_Terrain final : public CGameObject
{
private:
	CStageone_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CStageone_Terrain(const CStageone_Terrain& Prototye);
public:
	virtual ~CStageone_Terrain();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();



private:
	unique_ptr<Engine::CTerrain>		m_pTerrain = { nullptr };
	vector<VERTEX>						m_VertexList = {};
	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };

public:
	static unique_ptr<CStageone_Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END
