#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CNonModel;
}
NS_BEGIN(Client)
class CBackGround final :  public CGameObject
{
private:
	CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBackGround(const CBackGround& Prototye);
public:
	virtual ~CBackGround();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	unique_ptr<class CGuiObject>		m_pGuiObj = { nullptr };
	shared_ptr<Engine::CNonModel>		m_pMeshModel = { nullptr };

public:
	static unique_ptr<CBackGround> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END

