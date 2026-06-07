#pragma once
#include "Prototype_Manager.h"

NS_BEGIN(Engine)
class ENGINE_DLL CGameInstance
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
public:
	virtual ~CGameInstance();

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext);
	HRESULT		Update_Engine(_float fTimeDelta);
	HRESULT		Draw();
	void		Clear_Resources(uint32_t iClearLevelIndex);
	_float2		Get_ViewportSize() const {
		return m_fViewportSize;
	}
	_float	Random(_float fMin, _float fMax);
public:
#pragma region TIMER_MANAGER
	_float		Get_TimeDelta(const _wstring& strTimerTag);
	void		Compute_TimeDelta(const _wstring& strTimerTag);
	HRESULT		Add_Timer(const _wstring& strTimerTag);
#pragma endregion
#pragma region INPUT_DEVICE
	int8_t	Get_DIKeyState(uint8_t byKeyID);
	int8_t	Get_DIMouseState(DIMK eMouse);
	int32_t	Get_DIMouseMove(DIMM eMouseState);
	int8_t Get_DIKeyOneState(DIMKEYINPUT eInput);
	int8_t Get_DIMouseOneClick(DIMK eMouse, ENGINE_MOUSE eState = ENGINE_MOUSE::HOLD);
#pragma endregion
#pragma region GRAPHIC_DEVICE
	HRESULT	Clear_BackBuffer_View(const _float4* pClearColor);
	HRESULT	Clear_DepthStencil_View();
	HRESULT Present();
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(uint32_t iNewLEvelIndex, unique_ptr<class CLevel> pNewLevel);
	const uint32_t              Get_CurrentLevel();
#pragma endregion


#pragma region PROTOTPYE_MANAGER
	HRESULT	Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<CPrototype> pPrototype);
	shared_ptr<CPrototype> Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
#pragma endregion

#pragma region GAMEOBJECT_MANAGER
	HRESULT Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	CGameObject* Get_ObjectPtr(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _char* ObjTag);
	class CLayer* Find_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag);
	HRESULT     Resize_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, int32_t iResize);
	void		Save_Data(uint32_t iNumLevel,_wstring path, const _wstring& strLayerName, const string& pSaveArrayName);
	
	void ReBindComponents(uint32_t iLayerLevelIndex);
	HRESULT Add_GameObject_ToCopyLayer(uint32_t iPrototypeLevelIndex, const _wstring strPrototypeTag, void* pArg);
	HRESULT Move_CopyLayer_ToObjectLayer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag);
	void	Reset_CopyLayer();
	void	Move_To_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _wstring& strNextLayerTag, shared_ptr<CGameObject> pObj);
	void    Move_Tol_AllLayer(uint32_t iLayerCurrentLevelIndex, const _wstring& strNextLayerTag, list<shared_ptr<class CGameObject>> pObj);
	void	Add_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strLayerTag);
	
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pRenderObject);
	const uint32_t		Get_RanderCall();
#pragma endregion
#pragma region COLLISION_MANAGER
	weak_ptr<class CGameObject> Check_Ray(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, COLLISION_DESC& tagCollision);
	_bool			 Get_Collision(); 
	void			 Set_Collision(_bool bCollision);
	_bool				 Only_AABB_Collision(const weak_ptr<CTransform> pSrcTransform, const weak_ptr<CTransform> pDstTransform,_bool bBack = true, COLLISION_INFO* pstrCollision = nullptr);
	class CGameObject*		AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, weak_ptr<CGameObject> pObj);
	_bool	AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, _vector readStart, _vector startmat, _fvector endMat, _cmatrix OriginMatrix, vector<GRAB_ARM_EDGE>& EdgePoses,  vector<uint32_t>& iSizecnt,_bool bFinished, _bool bCheck = false);
	_bool					RayCast(const uint32_t endLayerIndex, const _wstring& strCompareLayerName, const _wstring& LayerName, const _char* tagName, weak_ptr<CTransform> pSrcTransform, _fvector OffsetRay);
	void					Add_Check_Collision(COLLISION eCollisionValue, weak_ptr<CGameObject> pObj);
	weak_ptr<CGameObject>	Matrix_Check_Collision(_fmatrix Checck, COLLISION eCollisionValue);
	_vector					CheckMesh_Triangle(shared_ptr<CGameObject> pObj, const vector<uint32_t>& MeshNumbers, _fvector vOriginPos, _fvector vOriginDir);
#pragma endregion
 
#pragma region INSTANCING

	HRESULT				   Add_Instancing_Data(uint32_t iIndex, INSTANCING_DESC InstanceData);
	_bool					Create_Instancing_Desc(INSTANCING_DESC& InstanceData);
	HRESULT					Add_Instancing_ObjectData(const uint32_t iIndex, _fmatrix World, shared_ptr<CGameObject> pObj);
#pragma endregion

#pragma region PIPELINE
	const _float4x4* Get_Transform(D3DTS eState);
	const _float4x4* Get_Transform_Inverse(D3DTS eState);
	const _float4* Get_CamPosition();
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);
#pragma endregion

#pragma region TRIGGER_MANAGER
	HRESULT								 Add_Trigger(uint32_t iLevelIndex, uint32_t iTargetNumber, weak_ptr<CTrigger> pTrigger);
	weak_ptr<CTrigger>					 Find_Trigger(uint32_t iLevelIndex, uint32_t iTargetNumber);
#pragma endregion 

#pragma region TEXTURE_MANAGER

	HRESULT							Add_Decal_Texture(const string filePath);
	ID3D11ShaderResourceView*		Find_Decal_Texture(const uint32_t& strFileName);
	HRESULT							Add_Direct_Decal_Texture(const string filePath, ComPtr<ID3D11ShaderResourceView> pTexture);
	const	vector<string>&			Get_TextureFileNameList();
	const int32_t					Find_TextueId(const string& filePath);

	void						Set_Color(_float4 fColor);
	_float4*						ColorTester();
#pragma endregion
#pragma region LIGHT_MANAGER

	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer);
	HRESULT						Add_LightMtrl(const string tagLight);
	weak_ptr<LIGHT_VALUE>		Find_LightMtrl(const string tagLightName);

	const vector<string>& Get_ObejctNames();
#pragma endregion
#pragma region NAVI_MANAGER
	shared_ptr<class CCell>  Select_TriAngle(_fvector vOrigin, _fvector vDir);
	void	Add_NaviMeshInfo(_float3* fPos, CELL_EVENT eEvent);
	_bool	Check_NeraPos(_float3* fPos);
	_bool	Check_First();
	HRESULT    Save_Navi(const _wstring& FilePath, const _char* pName);
	HRESULT    Load_Navi(const _wstring& FilePath, const _char* pName);
	void		 Ready_Neightbors();
	void		 Render_Navi();
	void		 Undo_Cell();

	void Connect_Navigaion(shared_ptr<class CNavigation> pNavigation);
#pragma endregion

#pragma region EVENT_MANAGER
	void				  Notify(const WORLD_EVENT& eEvent, const EVENT& event);
	HRESULT				  Add_Observers(const WORLD_EVENT& eEvent, shared_ptr<class CObserver> pObserver);
#pragma endregion
#pragma region PARTICLE_MANAGER
	    HRESULT	   Add_ParticleToPool(const _wstring strPrototypeTag, uint32_t iPrototypeLevel, uint32_t iGameLevel, void* pArg);
	    void	   Particle_Emit(WORLD_EVENT eParticleType);
		weak_ptr <class CParticleObject>	Select_Particle_Object(_fvector vOrigin, _fvector fDir);

#pragma endregion
#pragma region ASSIMP_MANAGER
	shared_ptr<class CMesh>				ImportOnlyMesh(void* pArg);
	HRESULT		ImportModel_Anime(const IMPORTMODEL_DESC& tagModel, vector<shared_ptr<class CVIBuffer>>& pPrototype, shared_ptr<class CAnimator>& pAnimator, weak_ptr<class CTransform> pTransform, _matrix Premat);
	HRESULT		ImportModel_NonAnime(const IMPORTMODEL_DESC& tagModel, weak_ptr<class CTransform> pTransform, vector<uint32_t>& nameList);
	
	const string						Get_Binary_Path();
	ID3D11ShaderResourceView*		    Find_Texture(const uint32_t id);
	CMeshNonAnime*						Find_Mesh(const uint32_t id);
	vector<VERTEX_NOANIME>* Get_MeshVetexesLists(uint32_t index);
	vector<uint32_t>* Get_MeshIndicesLists(uint32_t index);
#pragma endregion

#pragma region IMGUI
	_bool					WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	const string&			Find_Path(const string& strFileName);
	const vector<string>&	Get_FileNameList();
	string					Model_Animation(const vector<string>& pNames);
	void					Add_FilePath(const string fileName, const string filePath);
#pragma endregion
#pragma region TARGET_MANAGER
public:
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, shared_ptr<class CShader> pShader, const _char* pConstantName);


#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_RT_Render(const _wstring& strMRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr<class CRect> pVIBuffer);
#endif
#pragma endregion




private:
	_float2										m_fViewportSize = {};
private:
	unique_ptr<class CTimer_Manager>			m_pTimer_Manager  = { nullptr };
	unique_ptr<class CGraphic_Device>			m_pGraphic_Device = { nullptr };
	unique_ptr<class CLevel_Manager>			m_pLevel_Manager = { nullptr };
	unique_ptr<class CPrototype_Manager>		m_pPrototype_Manager = { nullptr };
	unique_ptr<class CObject_Manager>		    m_pObject_Manager = { nullptr };
	unique_ptr<class CRenderer>					m_pRenderer = { nullptr };
	unique_ptr<class CCollisionManager>			m_pCollision_Manager = { nullptr };
	unique_ptr<class CTrigger_Manager>			m_pTrigger_Manager = { nullptr };
	unique_ptr<class CPipeLine>					m_pPipeLine = { nullptr };
	unique_ptr<class CTexture_Manager>			m_pTexture_Manager = { nullptr };
	unique_ptr<class  CInput_Device>			m_pInput_Device = { nullptr };
	unique_ptr<class CLight_Manager>			m_pLight_Manager = { nullptr };
	unique_ptr<class CInstancing>				m_pInstancing = { nullptr };
	unique_ptr<class CNavi_Manager>				m_pNavi_Manager = { nullptr };
	unique_ptr<class CEvent_Manager>			m_pEvent_Manager = { nullptr };
	unique_ptr<class CParticle_Manager>			m_pParticle_Manager = { nullptr };
	unique_ptr<class CTarget_Manager>			m_pTarget_Manager = { nullptr };
	unique_ptr<class CAssimp_Manager>			m_pAssimp_Manager = { nullptr };
	unique_ptr<class CImGuiManager>				m_pGui_Manager	  = { nullptr };
	
public:
	void Release_Engine();
};
NS_END
