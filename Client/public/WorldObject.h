#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CNonModel;
class CCube;
class CShader;
NS_END

NS_BEGIN(Client)
class CWorldObject final : public CGameObject
{
public:
	typedef struct tagWorldObjectdesc : public CGameObject::GAMEOBJECT_DESC
	{
		
	}WORLDOBJECT_DESC;
private:
	CWorldObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWorldObject(const CWorldObject& Prototye);
public:
	virtual ~CWorldObject();

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
	HRESULT			Create_Component();

private:
	shared_ptr<Engine::CCube>			m_pBoxMesh = { nullptr };
	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };

	vector<uint32_t>					m_MeshNameList;
	int32_t								m_iModeNumber = {0};

public:
	static unique_ptr<CWorldObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END

