#pragma once

#include "Trigger.h"
NS_BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CGameObject abstract : public CPrototype
{
public:
typedef struct strGameObjectDesc : public CTransform::TRANSFORM_DESC , public CTrigger::TRIGGER_DESC
{
	 string FileName	   = {};
	 string strTriggerName = {};
	 json j;
	 uint32_t iLevel = {};
	 uint32_t index = {};
	 int32_t  iModeNumber;
	 
	 _bool	   bCopy = false;
	 _bool	   bFrontCamera = true;
	 
	 MESH_TYPE	eType = {};
}GAMEOBJECT_DESC;

protected:
	CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CGameObject(const CGameObject& Prototype);
public:
	virtual ~CGameObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Update(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Late_Update(_float fTimeDelta);
	virtual HRESULT	Render();

	virtual  void			ReBindsComponnet(uint32_t iLevelIndex);
	_bool					Get_Dead()      { return m_bDead; }
	MESH_TYPE				Get_MeshType()  { return m_eMeshType; }
	weak_ptr<CTransform>	Get_Transform() { return m_pTransform; }
	_bool					Get_EndObject() { return m_bEndObject; }
	CTransform*				Get_TransformPtr() { return m_pTransform.get(); }
	const string			Get_PathName()  { return m_PathName; }

	virtual void			Mesh_Change(vector<uint32_t> MeshList) {}
	_bool					Get_Finished() { return m_bFinished; }
	void					Set_Finisihed(_bool bFinishied) { m_bFinished = bFinishied; }
	void					Set_CheckCopyLater(_bool bLayer) { m_bCopyLayerCheck = bLayer; }
	_bool					Check_Name(const _char* pName);

	void					Set_Dead() { m_bDead = true; }
	void					Set_bBoxColor(_bool bColor) { m_bBoxColor = bColor; }
	void					Set_EndObject(_bool bEnd) { m_bEndObject = bEnd; }
	void					Set_Render(_bool bRender) { m_bRender = bRender; }
	virtual void            Load_Data(void* pDesc, const json& j);
	virtual json			Save_Data();

	shared_ptr<class CComponent> Find_Component(const _wstring& strComponentTag);
protected:
	
	template<typename T>
	HRESULT Add_Component(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,

		const _wstring& strComponentTag, shared_ptr<T>& pOut, void* pArg = nullptr)
	{
		if (nullptr != Find_Component(strComponentTag))
			return E_FAIL;

		auto	pComponent = dynamic_pointer_cast<CComponent>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (nullptr == pComponent)
			return E_FAIL;

		m_Components.emplace(strComponentTag, pComponent);

		pOut = dynamic_pointer_cast<T>(pComponent);

		return S_OK;
	}


protected:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
	shared_ptr<CTransform>				m_pTransform = { nullptr };
	shared_ptr<Engine::CShader>			m_pBoxShader = { nullptr };

	MESH_TYPE							m_eMeshType{ MESH_TYPE::END };

	_bool								m_bFinished = {false};
	_bool								m_bDead = { false };
	_bool								m_bEndObject = { false };
	_bool								m_bBoxColor = { false };
	_bool								m_bCopyLayerCheck = { false };
	_bool								m_bRender = { true };
	_char					m_pTagName[64];
	string						m_PathName;
protected:
	map<const _wstring, shared_ptr<class CComponent>> m_Components;

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

NS_END
