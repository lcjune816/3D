#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CNonModel;
class CCube;
class CShader;
class CTrigger;
class CDebugLine;
NS_END

NS_BEGIN(Client)
class CTriggerObject final : public CGameObject
{
public:
	typedef struct tagTriggerObject : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring strTriggerName;

	}TRIGGER_DESC;
private:
	typedef struct TriggerInfo
	{
		string			strTriggerName;
		uint32_t		iTargetObjectID = { 0 };
		uint32_t		iObjectID = {};
		int32_t			iModeNumber = { 0 };
		_bool			bOtherTrigger = { false };
		TRIGGER_ROT		eRot;
		_float			fMaxFrameTime{};
		_float			fFrameTickTime{};
		_float			fRotationArrow{};
		_float			fArrrowRotation{};
	}TRIGGER_INFO;
private:
	CTriggerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTriggerObject(const CTriggerObject& Prototye);
public:
	virtual ~CTriggerObject();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Load_Data(void* pDesc, const json& j);
	virtual json	Save_Data();
	HRESULT			Ready_Component();


	
	const TRIGGER_INFO	Get_TriggerInfo() { return m_TriggerInfo; }
	CTrigger*			Get_TriggerPtr() { return m_pTrigger.get(); }
	void				Set_TargetIDNumber(uint32_t iTargetNumber) { m_TriggerInfo.iTargetObjectID = iTargetNumber; m_pTrigger->Set_TargetNumber(m_TriggerInfo.iTargetObjectID); }
	void				Set_Trigger(); 
	
private: 
	HRESULT				Create_Component(void* pArg);

private:
	shared_ptr<Engine::CCube>			m_pBoxMesh = { nullptr };
	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CTrigger>		m_pTrigger = { nullptr };
	vector<uint32_t>					m_MeshNameList;

	TRIGGER_INFO						m_TriggerInfo;
	_float4x4							m_TargetMatrix = {};
public:
	static unique_ptr<CTriggerObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END

