#pragma once
#include "Component.h"


NS_BEGIN(Engine)
class CTexture_Manager final
{
private:
	CTexture_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CTexture_Manager();

public:
	HRESULT					Initialize();
	HRESULT					Update();
	void					Render();

public:

	HRESULT					Add_Decal_Texture(const string filePath);
	HRESULT					Add_Direct_Decal_Texture(const string filePath, ComPtr<ID3D11ShaderResourceView> pTexture);
	HRESULT					Texture_File_Loader();
	ID3D11ShaderResourceView* Find_Decal_Texture(const uint32_t& iIndex);


	const int32_t				Find_TextueId(const string& filePath);
	const vector<string>& Get_TextureFileNameList() { return m_NameList; }

public:

	static unique_ptr<CTexture_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

private:

	map<string, uint32_t>					 m_TextueMapID = {};
	vector<ComPtr<ID3D11ShaderResourceView>> m_TextureLists = {};
	vector<string>							 m_NameList = {};
	ComPtr<ID3D11Device>		m_pDevice ;
	ComPtr<ID3D11DeviceContext> m_pContext;

	int32_t						m_iSkipBind = { -1 };
};
NS_END

