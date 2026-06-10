#include "Texture_Manager.h"

CTexture_Manager::CTexture_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : m_pDevice(pDevice),m_pContext(pContext)
{

}
CTexture_Manager::~CTexture_Manager()
{
}

HRESULT CTexture_Manager::Initialize()
{

	if (FAILED(Texture_File_Loader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture_Manager::Update()
{

	return S_OK;

}
void CTexture_Manager::Render()
{

}

unique_ptr<CTexture_Manager> CTexture_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CTexture_Manager> pInstance = unique_ptr<CTexture_Manager>(new CTexture_Manager(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		return nullptr;
	}

	return pInstance;
}



HRESULT CTexture_Manager::Add_Decal_Texture(const string filePath)
{
	auto iter = m_TextueMapID.find(filePath);
	if (iter == m_TextueMapID.end())
	{
		uint32_t id = m_TextureLists.size();
		ComPtr<ID3D11ShaderResourceView> pTexture;

		size_t iSize = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), ETOUI(filePath.size()), NULL, 0);
		_wstring TriggerName(iSize, 0);
		MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), ETOUI(filePath.size()), TriggerName.data(), iSize);

		if (FAILED(CreateDDSTextureFromFile(m_pDevice.Get(), TriggerName.c_str(), nullptr, &pTexture)))
			return E_FAIL;

		m_TextueMapID.try_emplace(filePath, id);
		m_TextureLists.emplace_back(pTexture);

	}

	return S_OK;
}

HRESULT CTexture_Manager::Add_Direct_Decal_Texture(const string filePath, ComPtr<ID3D11ShaderResourceView> pTexture)
{

	auto iter = m_TextueMapID.find(filePath);
	
	if (iter == m_TextueMapID.end())
	{
		m_TextureLists.emplace_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture_Manager::Texture_File_Loader()
{
	string path = "../../DecalResource";

	//이거 혁명인데요..
	for (auto& iter : filesystem::recursive_directory_iterator(path))
	{
		if (iter.path().extension() == ".dds")
		{
			string strPathName = iter.path().string();
			m_NameList.push_back(strPathName);
		}
	}

	return S_OK;
}

ID3D11ShaderResourceView* CTexture_Manager::Find_Decal_Texture(const uint32_t& iIndex)
{
	if (m_TextureLists.size() <= iIndex)
		return nullptr;

	m_iSkipBind = iIndex;
	return m_TextureLists[iIndex].Get();
}

const int32_t CTexture_Manager::Find_TextueId(const string& filePath)
{
	auto TextueId = m_TextueMapID.find(filePath);

	if (TextueId != m_TextueMapID.end())
		return TextueId->second;

	return -1;
}
