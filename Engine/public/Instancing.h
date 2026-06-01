#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CInstancing final : public CComponent
{
private:
	CInstancing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CInstancing(const CInstancing& Prototpye);
public:
	~CInstancing();


public:
	HRESULT						Initialize();

	HRESULT						Draw_Instancing();


	_bool					Create_Instancing_Desc(INSTANCING_DESC& InstanceData);
	HRESULT						Add_Instancing_ObjectData(const uint32_t iIndex, _fmatrix World, shared_ptr<CGameObject> pObj);
	HRESULT						Add_Instancing_Data(uint32_t iIndex, INSTANCING_DESC InstanceData);
	const INSTANCING_DESC*		Find_Instancing_Data(const uint32_t meshindex);
private:
	map<uint32_t, INSTANCING_DESC>			m_InstancingDatas;

	map<uint32_t, vector<uint32_t>>			m_InstancingDataToTextures;
public:
	static		unique_ptr<CInstancing> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END