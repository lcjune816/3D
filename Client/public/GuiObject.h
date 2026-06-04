#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
namespace Engine
{
	class CNonModel;
	class CCell;
}
NS_BEGIN(Client)
class CGuiObject final : public CGameObject
{
public:
	typedef struct tagGuiObjecttdesc : public CGameObject::GAMEOBJECT_DESC
	{

	}GUIOBJECT_DESC;
private:
	CGuiObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CGuiObject(const CGuiObject& Prototye);
public:
	virtual ~CGuiObject();

	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

	
public:
	HRESULT							Enable_GUI(string& name);
	void							Select_Model();
	void							ImGui_Gizmo();

private:
	void							Light_Setting();
	void							Connect_Model();
	void							Click_Reset();
	void							Add_Decal_Texture();
	void							Multy_Copy();
	void							Move_Layer();
	void							Navi_Creator();
	void							Particle_Installer();
	void							ComboArray(const _char* ItemNameValue[], int32_t iTemArrayCnt, int32_t& itemIndex, const _char* pLabel, const _char* PreviewValue, ImGuiComboFlags eFlags);
	void							Load_Data();
	HRESULT							Load_Value(LEVEL eLevel,const _wstring strFilePath, 
									const _wstring strLayerName, const _wstring strObjName, const string strLoadDataName);
	void							Save();
	weak_ptr<CGameObject>			Picking_Object(const _wstring& LayerName);
	weak_ptr<CGameObject>			Picking_ParticleObject();

	_vector							Picking_Terrain(int32_t iSelect, _float3& frayOrigin, _float3& frayDir,_bool& bCheck);
private:
	weak_ptr<CGameObject>			m_pObj ;
	weak_ptr<CGameObject>			m_pConnetObjectFirst  ;
	weak_ptr<CGameObject>			m_pConnetObjectSecond ;
	weak_ptr<class CCell>			m_pCell;
	_bool							m_bModelCancle = { false };
	
	_bool							m_bMouseCheck = { false };

	_float2							m_fMousePickXY = {};

	_float3							m_fMosueLocalPos = {};
	_float3							m_fMouseLocalDir = {};

	_wstring						m_LayerName = {};
	string							m_CopyTriggerName = {};

	_float4x4						m_CopyWorld = {};
	_bool							m_bCopy = {false};

	int32_t							m_iModelSelect{0};
	int32_t							m_iSelectParticle{ 0 };
	LEVEL							m_eLevel = {};

	list<ComPtr<ID3D11ShaderResourceView>> m_GuiResources;
	const char*				m_WorldEventItem[ETOUI(WORLD_EVENT::END)];
	string				m_strGameObject,m_strTrigger,m_strDecal;
	_wstring				m_strGameObjectPath, m_strTriggerPath, m_strDecalpath;
public:
	static unique_ptr<CGuiObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

