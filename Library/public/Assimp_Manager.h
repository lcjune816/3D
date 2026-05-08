#pragma once
#include "3DModel.h"
#include "NonModel.h"
NS_BEGIN(Engine)

class CMesh;
class CMeshNonAnime;
class  CAssimp_Manager final
{
public:
	typedef struct strAssimpdesc : public C3DModel::MODEL_DESC
	{

	}ASSIMP_DESC;
private:
	CAssimp_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CAssimp_Manager();

	HRESULT										Initialize();
	HRESULT										Update(_float fTimeDelta);
	HRESULT										Render();

	shared_ptr<CMesh>							ImportOnlyMesh(void* pArg);
	HRESULT										ImportModel_Anime(const IMPORTMODEL_DESC& tagModel,  vector<shared_ptr<CVIBuffer>>& pPrototype, shared_ptr<class CAnimator>& pAnimator, weak_ptr<class CTransform> pTransform, _matrix Premat);
	HRESULT										ImportModel_NonAnimeModel(const IMPORTMODEL_DESC& tagModel, weak_ptr<class CTransform>& pTransform, vector<uint32_t>& IdList);
	void						    			ProcessNode(aiNode* pNode, const aiScene* pScene, const string& pFile,  vector<shared_ptr<CVIBuffer>>& pPrototype);
	shared_ptr<CPrototype>						ProcessMesh(aiMesh* pMesh, const aiScene* pScene, const string& pFile);
	
	void						    			NonAniMation_ProcessNode(aiNode* pNode, const aiScene* pScene, const string& pFile, vector<uint32_t>& MeshNames);
	shared_ptr<CPrototype>						NonAniMation_ProcessMesh(aiMesh* pMesh, const aiScene* pScene, const string& pFile);

	void										Add_Mesh(string name, shared_ptr<CMeshNonAnime>& pMesh);

	void										Read_MissingBones(vector<shared_ptr<class CBone>>& pbone, const aiAnimation* animation);
	void										Read_HeirarchyData(AssimpNodeData& dest, const aiNode* src);

	void										Load_Animation(const aiScene* pScene, void* pArg);
	vector<TEXTURE>								LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType Type, string typeName, const aiScene* pScene, MATERIAL& eMaterial);
	ComPtr<ID3D11ShaderResourceView>			LoadEmbeddedTexture(const aiTexture* pTexture);

	void										Set_VertexBoneData(VERTEX& vertex, uint32_t boneId, _float fweight);
	void										Set_VertexBoneDataToDefault(VERTEX& vertex);
	
	const string								Get_Binary_Path() { return m_strbinary; }
	ID3D11ShaderResourceView*					Find_Texture(const uint32_t& id);
	CMeshNonAnime*								Find_Mesh(const uint32_t& id);

private:
	void										NonAnime_Binary_File_Import(vector<uint32_t>& idList, const string& fileName, weak_ptr<class CTransform> pTransform );
	void										NonAnime_Binary_File_Import_Mesh(vector<uint32_t>& idList, ifstream& readFile);

	void										Binary_File_Import(vector<shared_ptr<CVIBuffer>>& pPrototype, const string& fileName, weak_ptr<class CTransform> pTransform, shared_ptr<class CAnimator>& pAnimator);

	void										Binary_File_Import_Mesh(vector<shared_ptr<CVIBuffer>>& pPrototype,  ifstream& readFile);
	void										Binary_File_Import_Bone(ifstream& readFile);
	void										Binary_File_Import_HeirarachyData(ifstream& readFile, AssimpNodeData& dest, uint32_t iArrayCnt);
	void										Binary_File_Import_ReadMissingBone(ifstream& readFile, uint32_t iArrayCnt);
	void										Binary_File_Import_Animation(ifstream& readFile, void* dest);
	
	void										Check_FileName(string& File, string SrcFile,string& findFile);
	_float4x4									mat_Copy(const aiMatrix4x4& Dst);

private:
	ComPtr<ID3D11Device>						m_pDevice		  = { nullptr };
	ComPtr<ID3D11DeviceContext>					m_pContext		  = { nullptr };

	vector<vector<shared_ptr<class CBone>>>		m_CopyBones;

	vector<TEXTURE>								m_pTexture_Loaded[AI_TEXTURE_TYPE_MAX] = {};
	vector<string>								m_MeshNames;

	unique_ptr<C3DModel>						m_pModel     = { nullptr };
	unique_ptr<CMesh>							m_pMesh      = { nullptr };
	unique_ptr<CMeshNonAnime>					m_pMeshNonAnime	= { nullptr };
	unique_ptr<class CNonModel>					m_pNonModel = { nullptr };
	unique_ptr<class CAnimator>					m_pAnimator  = { nullptr };
	unique_ptr<class CAnimation>				m_pAnimation = { nullptr };
	unique_ptr<class CBone>						m_pBone		 = { nullptr };

	map<string, int32_t>						m_BoneList;
	vector<Bone>								m_Bones;
	vector<ComPtr<ID3D11ShaderResourceView>>	m_TextureLists = {};
	map<string,uint32_t>						m_TextureMapIdList = {};
	
	map<string, uint32_t>						m_MeshMapIdList = {};
	vector<shared_ptr<CMeshNonAnime>>			m_MeshLists = {};

	_float4x4									m_PreMatrix;
	string										m_strDirectory	 = {}, m_strMeshName = {},	m_strFileName = {};
	uint32_t									m_BoneCounter = { 0 };
	uint32_t									m_iMeshCnt	  = { 0 };
;
	int32_t										m_iTextureSkipBind= {-1};
	_float3										m_fMin = {}, m_fMax= {};

	string										m_strbinary = {};
	_bool										m_bMode;
	MESH_TYPE									m_eType;
public:
	static unique_ptr<CAssimp_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END

