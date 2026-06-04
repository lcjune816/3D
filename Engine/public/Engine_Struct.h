#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Function.h"
#include "DirectXTK-main/DDSTextureLoader.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND		hWnd;
		HINSTANCE   hInstance;
		WINMODE		eWinMode;
		uint32_t		iWinSizeX, iWinSizeY;
		uint32_t	iNumLevels;
	}ENGINE_DESC;
	
	typedef struct tagEdgeStruct
	{
		_bool bCheck;
		_float3 fPos;
		_float3 fNormal[3];
		_bool	m_bLeft{ true };
	}GRAB_ARM_EDGE;
	typedef struct tagevent
	{
		WORLD_EVENT eEvent;
		int32_t iIndex = { -1 };
		_float3 fPos{};
		void* pArg = nullptr;
	}EVENT;
	typedef struct strcollisioninfo
	{
		_float3 DstLocalPos;
		_float3 DstWorldPos;
		_float3 SrcLocalPos;
		_float3 SrcWorldPos;
		_float3 LocalExtents;
		_float fDist;
	}COLLISION_INFO;

	typedef struct strblend
	{
		_vector vPos;
		_vector vRot;
		_vector vScale;
	}BONE_BLEND;
	typedef struct tagEngineCollisionDesc
	{
		_float2 fMouse;
		_float3 fPos;
		_float3 fDir;

		_float4x4 matView;
		_float4x4 matProj;

	}COLLISION_DESC;

	typedef struct ObjectInfo
	{
		_float3 fPos;
		_float3 fScale;
		_float3 fRot;

	}INFO;
	typedef struct Texture
	{
		uint32_t iTextureID;
		string path;
	}TEXTURE;

	typedef struct tagInstancing
	{
		ComPtr<ID3D11Buffer> pVB;
		ComPtr<ID3D11Buffer> pIB;
		vector<TEXTURE>		Textures[AI_TEXTURE_TYPE_MAX];
		vector<_float4x4>	matWorlds;
		uint32_t  indices;
		uint32_t  Vertices;

		vector<weak_ptr<class CGameObject>> ObjectsTemp;
	}INSTANCING_DESC;

	typedef struct taglight
	{
		_float4		vLightDir, vLightDiffuse, vLightAmbient, vLightSpecular;

		_float4     vMtrlAmbient, vMtrlSpecular;

	}LIGHT_VALUE;
	typedef struct AstarNode
	{
		_float	  G, H, F;
		int32_t   iParent_node{-1}, iNode_Nubmer;
		_float3	  Pos;

	}ENGINE_ASTAR;

	typedef struct strnavimesh
	{
		int32_t		iIndex = { -1 };
		_float3     vPoints[ETOUI(EPOINT::END)];
		_float3		vNormals[ETOUI(LINE::END)];
		int32_t		iNeighborIndices[ETOUI(LINE::END)] = { -1,-1,-1 };
		_float4     m_vPlane;
		_float3		vCenter;
		_bool		bDead{ false };
	}NAVI;

	typedef struct filepath
	{
		string strFileName;
		string strPathName;
	}FPATH;

	typedef struct vertexpos
	{
		_float3		fPos;			//0

		static constexpr uint32_t		iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] =
		{
			{"POSITION",0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)(this), sizeof(VTX_POS));
		}
	}VTX_POS;
	
	typedef struct vertexnoanime
	{
		_float3		fPos;			//0
		_float3     vNormal;
		_float2		texcoord;		//12
		
		static constexpr uint32_t		iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] =
		{
			{"POSITION",0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",0   ,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT   ,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)(this), sizeof(VERTEX_NOANIME));
		}
	}VERTEX_NOANIME;

	typedef struct vertex
	{
		_float3		fPos;			//0
		_float2		texcoord;		//12
		uint32_t	iBoneIDs[4];	//20
		_float		fWeights[4];	//36 
		static constexpr uint32_t		iNumElements = {4 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements]  =
		{
			{"POSITION",0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT   ,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT",0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }

		};
		
		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)(this), sizeof(VERTEX));
		}
	}VERTEX;
	
	typedef struct tagVertexInstanceParticle
	{
		_float4		fRight, fUp, fLook, fTranslation;
		_float2		fLifeTime;
		_float4   fUv;
		_float	 fTick{};
	}VTXINSTANCE_PARTICLE;
	typedef struct tagVertexInstanceMesh
	{
		_float4		fRight, fUp, fLook, fTranslation;

	}VTXINSTANCE_MESH;
	typedef struct tagVertexInstanceNonAnime_Mesh
	{
		static constexpr uint32_t			iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] = {
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,   0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",  0,DXGI_FORMAT_R32G32B32_FLOAT,   0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,      0,24,D3D11_INPUT_PER_VERTEX_DATA,0},

			{"TEXCOORD",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1, 0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",4,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},

		};

	}VTXINSTANCE_NONANIME_MESH;
	
	typedef struct tagVertexInstanceParticle_Rect
	{
		static constexpr uint32_t			iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] = {
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},

			{"WORLD",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1, 0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"WORLD",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"WORLD",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"WORLD",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,72,D3D11_INPUT_PER_INSTANCE_DATA,1},

		};

	}VTXINSTANCE_PARTICLE_POINT;


	typedef struct vertexbone
	{
		_float3		fPos;			//0
		_float2		texcoord;		//12
		static constexpr uint32_t		iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] =
		{
			{"POSITION",0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT   ,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};

		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)(this), sizeof(vertexbone));
		}

	}VERTEX_BONE;


	typedef struct vertexbox
	{
		_float3		fPos;			
		_float4		fColor;			
		static constexpr uint32_t		iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] =
		{
			{"POSITION",0 ,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}

		};
	}VERTEX_BOX;

	//저장 필요


	//저장 필요
	typedef struct Bone
	{
		uint32_t index = 0;
		_float4x4	matBone;

		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)this, sizeof(Bone));
		}

	}BONE;

	typedef struct AnimationTick
	{
		_float m_Duration;
		_float m_iTicksPerSecond;

		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)this, sizeof(ANITIME));
		}
	}ANITIME;

	//저장 필요
	typedef struct mesh
	{
		vector<VERTEX>	_VertexList;
		vector<uint32_t>  _Indices;
		vector<TEXTURE>	  _TextureList[AI_TEXTURE_TYPE_MAX];
	
		void Load_Data(ifstream& fRead,ID3D11Device* pDevice, ID3D11DeviceContext* pContext, map<string,
			uint32_t>& TextureMapID, vector< ComPtr<ID3D11ShaderResourceView>>& pTextureList)
		{
			Load_Binary_Data_Array(_VertexList, fRead);
			Load_Binary_Data_Array(_Indices, fRead);

			for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			{

				size_t iTextureSize = 0;
				fRead.read((char*)(&iTextureSize), sizeof(size_t));
				_TextureList[i].reserve(iTextureSize);
				for (size_t j = 0; j < iTextureSize; ++j)
				{
					TEXTURE tex{};
					size_t iLength = 0;
					fRead.read((char*)(&iLength), sizeof(iLength));
					tex.path.resize(iLength);
					fRead.read((char*)(&tex.path[0]), sizeof(char) * (iLength));

					_wstring PathName(tex.path.begin(), tex.path.end());

					auto iter = TextureMapID.find(tex.path);
					if (iter == TextureMapID.end())
					{
						ComPtr<ID3D11ShaderResourceView> pTexture;

						uint32_t id = TextureMapID.size();

						if (FAILED(CreateDDSTextureFromFile(pDevice, nullptr, PathName.c_str(), nullptr, &pTexture)))
							MSG_BOX("바이너리 텍스쳐 개같이 실패");


						TextureMapID.emplace(tex.path, id);
						tex.iTextureID = id;
						pTextureList.push_back(pTexture);

					}
					else
					{
						tex.iTextureID = iter->second;
					}

					_TextureList[i].emplace_back(tex);
				}
			}
			
		}
		void Save_Data(const string& fWrite)
		{
			ofstream file(fWrite, std::ios::binary | std::ios::app);
			Save_Binary_Data_Array(_VertexList, file);
			Save_Binary_Data_Array(_Indices, file);
			
			for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			{
				size_t iTextures = _TextureList[i].size();
			
				file.write((char*)(&iTextures), sizeof(size_t));

				for (size_t j = 0; j < iTextures; ++j)
				{
					size_t iLength = _TextureList[i][j].path.length();
					file.write((char*)(&iLength), sizeof(size_t));
					file.write((char*)(_TextureList[i][j].path.data()), sizeof(char) * iLength);
				}
			}
			file.close();
		}

	}MESH;

	//저장 필요
	struct KeyPosition
	{
		_float3 fPos;
		_float fTimeStamp;
	};
	//저장 필요
	struct KeyRotation
	{
		_float4 orientation;
		_float fTimeStamp;
	};
	//저장 필요
	struct KeyScale
	{
		_float3 fScale;
		_float	fTimeStamp;
	};


	//저장 필요
	typedef struct KeyAnimation
	{
		vector<KeyPosition> vecPos;
		vector<KeyRotation> vecRot;
		vector<KeyScale> vecScale;

		void Save_Data(const string& fWrite)
		{
			ofstream file(fWrite, std::ios::binary | std::ios::app);

			Save_Binary_Data_Array(vecPos, file);
			Save_Binary_Data_Array(vecRot, file);
			Save_Binary_Data_Array(vecScale, file);
			file.close();
		}

		void Load_Data(ifstream& fRead)
		{
			Load_Binary_Data_Array(vecPos, fRead);
			Load_Binary_Data_Array(vecRot, fRead);
			Load_Binary_Data_Array(vecScale, fRead);	
		}
	}KEYANI;
	
	//저장 필요
	struct AssimpNodeData
	{
		_float4x4 transformation;
		int32_t	index;
		uint32_t	iChildrenCount;
		vector<AssimpNodeData> vecChildern;

		void Save_Data(ofstream& fWrite)
		{
			fWrite.write((char*)(&transformation), sizeof(_float4x4));
		
			fWrite.write((char*)(&index), sizeof(int32_t));

			fWrite.write((char*)(&iChildrenCount), sizeof(uint32_t));

			
			for (size_t i = 0; i < iChildrenCount; ++i)
			{
				vecChildern[i].Save_Data(fWrite);
			}

		}
	};

	typedef struct tagImportModel
	{
		string pFile;
		int32_t bAllModel;
		MESH_TYPE eType;
	}IMPORTMODEL_DESC;
}


#endif // Engine_Struct_h__
