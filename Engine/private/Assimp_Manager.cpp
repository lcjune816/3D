#include "Assimp_Manager.h"
#include "GameInstance.h"
#include "MeshNonAnime.h"
#include "Bone.h"
#include "MeshModel.h"
CAssimp_Manager::CAssimp_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : m_pDevice(pDevice), m_pContext(pContext){}

CAssimp_Manager::~CAssimp_Manager() {}

HRESULT CAssimp_Manager::Update(_float fTimeDelta)
{
	return S_OK;
}
HRESULT CAssimp_Manager::Render()
{
	return S_OK;
}

unique_ptr<CAssimp_Manager> CAssimp_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CAssimp_Manager>(new CAssimp_Manager(pDevice, pContext));
	if (FAILED(pInstance->Initialize()))
		return nullptr;

	return pInstance;
}

HRESULT CAssimp_Manager::Initialize()
{
	m_pMesh      = CMesh::Create(m_pDevice, m_pContext);
	m_pAnimator  = CAnimator::Create(m_pDevice, m_pContext);
	m_pAnimation = CAnimation::Create(m_pDevice, m_pContext);
	m_pBone		 = CBone::Create(m_pDevice, m_pContext);
	m_pMeshNonAnime = CMeshNonAnime::Create(m_pDevice, m_pContext);
	return S_OK;
}


shared_ptr<CMesh> CAssimp_Manager::ImportOnlyMesh(void* pArg)
{
	if (nullptr == pArg)
		return nullptr;

	auto Desc = static_cast<CMesh::MESH_DESC*>(pArg);

	auto pInstance = m_pMesh->Clone(pArg);

	if (nullptr == pInstance)
		return nullptr;

	return static_pointer_cast<CMesh>(pInstance);
}

HRESULT CAssimp_Manager::ImportModel_Anime(const IMPORTMODEL_DESC& tagModel, vector<shared_ptr<CVIBuffer>>& pPrototype, shared_ptr<class CAnimator>& pAnimator, weak_ptr<class CTransform> pTransform,_matrix Premat)
{

	if (tagModel.pFile == "")
		return E_FAIL;
	XMStoreFloat4x4(&m_PreMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_PreMatrix, Premat);
	m_BoneList.clear();
	m_Bones.clear();
	m_iMeshCnt = 0;
	string strFbx = {}, find{};
	Check_FileName(strFbx, tagModel.pFile, find);
	//bin 없을경우 fbx 부를 이름, src 파일이름 ,bin파일 이름 결과
	if (find != "")
	{
		Binary_File_Import(pPrototype, m_strbinary, pTransform, pAnimator);
		return S_OK;
	}


	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, UINT16_MAX);
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 15.f);
	//pFile 경로를 읽어서 결과를 aiScene 구조체로 반환
	const aiScene* pScene = importer.ReadFile(strFbx,
		aiProcess_ConvertToLeftHanded |
		aiProcess_GlobalScale | aiProcessPreset_TargetRealtime_Fast
	);

	if (nullptr == pScene) {
		const char* pErrorLog;
		pErrorLog = importer.GetErrorString();
		MSG_BOX("Create Failed Assimp Scene");
		return E_FAIL;
	}
	// 매쉬 총 개수 저장 매쉬 저장 -> 애니메이션 key 관련 저장 -> bone 저장

	auto Transform = pTransform.lock();
		CAnimation::ANIMATION_DESC pAniDesc;
		CAnimator::ANIMATOR_DESC pAnimatorDesc;
		ProcessNode(pScene->mRootNode, pScene, tagModel.pFile, pPrototype);

		ofstream path(m_strbinary, std::ios::binary | std::ios::app);
		pAnimatorDesc.PreTransform = m_PreMatrix;
		Transform->Set_MaxMin(m_fMin, m_fMax);

		path.write((char*)(&m_fMax), sizeof(_float3));
		path.write((char*)(&m_fMin), sizeof(_float3));
		pAnimatorDesc.iBoneCnt = m_BoneCounter;
		path.write((char*)(&m_BoneCounter), sizeof(uint32_t));

		size_t iSize = m_Bones.size();
		path.write((char*)(&iSize), sizeof(size_t));
		path.write((char*)(m_Bones.data()), sizeof(Bone) * iSize);
		size_t iArray = m_BoneList.size();
		path.write((char*)(&iArray), sizeof(size_t));

		for (auto iter = m_BoneList.begin(); iter != m_BoneList.end(); ++iter)
		{
			string name = iter->first;
			size_t iNameSize = name.length();
			int32_t index = iter->second;
			path.write((char*)(&iNameSize), sizeof(size_t));
			path.write((char*)(name.data()), sizeof(char) * iNameSize);

			path.write((char*)(&index), sizeof(int32_t));

		}
		path.close();

		pAniDesc.BoneInfo = m_Bones;
		Load_Animation(pScene, &pAniDesc);

		pAniDesc.Bones = move(m_CopyBones);
		pAnimatorDesc.m_BoneList = m_BoneList;

		

		pAnimatorDesc.pCurretAnimation = static_pointer_cast<CAnimation>(m_pAnimation->Clone(&pAniDesc));

		pAnimator = move(static_pointer_cast<CAnimator>(m_pAnimator->Clone(&pAnimatorDesc)));
			
		//논 애니매 생성 -> 클론 -> 이니셜라이즈에서 매쉬를 불러와야 되는데
		//bin파일 없는거 중간에 추가
		CGameInstance::Get().Add_FilePath(m_strFileName, m_strbinary);
	

	return S_OK;
}


HRESULT CAssimp_Manager::ImportModel_NonAnimeModel(const IMPORTMODEL_DESC& tagModel, weak_ptr<class CTransform>& pTransform, vector<uint32_t>& IdList)
{
	if (tagModel.pFile == "")
		return E_FAIL;

	m_iMeshCnt = 0;
	string strFbx = {}, find{};
	Check_FileName(strFbx, tagModel.pFile, find);
	if (find != "")
	{
		NonAnime_Binary_File_Import(IdList, m_strbinary, pTransform);
		return S_OK;
	}


	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, UINT16_MAX);
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 15.f);
	//pFile 경로를 읽어서 결과를 aiScene 구조체로 반환
	const aiScene* pScene = importer.ReadFile(strFbx,
		aiProcess_ConvertToLeftHanded | //탄젠트/비단젠트 계산 (노말용)
		 aiProcess_GlobalScale | aiProcessPreset_TargetRealtime_Fast
	);

	if (nullptr == pScene) {
		const char* pErrorLog;
		pErrorLog = importer.GetErrorString();
		MSG_BOX("Create Failed Assimp Scene");
		return E_FAIL;
	}

	NonAniMation_ProcessNode(pScene->mRootNode, pScene, tagModel.pFile, IdList);

	ofstream path(m_strbinary, std::ios::binary | std::ios::app);
	auto Transform = pTransform.lock();
	Transform->Set_MaxMin(m_fMin, m_fMax);

	path.write((char*)(&m_fMax), sizeof(_float3));
	path.write((char*)(&m_fMin), sizeof(_float3));
	path.close();

	CGameInstance::Get().Add_FilePath(m_strFileName, m_strbinary);

	return S_OK;
}

void CAssimp_Manager::ProcessNode(aiNode* pNode, const aiScene* pScene, const string& pFile, vector<shared_ptr<CVIBuffer>>& pPrototype)
{
	size_t iMeshSize = pNode->mNumMeshes;
	
	ofstream path(m_strbinary, std::ios::binary | std::ios::app);
	path.write((char*)(&iMeshSize), sizeof(size_t));
	path.close();

	for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
		pPrototype.emplace_back(static_pointer_cast<CMesh>((ProcessMesh(mesh, pScene, pFile))));
	}
	ofstream pathChild(m_strbinary, std::ios::binary | std::ios::app);
	size_t iMeshChildren = pNode->mNumChildren;
	pathChild.write((char*)(&iMeshChildren), sizeof(size_t));
	pathChild.close();

	for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
	{
		ProcessNode(pNode->mChildren[i], pScene, pFile, pPrototype);
		
	}

}
void CAssimp_Manager::Load_Animation(const aiScene* pScene, void* pArg)
{
	assert(pScene && pScene->mRootNode);
	
	
	Read_HeirarchyData(static_cast<CAnimation::ANIMATION_DESC*>(pArg)->RootNode, pScene->mRootNode);
	Save_Binary_Data_NoArray(static_cast<CAnimation::ANIMATION_DESC*>(pArg)->RootNode, m_strbinary);

	if (NULL_TRUE(pScene->mAnimations))
	return;
		
	m_CopyBones.resize(pScene->mNumAnimations);


	//////////////////////////애니메이션//////////////////////////////
	size_t iSize = pScene->mNumAnimations;
	ofstream path(m_strbinary, std::ios::binary | std::ios::app);
	path.write((char*)(&iSize), sizeof(size_t));
	path.close();

	for (size_t i = 0; i < pScene->mNumAnimations; ++i)
	{
		ANITIME Time{};

		auto animation = pScene->mAnimations[i];
		string name = pScene->mAnimations[i]->mName.C_Str();

		Time.m_Duration = animation->mDuration;
		Time.m_iTicksPerSecond = animation->mTicksPerSecond;
		
		Read_MissingBones(m_CopyBones[i], animation);
		static_cast<CAnimation::ANIMATION_DESC*>(pArg)->AniTime.push_back(Time);
		static_cast<CAnimation::ANIMATION_DESC*>(pArg)->AnimantionName.push_back(name);
		Save_Binary_Data_NoArray(Time, m_strbinary);
		Save_Binary_Data_String(name, m_strbinary);
	}

}

void CAssimp_Manager::Read_MissingBones(vector<shared_ptr<class CBone>>& pbone, const aiAnimation* animation)
{
	uint32_t size = animation->mNumChannels;

	ofstream path(m_strbinary, std::ios::binary | std::ios::app);
	path.write((char*)(&size), sizeof(size_t));
	path.close();

	for (uint32_t i = 0; i < size; ++i)
	{

		CBone::BONE_DESC desc;
		auto channel = animation->mChannels[i];
		string boneName = channel->mNodeName.data;

		auto iter = m_BoneList.find(boneName);
		if (iter != m_BoneList.end())
		{
			desc.ID = m_Bones[iter->second].index;
			desc.index = iter->second;
			desc.pChannel = channel;


			Save_Binary_Data_String(boneName, m_strbinary);
			Save_Binary_Data_SizeT(desc.ID, m_strbinary);

			pbone.push_back(static_pointer_cast<CBone>(m_pBone->Clone(&desc)));

		}
		else
		{
			string name = "";
			Save_Binary_Data_String(name, m_strbinary);

		}
	}
	//uint32_t size = animation->mNumChannels;
	//
	//ofstream path(m_strbinary, std::ios::binary | std::ios::app);
	//path.write((char*)(&size), sizeof(size_t));
	//path.close();
	//
	//for (uint32_t i = 0; i < size; ++i)
	//{
	//
	//	CBone::BONE_DESC desc;
	//	auto channel = animation->mChannels[i];
	//	string boneName = channel->mNodeName.data;
	//
	//	auto iter = m_BoneList.find(boneName);
	//	if (iter != m_BoneList.end())
	//	{
	//		desc.ID = m_Bones[iter->second].index;
	//		desc.index = iter->second;
	//		desc.pChannel = channel;
	//		
	//
	//		Save_Binary_Data_String(boneName, m_strbinary);
	//		Save_Binary_Data_SizeT(desc.ID, m_strbinary);
	//
	//		pbone.push_back(static_pointer_cast<CBone>(m_pBone->Clone(&desc)));
	//		
	//	}
	//	else
	//	{
	//
	//		BONE mat{};
	//		XMStoreFloat4x4(&mat.matBone ,XMMatrixIdentity());
	//		mat.index = m_BoneList.size();
	//		
	//		m_BoneList.emplace(boneName, m_BoneList.size());
	//		m_Bones.push_back(mat);
	//	//	m_Bones[boneName] = m_Bones.size();
	//		desc.ID = m_BoneList[boneName];
	//		desc.index = m_BoneList[boneName];
	//		desc.pChannel = channel;
	//		//Save_Binary_Data_String(boneName, m_strbinary);
	//		//Save_Binary_Data_SizeT(desc.ID, m_strbinary);
	//
	//		++m_BoneCounter;
	//		pbone.push_back(static_pointer_cast<CBone>(m_pBone->Clone(&desc)));
	//		//string name = "";
	//		//Save_Binary_Data_String(name, m_strbinary);
	//		
	//	}
	//}
	
}

void CAssimp_Manager::Read_HeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);


	auto Index = m_BoneList.find(src->mName.data);
	if (Index == m_BoneList.end())
		dest.index = -1;
	else
		dest.index = Index->second;
		
	dest.transformation = mat_Copy(src->mTransformation);
	dest.iChildrenCount = src->mNumChildren;
	

	for (int i = 0; i < src->mNumChildren; ++i)
	{
		AssimpNodeData Data;
		Read_HeirarchyData(Data, src->mChildren[i]);
		dest.vecChildern.push_back(Data);
	}
}
shared_ptr<CPrototype> CAssimp_Manager::ProcessMesh(aiMesh* pMesh, const aiScene* pScene, const string& pFile)
{
	MESH	Data{};

	//현재 매쉬의 정점 총 개수

	for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
	{
		VERTEX vertex{};
		Set_VertexBoneDataToDefault(vertex);
		//매쉬에 저장된 정점 정보를 순회 하면서 담으라
		memcpy(&vertex.fPos, &pMesh->mVertices[i],sizeof(_float3));
		//텍스쳐 UV 좌표를 담으라
		if (pMesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (_float)pMesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (_float)pMesh->mTextureCoords[0][i].y;
		}
		//정점 로컬의 최소 최대 구하기
			m_fMin.x = min(m_fMin.x, vertex.fPos.x);
			m_fMin.y = min(m_fMin.y, vertex.fPos.y);
			m_fMin.z = min(m_fMin.z, vertex.fPos.z);

			m_fMax.x = max(m_fMax.x, vertex.fPos.x);
			m_fMax.y = max(m_fMax.y, vertex.fPos.y);
			m_fMax.z = max(m_fMax.z, vertex.fPos.z);

			Data._VertexList.push_back(vertex);
	}

	//모든 면을 돌면서 인덱스 저장
	for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
	{
		aiFace face = pMesh->mFaces[i];
		//하나의 삼각형 매쉬를 가져오고

		for (uint32_t j = 0; j < face.mNumIndices; ++j)
			Data._Indices.push_back(face.mIndices[j]);
		//삼각형이 가지고 있는 인덱스 연결 정보를 담는다

	}

	//뼈
		for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
		{
			string  BoneName = pMesh->mBones[i]->mName.C_Str();
		
			BONE mat{};
			int32_t boneID = -1;

			auto& bone = m_BoneList;
			auto iter = bone.find(BoneName);

			if (iter == m_BoneList.end())
			{
				//uint32_t i = bone.size();
				mat.matBone = mat_Copy(pMesh->mBones[i]->mOffsetMatrix);
				mat.index = m_BoneCounter;
				boneID = m_BoneCounter;
				bone.emplace(BoneName, bone.size());

				m_Bones.push_back(mat);
				++m_BoneCounter;
			}
			else
			{
				boneID = m_Bones[iter->second].index;
			}

			assert(boneID != -1);
			auto weights = pMesh->mBones[i]->mWeights;
			//mNumWights는 현재 bone이 다른 정점에 wights만큼 영향을끼치는 수
			// 500개면 500개의 정점에 현재 가중치가 더해져야됨
			
			for (uint32_t j = 0; j < pMesh->mBones[i]->mNumWeights; ++j)
			{
				uint32_t vertexid = weights[j].mVertexId;
				_float	weight = weights[j].mWeight;
				
				assert(vertexid < Data._VertexList.size());
				Set_VertexBoneData(Data._VertexList[vertexid], boneID, weight);

			}

		}
		//머테리얼 정보를 담고 있으면
 		if (pMesh->mMaterialIndex >= 0)
		{
			MATERIAL ematerial;
			aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
			for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			{
				vector<TEXTURE> diffuseMaps = LoadMaterialTextures(material, static_cast<aiTextureType>(i), "texture_diffuse", pScene, ematerial);
				Data._TextureList[i].insert(Data._TextureList[i].end(), diffuseMaps.begin(), diffuseMaps.end());
			}

		

		}


		CMesh::MESH_DESC Desc;
		Desc.m_Mesh = Data;

		Data.Save_Data(m_strbinary);

	return m_pMesh->Clone(&Desc);
}

void CAssimp_Manager::NonAniMation_ProcessNode(aiNode* pNode, const aiScene* pScene, const string& pFile, vector<uint32_t>& MeshNames)
{
	size_t iMeshSize = pNode->mNumMeshes;

	ofstream path(m_strbinary, std::ios::binary | std::ios::app);
	path.write((char*)(&iMeshSize), sizeof(size_t));
	path.close();

	for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
		string name = m_strMeshName + to_string(m_iMeshCnt);

		auto MeshID = m_MeshMapIdList.find(name);

		if (MeshID == m_MeshMapIdList.end())
		{
			uint32_t id = m_MeshLists.size();
			//매쉬 id는 0번쨰부터
			m_MeshMapIdList.emplace(name, id);
			//해당 이름에 해당하는 id가 없을경우 매쉬를 새로 추가하고
			m_MeshLists.emplace_back(static_pointer_cast<CMeshNonAnime>((NonAniMation_ProcessMesh(mesh, pScene, pFile))));
			// 그 배열 번호를 담는다
			MeshNames.push_back(id);
			

		}
		else
		{
			//만약 해당 이름 있으면 그 배열 번호만 담으시고.
			MeshNames.push_back(MeshID->second);
		}

		++m_iMeshCnt;
		

	}
	ofstream pathChild(m_strbinary, std::ios::binary | std::ios::app);
	size_t iMeshChildren = pNode->mNumChildren;
	pathChild.write((char*)(&iMeshChildren), sizeof(size_t));
	pathChild.close();

	for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
	{
		NonAniMation_ProcessNode(pNode->mChildren[i], pScene, pFile, MeshNames);

	}
}

shared_ptr<CPrototype> CAssimp_Manager::NonAniMation_ProcessMesh(aiMesh* pMesh, const aiScene* pScene, const string& pFile)
{
	vector<VERTEX_NOANIME>  Vertexes;
	vector<uint32_t>		Indices ;
	vector<TEXTURE>			Textures[AI_TEXTURE_TYPE_MAX];
	MATERIAL				eMaterial;
	//현재 매쉬의 정점 총 개수

	for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
	{
		VERTEX_NOANIME vertex{};

		//매쉬에 저장된 정점 정보를 순회 하면서 담으라
		memcpy(&vertex.fPos, &pMesh->mVertices[i], sizeof(_float3));
		memcpy(&vertex.vNormal, &pMesh->mNormals[i], sizeof(_float3));
	
		//텍스쳐 UV 좌표를 담으라
		if (pMesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (_float)pMesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (_float)pMesh->mTextureCoords[0][i].y;
		}
		m_fMin.x = min(m_fMin.x, vertex.fPos.x);
		m_fMin.y = min(m_fMin.y, vertex.fPos.y);
		m_fMin.z = min(m_fMin.z, vertex.fPos.z);
		m_fMax.x = max(m_fMax.x, vertex.fPos.x);
		m_fMax.y = max(m_fMax.y, vertex.fPos.y);
		m_fMax.z = max(m_fMax.z, vertex.fPos.z);

		Vertexes.push_back(vertex);
	}

	//모든 면을 돌면서 인덱스 저장
	for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
	{
		aiFace face = pMesh->mFaces[i];
		//하나의 삼각형 매쉬를 가져오고

		for (uint32_t j = 0; j < face.mNumIndices; ++j)
			Indices.push_back(face.mIndices[j]);
		//삼각형이 가지고 있는 인덱스 연결 정보를 담는다

	}

	//머테리얼 정보를 담고 있으면
	if (pMesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
		
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			vector<TEXTURE> diffuseMaps = LoadMaterialTextures(material, static_cast<aiTextureType>(i), m_strFileName, pScene, eMaterial);
			Textures[i].insert(Textures[i].end(), diffuseMaps.begin(), diffuseMaps.end());
		}

	}

	ofstream file(m_strbinary, std::ios::binary | std::ios::app);
	
		uint32_t iMaterial = ETOUI(eMaterial);
		Save_Binary_Data_Array(Vertexes,file);
		Save_Binary_Data_Array(Indices,file);
		file.write((char*)(&iMaterial),sizeof(uint32_t));

		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			size_t iTextures = Textures[i].size();
			file.write((char*)(&iTextures), sizeof(size_t));

			for (size_t j = 0; j < iTextures; ++j)
			{
				size_t iLength = Textures[i][j].path.length();

				file.write((char*)(&iLength), sizeof(size_t));
				file.write((char*)(Textures[i][j].path.data()), sizeof(char) * iLength);
				//iLength = Textures[i][j].type.length();
				//file.write((char*)(&iLength), sizeof(size_t));
				//file.write((char*)(Textures[i][j].type.data()), sizeof(char) * iLength);
			}
		}
	file.close();

	CMeshNonAnime::NONANIME_DESC Desc;
	Desc.Vertexes = Vertexes;
	Desc.Indices = Indices;
	for(size_t i=0; i< AI_TEXTURE_TYPE_MAX; ++i)
		Desc.Textures[i] = Textures[i];

	Desc.eMatrial = eMaterial;
	return m_pMeshNonAnime->Clone(&Desc);
}

void CAssimp_Manager::Add_Mesh(string name, shared_ptr<CMeshNonAnime>& pMesh)
{
	//auto iter = m_MeshMapList.find(name);
	//
	//if (iter != m_MeshMapList.end())
	//{
	//	m_MeshMapList.emplace(name, pMesh);
	//}
	
}


vector<TEXTURE> CAssimp_Manager::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType Type, string typeName, const aiScene* pScene ,MATERIAL& eMaterial)
{
	vector<TEXTURE> textures;

	uint32_t iTextureNumber = pMaterial->GetTextureCount(Type);
	aiString matName = {};
	pMaterial->Get(AI_MATKEY_NAME, matName);
	string name = "";
	name = matName.C_Str();

	if (name.find("Glass") != string::npos)
	{
		eMaterial = MATERIAL::GLASS;
	}
	else eMaterial = MATERIAL::NORMAL;

	for (uint32_t i = 0; i < iTextureNumber; ++i)
	{
		aiString str;
		pMaterial->GetTexture(Type, i, &str);

		_bool skip = false;
		for (uint32_t j = 0; j < m_pTexture_Loaded[static_cast<int32_t>(Type)].size(); ++j)
		{
			if (strcmp(m_pTexture_Loaded[static_cast<int32_t>(Type)][j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(m_pTexture_Loaded[static_cast<int32_t>(Type)][j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			TEXTURE texture{};
			ComPtr<ID3D11ShaderResourceView> pTexture;
			filesystem::path fPath;
				fPath = string(str.C_Str());
				//파일 이름만 추출해서 filename에 담기
				string filename = fPath.filename().string();
				//파일 지점에서 . 찾기
				size_t iDot = filename.find_last_of(".");
				if (iDot != string::npos)
				{//점이 있으면 해당 범위부터 4글자를 .dds로 변환
					filename.replace(iDot, 4, ".dds");

				}
				
				string path = "../../Resource";
				string check = {};
				for (auto& iter : filesystem::recursive_directory_iterator(path))
				{
					if (iter.path().filename().string() == filename)
					{
						texture.path = iter.path().string();
						filename = iter.path().string();
						break;
					}
					check = iter.path().filename().string();
				}
				_wstring filenamews = _wstring(filename.begin(), filename.end());

				auto Texture = m_TextureMapIdList.find(check);
				if (Texture == m_TextureMapIdList.end())
				{
					uint32_t iD = m_TextureLists.size();
					if (FAILED(CreateDDSTextureFromFile(m_pDevice.Get(), nullptr, filenamews.c_str(), nullptr, &pTexture)))
						MSG_BOX("Create Failed WICTexture");

					m_TextureMapIdList.emplace(texture.path, iD);
					m_TextureLists.push_back(pTexture);
					texture.iTextureID = iD;

				}
				else
				{
					texture.iTextureID = Texture->second;
				}

				
			textures.push_back(texture);
			m_pTexture_Loaded[static_cast<int32_t>(Type)].push_back(texture);
		}

	}
	return textures;
}

ComPtr<ID3D11ShaderResourceView> CAssimp_Manager::LoadEmbeddedTexture(const aiTexture* pTexture)
{
	ComPtr<ID3D11ShaderResourceView> pComTexture = nullptr;

	if (pTexture->mHeight != 0)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pTexture->mWidth;
		desc.Height = pTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = pTexture->pcData;
		subresourceData.SysMemPitch = pTexture->mWidth * 4;
		subresourceData.SysMemSlicePitch = pTexture->mWidth * pTexture->mHeight * 4;

		ComPtr<ID3D11Texture2D> pTexture2D = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&desc, &subresourceData, &pTexture2D)))
		{
			MSG_BOX("Create Failed Texuter2D");
			return nullptr;
		}

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pComTexture)))
		{
			MSG_BOX("Create Failed ResourceView");
			return nullptr;
		}

	}
	return pComTexture;
}

void CAssimp_Manager::Set_VertexBoneData(VERTEX& vertex, uint32_t boneId, _float fweight)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		if (vertex.fWeights[i] == 0)
		{
			vertex.fWeights[i] = fweight;
			vertex.iBoneIDs[i] = boneId;
			break;
		}

	}
}

void CAssimp_Manager::Set_VertexBoneDataToDefault(VERTEX& vertex)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		vertex.iBoneIDs[i] = 0;
		vertex.fWeights[i] = 0.0f;
	}
}

void CAssimp_Manager::NonAnime_Binary_File_Import(vector<uint32_t>& idList, const string& fileName,weak_ptr<class CTransform> pTransform)
{
	CAnimation::ANIMATION_DESC pAniDesc;
	ifstream readFile(fileName, ios::binary);
	if (!readFile.is_open())
		MSG_BOX("읽기 개같이 실패");

	NonAnime_Binary_File_Import_Mesh(idList, readFile);
	_float3 fMin{}, fMax{};
	readFile.read((char*)(&fMax), sizeof(_float3));
	readFile.read((char*)(&fMin), sizeof(_float3));

	pTransform.lock()->Set_MaxMin(fMin, fMax);


	readFile.close();
}

void CAssimp_Manager::NonAnime_Binary_File_Import_Mesh(vector<uint32_t>& idList, ifstream& readFile)
{
	size_t iMeshSIze(0);

	readFile.read((char*)(&iMeshSIze), sizeof(size_t));

	for (size_t i = 0; i < iMeshSIze; ++i)
	{

		CMeshNonAnime::NONANIME_DESC Desc;
		Load_Binary_Data_Array(Desc.Vertexes, readFile);
		Load_Binary_Data_Array(Desc.Indices, readFile);
		uint32_t Material = {};
		readFile.read((char*)(&Material), sizeof(uint32_t));
		Desc.eMatrial = static_cast<MATERIAL>(Material);

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			size_t iTextureSize = 0;
			readFile.read((char*)(&iTextureSize), sizeof(size_t));
			Desc.Textures[j].reserve(iTextureSize);
			for (size_t i = 0; i < iTextureSize; ++i)
			{
				TEXTURE tex{};
				size_t iLength = 0;
				readFile.read((char*)(&iLength), sizeof(iLength));
				tex.path.resize(iLength);
				readFile.read((char*)(&tex.path[0]), sizeof(char) * (iLength));

				//readFile.read((char*)(&iLength), sizeof(iLength));
				//tex.type.resize(iLength);
				//readFile.read((char*)(&tex.type[0]), sizeof(char) * (iLength));

				_wstring PathName(tex.path.begin(), tex.path.end());

				auto iter = m_TextureMapIdList.find(tex.path);
				if (iter == m_TextureMapIdList.end())
				{
					ComPtr<ID3D11ShaderResourceView> pTexture;

					uint32_t id = m_TextureLists.size();

					if (FAILED(CreateDDSTextureFromFile(m_pDevice.Get(), m_pContext.Get(), PathName.c_str(), nullptr, &pTexture)))
						MSG_BOX("바이너리 텍스쳐 개같이 실패");


					m_TextureMapIdList.emplace(tex.path, id);
					tex.iTextureID = id;
					m_TextureLists.push_back(pTexture);
					
				}
				else
				{
					tex.iTextureID = iter->second;
				}

				Desc.Textures[j].emplace_back(tex);
			}
		}

		string name = m_strMeshName + to_string(m_iMeshCnt);
		auto MeshID = m_MeshMapIdList.find(name);

		if (MeshID == m_MeshMapIdList.end())
		{
			uint32_t id = m_MeshLists.size();
			//매쉬 id는 0번쨰부터
			m_MeshMapIdList.emplace(name, id);
			//해당 이름에 해당하는 id가 없을경우 매쉬를 새로 추가하고
			m_MeshLists.emplace_back(static_pointer_cast<CMeshNonAnime>(static_pointer_cast<CMeshNonAnime>(m_pMeshNonAnime->Clone(&Desc))));
			// 그 배열 번호를 담는다
			idList.push_back(id);


		}
		else
		{
			//만약 해당 이름 있으면 그 배열 번호만 담으시고.
			idList.push_back(MeshID->second);
		}
		++m_iMeshCnt;
		
		
	}
	size_t iChildren = 0;
	readFile.read((char*)(&iChildren), sizeof(size_t));

	for (size_t i = 0; i < iChildren; ++i)
	{
		NonAnime_Binary_File_Import_Mesh(idList, readFile);
	}
}

void CAssimp_Manager::Binary_File_Import(vector<shared_ptr<CVIBuffer>>& pPrototype,const string& fileName, weak_ptr<CTransform> pTransform, shared_ptr<CAnimator>& pAnimator)
{

	CAnimation::ANIMATION_DESC pAniDesc;
	CAnimator::ANIMATOR_DESC   pAnimatorDesc;
	ifstream readFile(fileName,ios::binary);
	if (!readFile.is_open())
		MSG_BOX("애니메이션 바이너리 읽기 개같이 실패");

	pAnimatorDesc.PreTransform = m_PreMatrix;
	Binary_File_Import_Mesh(pPrototype, readFile);
	_float3 fMin{}, fMax{};
	readFile.read((char*)(&fMax), sizeof(_float3));
	readFile.read((char*)(&fMin), sizeof(_float3));
	pTransform.lock()->Set_MaxMin(fMin, fMax);
	readFile.read((char*)(&m_BoneCounter), sizeof(uint32_t));
	pAnimatorDesc.iBoneCnt = m_BoneCounter;

	Binary_File_Import_Bone(readFile);


	if (!m_Bones.empty())
	{
		pAniDesc.BoneInfo = m_Bones;

		Binary_File_Import_HeirarachyData(readFile, pAniDesc.RootNode, 0);
		

		if (!pAniDesc.RootNode.vecChildern.empty())
		{
			size_t iAnimation = 0;
			readFile.read((char*)(&iAnimation), sizeof(size_t));
			if (iAnimation != 0)
			{
				m_CopyBones.resize(iAnimation);

				for (size_t i = 0; i < iAnimation; ++i)
				{
					Binary_File_Import_ReadMissingBone(readFile, i);
					Binary_File_Import_Animation(readFile, &pAniDesc);

				}
			}

		}
		pAnimatorDesc.m_BoneList = m_BoneList;
		pAniDesc.Bones = move(m_CopyBones);
		pAnimatorDesc.pCurretAnimation = static_pointer_cast<CAnimation>(m_pAnimation->Clone(&pAniDesc));

	
		pAnimator = move(static_pointer_cast<CAnimator>(m_pAnimator->Clone(&pAnimatorDesc)));

	}

	readFile.close();
}

void CAssimp_Manager::Binary_File_Import_Mesh(vector<shared_ptr<CVIBuffer>>& pPrototype,  ifstream& readFile)
{
	size_t iMeshSIze(0);

	readFile.read((char*)(&iMeshSIze), sizeof(size_t));

	for (size_t i = 0; i < iMeshSIze; ++i)
	{
		CMesh::MESH_DESC Desc;
		Desc.m_Mesh.Load_Data(readFile, m_pDevice.Get(), m_pContext.Get(), m_TextureMapIdList,m_TextureLists);
		pPrototype.emplace_back(static_pointer_cast<CMesh>(m_pMesh->Clone(&Desc)));
		
	}
	size_t iChildren = 0;
	readFile.read((char*)(&iChildren), sizeof(size_t));

	for (size_t i = 0; i < iChildren; ++i)
	{
		Binary_File_Import_Mesh(pPrototype, readFile);
	}
}

void CAssimp_Manager::Binary_File_Import_Bone(ifstream& readFile)
{
	Load_Binary_Data_Array(m_Bones, readFile);
	size_t iArray = {};
	readFile.read((char*)(&iArray), sizeof(size_t));
	for (size_t i = 0; i < iArray; ++i)
	{
		size_t iSize = {};
		string name = "";
		readFile.read((char*)(&iSize), sizeof(size_t));
		name.resize(iSize);
		readFile.read((char*)(name.data()), sizeof(char) * (iSize));
		int32_t iIndex = { };
		readFile.read((char*)(&iIndex), sizeof(int32_t));

		m_BoneList[name] = iIndex;

	}
}

void CAssimp_Manager::Binary_File_Import_HeirarachyData(ifstream& readFile, AssimpNodeData& dest, uint32_t iArrayCnt)
{
	_float4x4 transform = {};
	int32_t iNameSize = 0;
	uint32_t iChildSize = 0;
	readFile.read((char*)(&transform), sizeof(_float4x4));
	readFile.read((char*)(&iNameSize), sizeof(int32_t));
	readFile.read((char*)(&iChildSize), sizeof(uint32_t));

	dest.transformation = transform;
	dest.index = iNameSize;
	dest.iChildrenCount = iChildSize;

	for (size_t i = 0; i < dest.iChildrenCount; ++i)
	{
		AssimpNodeData Node{};
		Binary_File_Import_HeirarachyData(readFile, Node, dest.iChildrenCount);
		dest.vecChildern.emplace_back(Node);
	}
}

void CAssimp_Manager::Binary_File_Import_ReadMissingBone(ifstream& readFile, uint32_t iArrayCnt)
{
	uint32_t size = 0;
	readFile.read((char*)(&size), sizeof(size_t));

	for (size_t i = 0; i < size; ++i)
	{
		size_t j = 0;
		char na[256] = "";

		readFile.read((char*)(&j), sizeof(size_t));
		readFile.read((char*)(na), sizeof(char) * j);
		
		string name = na;
		auto iter = m_BoneList.find(na);
		if (iter != m_BoneList.end())
		{
			uint32_t k(0), iNumPosition(0), iNumRotation(0), iNumScale(0);
			CBone::BONE_DESC desc;

			readFile.read((char*)(&desc.ID), sizeof(uint32_t));
			readFile.read((char*)(&desc.iNumPosition), sizeof(uint32_t));
			readFile.read((char*)(&desc.iNumRotation), sizeof(uint32_t));
			readFile.read((char*)(&desc.iNumScale), sizeof(uint32_t));

			desc.check = false;
			desc.index = iter->second;
			desc.Key.Load_Data(readFile);
			m_CopyBones[iArrayCnt].emplace_back(static_pointer_cast<CBone>(m_pBone->Clone(&desc)));
		}
	
	}
	

}

void CAssimp_Manager::Binary_File_Import_Animation(ifstream& readFile, void* dest)
{
	ANITIME Time{};

	size_t iAnimationNameSize = 0;
	char	Name[256] = "";
	readFile.read((char*)(&Time), sizeof(ANITIME));

	readFile.read((char*)(&iAnimationNameSize), sizeof(size_t));
	readFile.read((char*)(&Name), sizeof(char)* iAnimationNameSize);
 	string na = Name;
	static_cast<CAnimation::ANIMATION_DESC*>(dest)->AniTime.push_back(Time);
	static_cast<CAnimation::ANIMATION_DESC*>(dest)->AnimantionName.push_back(na);
}

ID3D11ShaderResourceView* CAssimp_Manager::Find_Texture(const uint32_t& id)
{
	if (m_TextureLists.size() <= id || id == m_iTextureSkipBind)
		return nullptr;

	m_iTextureSkipBind = id;
	return m_TextureLists[id].Get();
}

CMeshNonAnime* CAssimp_Manager::Find_Mesh(const uint32_t& id)
{
	if (m_MeshLists.size() <= id)
		return nullptr;

	return m_MeshLists[id].get();
}


void CAssimp_Manager::Check_FileName(string& File, string SrcFile, string& findFile)
{
	m_fMin.x = FLT_MAX; m_fMax.x = -FLT_MAX;
	m_fMin.y = FLT_MAX; m_fMax.y = -FLT_MAX;
	m_fMin.z = FLT_MAX; m_fMax.z = -FLT_MAX;

	for(size_t i=0; i< AI_TEXTURE_TYPE_MAX; ++i)
		m_pTexture_Loaded[i].clear();

	//  모델에 넣기
	m_BoneCounter   = 0;
	m_strDirectory  = SrcFile.substr(0, SrcFile.find_last_of("/\\"));
	string filename = SrcFile;
	size_t iSlash   = SrcFile.find_last_of("\\/");
	filename = (iSlash == std::string::npos) ? filename : filename.substr(iSlash - 1);
	filesystem::path pName = filename;

	m_strMeshName = pName.stem().string();
	m_strFileName = pName.stem().string() + ".bin";
	string strFbx = pName.stem().string() + ".fbx";

	m_strbinary = "../../Resource/" + m_strFileName;
	findFile = CGameInstance::Get().Find_Path(m_strFileName);
	File = CGameInstance::Get().Find_Path(strFbx);
}

_float4x4 CAssimp_Manager::mat_Copy(const aiMatrix4x4& Dst)
{
	_float4x4 mat;
	mat._11 = Dst.a1; 	mat._12 = Dst.b1; 	mat._13 = Dst.c1; 	mat._14 = Dst.d1;
	mat._21 = Dst.a2; 	mat._22 = Dst.b2; 	mat._23 = Dst.c2; 	mat._24 = Dst.d2;
	mat._31 = Dst.a3; 	mat._32 = Dst.b3; 	mat._33 = Dst.c3; 	mat._34 = Dst.d3;
	mat._41 = Dst.a4; 	mat._42 = Dst.b4; 	mat._43 = Dst.c4; 	mat._44 = Dst.d4;
	
	return mat;
}

