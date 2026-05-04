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
	HRESULT						Add_Instancing_Data(const string strName, INSTANCING_DATA Data, vector<string> TextureNames);
	HRESULT						Add_Instancing_Shader(shared_ptr<class CShader> pShader);
	const INSTANCING_DESC*		Find_Instancing_Data(const string strName);
private:
	map<string, INSTANCING_DESC>		m_InstancingDatas;
	
	shared_ptr<class CShader>			m_pShader;
	shared_ptr<class CTransform>		m_pTransform;

	vector<string>						m_OnlyOneDrawCallList;

	map<string, vector<string>>			m_InstancingDataToTextures;
public:
	static		unique_ptr<CInstancing> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END