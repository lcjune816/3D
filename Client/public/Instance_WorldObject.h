#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Mesh;
class CShader;
NS_END

NS_BEGIN(Client)
class CInstance_WorldObject final : public CGameObject
{
public:
	typedef struct tagInstancingWorldObjectdesc : public CGameObject::GAMEOBJECT_DESC
	{
		int32_t		iMeshNumber;
		INSTANCING_DESC InstancingData;
	}INSTANCING_WORLDOBJECT_DESC;
private:
	CInstance_WorldObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CInstance_WorldObject(const CInstance_WorldObject& Prototye);
public:
	virtual ~CInstance_WorldObject();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT			Ready_Component(void* pArg);


private:
	shared_ptr<Engine::CShader>					m_pShaderCom = { nullptr };
	shared_ptr<Engine::CVIBuffer_Instance_Mesh> m_pViBufferCom = { nullptr };

	int32_t									m_iMeshNumber = {-1};
public:
	static unique_ptr<CInstance_WorldObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END

