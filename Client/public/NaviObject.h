#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CNaviMesh;
class CShader;
NS_END

NS_BEGIN(Client)
class CNaviObject final : public CGameObject
{
private:
	CNaviObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CNaviObject(const CNaviObject& Prototye);
public:
	virtual ~CNaviObject();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Load_Data(void* pDesc, const json& j);
	virtual json	Save_Data();
	HRESULT			Ready_Component();

private:
	HRESULT				Create_Component(void* pArg);

private:
	shared_ptr<Engine::CShader>				m_pShaderCom = { nullptr };
public:
	static unique_ptr<CNaviObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END

