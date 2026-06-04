#include "GameInstance.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Renderer.h"
#include "CollisionManager.h"
#include "Trigger_Manager.h"
#include "ImGuiManager.h"
#include "Assimp_Manager.h"
#include "PipeLine.h"
#include "Texture_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Navi_Manager.h"
#include "Instancing.h"
#include "Event_Manager.h"
#include "Particle_Manager.h"
CGameInstance::CGameInstance()
{

}
CGameInstance::~CGameInstance()
{
	
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext)
{
	m_fViewportSize = _float2(EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);

	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pOutDevice, pOutContext);
	if (NULL_TRUE(m_pGraphic_Device))
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (NULL_TRUE(m_pPrototype_Manager))
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (NULL_TRUE(m_pObject_Manager))
		return E_FAIL;

	m_pRenderer = CRenderer::Create(pOutDevice, pOutContext);
	if (NULL_TRUE(m_pRenderer))
		return E_FAIL;

	m_pCollision_Manager = CCollisionManager::Create();
	if (NULL_TRUE(m_pCollision_Manager))
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (NULL_TRUE(m_pTimer_Manager))
		return E_FAIL;

	m_pGui_Manager = CImGuiManager::Create(EngineDesc.hWnd, pOutDevice, pOutContext);
	if (NULL_TRUE(m_pGui_Manager))
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (NULL_TRUE(m_pLevel_Manager))
		return E_FAIL;

	m_pTrigger_Manager = CTrigger_Manager::Create();
	if (NULL_TRUE(m_pTrigger_Manager))
		return E_FAIL;

	m_pAssimp_Manager = CAssimp_Manager::Create(pOutDevice, pOutContext);
	if (NULL_TRUE(m_pAssimp_Manager))
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (NULL_TRUE(m_pPipeLine))
		return E_FAIL;

	m_pTexture_Manager = CTexture_Manager::Create(pOutDevice, pOutContext);
	if (NULL_TRUE(m_pTexture_Manager))
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (NULL_TRUE(m_pLight_Manager))
		return E_FAIL;

	m_pInstancing = CInstancing::Create(pOutDevice, pOutContext);
	if (NULL_TRUE(m_pInstancing))
		return E_FAIL;

	m_pNavi_Manager = CNavi_Manager::Create(pOutDevice,pOutContext);
	if (NULL_TRUE(m_pNavi_Manager))
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create();
	if (NULL_TRUE(m_pEvent_Manager))
		return E_FAIL;
	m_pParticle_Manager = CParticle_Manager::Create(EngineDesc.iNumLevels);
	if (NULL_TRUE(m_pParticle_Manager))
		return E_FAIL;
	return S_OK;
}


HRESULT	CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update_InputDev();

	m_pPipeLine->Update();

	m_pGui_Manager->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pParticle_Manager->Priority_Update_Particle(fTimeDelta);
	
	m_pObject_Manager->Update(fTimeDelta);
	m_pParticle_Manager->Update_Particle(fTimeDelta);
	m_pObject_Manager->Late_Update(fTimeDelta);
	m_pParticle_Manager->Late_Update_Particle(fTimeDelta);
	m_pLevel_Manager->Update(fTimeDelta);
	


	m_pAssimp_Manager->Update(fTimeDelta);
	return S_OK;
}
HRESULT	CGameInstance::Draw()
{
	if(FAILED(m_pRenderer->Draw()))
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

	m_pGui_Manager->Render();
	return S_OK;
}
void CGameInstance::Clear_Resources(uint32_t iClearLevelIndex)
{
	m_pObject_Manager->Clear(iClearLevelIndex);

	m_pPrototype_Manager->Clear(iClearLevelIndex);
}
#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);
}
const uint32_t		CGameInstance::Get_RanderCall()
{
	return m_pRenderer->Get_RanderCall();
}
#pragma endregion

#pragma region INSTANCING
HRESULT				   CGameInstance::Add_Instancing_Data(uint32_t iIndex, INSTANCING_DESC InstanceData)
{
	return m_pInstancing->Add_Instancing_Data(iIndex, InstanceData);
}
_bool						CGameInstance::Create_Instancing_Desc(INSTANCING_DESC& InstanceData)
{
	return m_pInstancing->Create_Instancing_Desc(InstanceData);
}
HRESULT						CGameInstance::Add_Instancing_ObjectData(const uint32_t iIndex, _fmatrix World, shared_ptr<CGameObject> pObj)
{
	return m_pInstancing->Add_Instancing_ObjectData(iIndex, World, pObj);
}
#pragma endregion
#pragma region ASSIMP_MANAGER

shared_ptr<CMesh>	CGameInstance::ImportOnlyMesh(void* pArg)
{
	return m_pAssimp_Manager->ImportOnlyMesh(pArg);
}
HRESULT CGameInstance::ImportModel_Anime(const IMPORTMODEL_DESC& tagModel, vector<shared_ptr<CVIBuffer>>& pPrototype, shared_ptr<class CAnimator>& pAnimator, weak_ptr<class CTransform> pTransform, _matrix Premat)
{
	return m_pAssimp_Manager->ImportModel_Anime(tagModel, pPrototype, pAnimator, pTransform, Premat);
}
HRESULT CGameInstance::ImportModel_NonAnime(const IMPORTMODEL_DESC& tagModel, weak_ptr<class CTransform> pTransform, vector<uint32_t>& nameList)
{
	return m_pAssimp_Manager->ImportModel_NonAnimeModel(tagModel,pTransform,nameList);
}
const string CGameInstance::Get_Binary_Path()
{
	return m_pAssimp_Manager->Get_Binary_Path();
}
ID3D11ShaderResourceView* CGameInstance::Find_Texture(const uint32_t id)
{
	return m_pAssimp_Manager->Find_Texture(id);
}
CMeshNonAnime* CGameInstance::Find_Mesh(const uint32_t id)
{
	return m_pAssimp_Manager->Find_Mesh(id);
}

#pragma endregion

#pragma region LIGHT_MANAGER

HRESULT			      CGameInstance::Add_LightMtrl(const string tagLight)
{
	return m_pLight_Manager->Add_LightMtrl(tagLight);
}
weak_ptr<LIGHT_VALUE> CGameInstance::Find_LightMtrl(const string tagLightName)
{
	return m_pLight_Manager->Find_LightMtrl(tagLightName);
}
const vector<string>& CGameInstance::Get_ObejctNames()
{
	return m_pLight_Manager->Get_ObejctNames();
}
#pragma endregion

#pragma region INPUT_DEVICE
int8_t CGameInstance::Get_DIKeyState(uint8_t byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

int8_t CGameInstance::Get_DIKeyOneState(DIMKEYINPUT eInput)
{
	return m_pInput_Device->Get_DIKeyOneState(eInput);
}
int8_t CGameInstance::Get_DIMouseState(DIMK eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

int32_t CGameInstance::Get_DIMouseMove(DIMM eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}
int8_t CGameInstance::Get_DIMouseOneClick(DIMK eMouse, ENGINE_MOUSE eState)
{

	return m_pInput_Device->Get_DIMouseOneClick(eMouse, eState);
}
#pragma endregion
#pragma region IMGUI
_bool CGameInstance::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return m_pGui_Manager->WinProc(hWnd, msg, wParam, lParam);
}
const string& CGameInstance::Find_Path(const string& strFileName)
{
	return m_pGui_Manager->Find_Path(strFileName);
}
const vector<string>& CGameInstance::Get_FileNameList()
{
	return m_pGui_Manager->Get_FileNameList();
}
void CGameInstance::Add_FilePath(const string fileName, const string filePath)
{
	m_pGui_Manager->Add_FilePath(fileName, filePath);
}


#pragma endregion
#pragma region GRAPHIC_DEVICE
HRESULT	CGameInstance::Clear_BackBuffer_View(const _float4* pClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);
}
HRESULT	CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}
HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}
#pragma endregion
#pragma region TIMER_MANAGER
_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + static_cast<_float>(rand()) / RAND_MAX * (fMax - fMin);
}
_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}
void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Set_TimeDelta(strTimerTag);
}
HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Ready_Timer(strTimerTag);
}

#pragma endregion
#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iNewLevelIndex, std::move(pNewLevel));
}
const uint32_t              CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}
#pragma endregion
#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<CPrototype> pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, std::move(pPrototype));
}
shared_ptr<CPrototype> CGameInstance::Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(iLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion
#pragma region GAMEOBJECT_MANAGER
void CGameInstance::ReBindComponents(uint32_t iLayerLevelIndex)
{
	m_pObject_Manager->ReBindComponents(iLayerLevelIndex);
}
HRESULT CGameInstance::Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
	uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	m_pObject_Manager->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
	return S_OK;
}
HRESULT CGameInstance::Resize_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, int32_t iResize)
{
	return m_pObject_Manager->Resize_Layer(iLayerLevelIndex, strLayerTag, iResize);

}
CLayer* CGameInstance::Find_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLayerLevelIndex, strLayerTag);
}
HRESULT CGameInstance::Add_GameObject_ToCopyLayer(uint32_t iPrototypeLevelIndex, const _wstring strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_ToCopyLayer(iPrototypeLevelIndex, strPrototypeTag, pArg);
}
HRESULT CGameInstance::Move_CopyLayer_ToObjectLayer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Move_CopyLayer_ToObjectLayer(iLayerLevelIndex, strLayerTag);
}
CGameObject* CGameInstance::Get_ObjectPtr(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _char* ObjTag)
{
	return m_pObject_Manager->Get_ObjectPtr(iLayerCurrentLevelIndex, strCurrentLayerTag, ObjTag);
}
void CGameInstance::Move_Tol_AllLayer(uint32_t iLayerCurrentLevelIndex, const _wstring& strNextLayerTag, list<shared_ptr<class CGameObject>> pObj)
{
	m_pObject_Manager->Move_Tol_AllLayer(iLayerCurrentLevelIndex, strNextLayerTag, pObj);
}

void	CGameInstance::Reset_CopyLayer()
{
	m_pObject_Manager->Reset_CopyLayer();
}
void	CGameInstance::Move_To_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _wstring& strNextLayerTag, shared_ptr<CGameObject> pObj)
{
	m_pObject_Manager->Move_To_Layer(iLayerCurrentLevelIndex, strCurrentLayerTag, strNextLayerTag, pObj);
}
void	CGameInstance::Add_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strLayerTag)
{
	m_pObject_Manager->Add_Layer(iLayerCurrentLevelIndex, strLayerTag);
}

#pragma endregion
#pragma region COLLISION_MANAGER

weak_ptr<CGameObject> CGameInstance::Check_Ray(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, COLLISION_DESC& tagCollision)
{
	return m_pCollision_Manager->Check_Ray(iLayerLevelIndex, strLayerTag, tagCollision);
}
_bool			 CGameInstance::Get_Collision()
{
	return m_pCollision_Manager->Get_Collision();
}
void			CGameInstance::Set_Collision(_bool bCollision)
{
	m_pCollision_Manager->Set_Collision(bCollision);
}
_bool			CGameInstance::Only_AABB_Collision(const weak_ptr<CTransform> pSrcTransform, const weak_ptr<CTransform> pDstTransform,_bool bBack, COLLISION_INFO* pstrCollision)
{
	return m_pCollision_Manager->Only_AABB_Collision(pSrcTransform, pDstTransform, bBack, pstrCollision);
}

_bool	CGameInstance::AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, _vector readStart, _vector startmat, _fvector endMat, _cmatrix OriginMatrix, vector<GRAB_ARM_EDGE>& EdgePoses, vector<uint32_t>& iSizecnt, _bool bFinished ,_bool bCheck )
{
	return m_pCollision_Manager->AABB_CheckinLayer(endLayerIndex, LayerName, readStart, startmat, endMat, OriginMatrix, EdgePoses,iSizecnt, bFinished, bCheck);
}
CGameObject* CGameInstance::AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, weak_ptr<CGameObject> pObj)
{
	return m_pCollision_Manager->AABB_CheckinLayer(endLayerIndex, LayerName, pObj);
}
_bool		CGameInstance::RayCast(const uint32_t endLayerIndex, const _wstring& strCompareLayerName, const _wstring& LayerName, const _char* tagName, weak_ptr<CTransform> pSrcTransform, _fvector OffsetRay)
{
	return m_pCollision_Manager->RayCast(endLayerIndex, strCompareLayerName,LayerName, tagName, pSrcTransform, OffsetRay);
}
void					CGameInstance::Add_Check_Collision(COLLISION eCollisionValue, weak_ptr<CGameObject> pObj)
{
	return m_pCollision_Manager->Add_Check_Collision(eCollisionValue, pObj);
}
weak_ptr<CGameObject>	CGameInstance::Matrix_Check_Collision(_fmatrix Checck, COLLISION eCollisionValue)
{
	return m_pCollision_Manager->Matrix_Check_Collision(Checck, eCollisionValue);
}
#pragma endregion

#pragma  region
void				  CGameInstance::Notify(const WORLD_EVENT& eEvent,const EVENT& event)
{
	m_pEvent_Manager->Notify(eEvent, event);
}
HRESULT				  CGameInstance::Add_Observers(const WORLD_EVENT& eEvent, shared_ptr<CObserver> pObserver)
{
	return m_pEvent_Manager->Add_Observers(eEvent, pObserver);
}
#pragma endregion

#pragma region NAVI_MANAGER
void CGameInstance::Ready_Neightbors()
{
	m_pNavi_Manager->Ready_Neightbors();
}
shared_ptr<CCell>  CGameInstance::Select_TriAngle(_fvector vOrigin, _fvector vDir)
{
	return m_pNavi_Manager->Select_TriAngle(vOrigin, vDir);
}

_bool	CGameInstance::Check_NeraPos(_float3* fPos)
{
	return m_pNavi_Manager->Check_NeraPos(fPos);
}

_bool	CGameInstance::Check_First()
{
	return m_pNavi_Manager->Check_First();
}
HRESULT    CGameInstance::Save_Navi(const _wstring& FilePath, const _char* pName)
{
	return m_pNavi_Manager->Save_Navi(FilePath, pName);
}
HRESULT    CGameInstance::Load_Navi(const _wstring& FilePath, const _char* pName)
{

	return m_pNavi_Manager->Load_Navi(FilePath, pName);
}
void	CGameInstance::Add_NaviMeshInfo(_float3* fPos, CELL_EVENT eEvent)
{
	m_pNavi_Manager->Add_NaviMeshInfo(fPos, eEvent);
}
void CGameInstance::Connect_Navigaion(shared_ptr<class CNavigation> pNavigation)
{
	m_pNavi_Manager->Connect_Navigaion(pNavigation);
}
void CGameInstance::Undo_Cell()
{
	m_pNavi_Manager->Undo_Cell();
}
void CGameInstance::Render_Navi()
{
	m_pNavi_Manager->Render();
}
#pragma endregion
#pragma region TRIGGER_MANAGER
HRESULT					CGameInstance::Add_Trigger(uint32_t iTargetNumber, weak_ptr<CTrigger> pTrigger)
{
	return m_pTrigger_Manager->Add_Trigger(iTargetNumber, pTrigger);
}
weak_ptr<CTrigger>		CGameInstance::Find_Trigger(uint32_t iTargetNumber)
{
	return m_pTrigger_Manager->Find_Trigger(iTargetNumber);
}
#pragma endregion 
#pragma region PIPELINE 
const _float4x4* CGameInstance::Get_Transform(D3DTS eState)
{
	return m_pPipeLine->Get_Transform(eState);
}
const _float4x4* CGameInstance::Get_Transform_Inverse(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Inverse(eState);
}
const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}
void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
#pragma endregion

#pragma region TEXTURE_MANAGER

HRESULT	CGameInstance::Add_Decal_Texture(const string filePath)
{
	return m_pTexture_Manager->Add_Decal_Texture(filePath);
}
ID3D11ShaderResourceView* CGameInstance::Find_Decal_Texture(const uint32_t& strFileName)
{
	return m_pTexture_Manager->Find_Decal_Texture(strFileName);
}

const	vector<string>&  CGameInstance::Get_TextureFileNameList()
{
	return m_pTexture_Manager->Get_TextureFileNameList();
}
HRESULT CGameInstance::Add_Direct_Decal_Texture(const string filePath, ComPtr<ID3D11ShaderResourceView> pTexture)
{
	return m_pTexture_Manager->Add_Direct_Decal_Texture(filePath, pTexture);
}
const int32_t					CGameInstance::Find_TextueId(const string& filePath)
{
	return m_pTexture_Manager->Find_TextueId(filePath);
}
#pragma endregion
void CGameInstance::Save_Data(uint32_t iNumLevel, _wstring path, const _wstring& strLayerName, const string& pSaveArrayName)
{
	m_pObject_Manager->Save_Data(iNumLevel, path, strLayerName, pSaveArrayName);

}
#pragma region PARTICLE_MANAGER
HRESULT	   CGameInstance::Add_ParticleToPool(const _wstring strPrototypeTag, uint32_t iPrototypeLevel, uint32_t iGameLevel, void* pArg)
{
	return m_pParticle_Manager->Add_ParticleToPool(strPrototypeTag, iPrototypeLevel, iGameLevel, pArg);
}
void	   CGameInstance::Particle_Emit(WORLD_EVENT eParticleType)
{
	m_pParticle_Manager->Particle_Emit(eParticleType);
}
weak_ptr <class CParticleObject>	CGameInstance::Select_Particle_Object(_fvector vOrigin, _fvector fDir)
{
	return m_pParticle_Manager->Select_Particle_Object(vOrigin, fDir);
}
#pragma endregion
void CGameInstance::Release_Engine()
{
	m_pLight_Manager.reset();
	
	m_pInstancing.reset();

	m_pNavi_Manager.reset();

	m_pInput_Device.reset();

	m_pCollision_Manager.reset();

	m_pTrigger_Manager.reset();
	
	m_pRenderer.reset();

	m_pPipeLine.reset();

	m_pTexture_Manager.reset();

	m_pLevel_Manager.reset();

	m_pTimer_Manager.reset();

	m_pAssimp_Manager.reset();

	m_pParticle_Manager.reset();

	m_pObject_Manager.reset();
	
	m_pPrototype_Manager.reset();

	m_pGui_Manager.reset();

	m_pGraphic_Device->Shutdown();

	m_pGraphic_Device.reset();

}