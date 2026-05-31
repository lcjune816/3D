#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
NS_BEGIN(Engine)
class CNonModel;
class CRect;
class CShader;
class CCube;
NS_END

NS_BEGIN(Client)
class CDecalObject final : public CGameObject
{
public:
	typedef struct tagDecalObject : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring strTriggerName;

	}TECAL_OBJECT;

private:
	CDecalObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CDecalObject(const CDecalObject& Prototye);
public:
	virtual ~CDecalObject();

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
	HRESULT				Create_Component();

private:
	shared_ptr<Engine::CRect>			m_pRect = { nullptr };
	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CCube>			m_pBoxMesh = { nullptr };

	uint32_t							m_iTextureID = {};
public:
	static unique_ptr<CDecalObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END

