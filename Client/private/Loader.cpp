#include "Loader.h"
#include "Loader_Defines.h"


CLoader::CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : m_pDevice(pDevice),m_pContext(pContext)
{

}
CLoader::~CLoader()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);
}
uint32_t APIENTRY ThreadMain(void* pArg)
{
	auto		pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}
HRESULT CLoader::Initialize(LEVEL eNextLevelIndex)
{
	InitializeCriticalSection(&m_CriticalSection);

	m_eNextLevelIndex = eNextLevelIndex;
	
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;

}
HRESULT	CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	HRESULT			hr = { };

	switch (m_eNextLevelIndex)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case LEVEL::GASZONE:
		hr = Loading_For_GasProduction();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	CoUninitialize();
	
	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
		m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_PlayerRightHand"),
		CPLayer_RightHand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Arm"),
		CPlayer_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Aim"),
		CPlayerAim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_PlayerLeftHand"),
		CPlayer_LeftHand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_WorldObject"),
		CWorldObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Terrian"),
		CStageone_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Trigger"),
		CTriggerObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Decal"),
		CDecalObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Teacher"),
		CBoss_Teacher::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Particle"),
		CWorldParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	////////////////////////fFFFFFFSSSSSSSMMMMMMMMM///////////////////////////////////////////
	if(FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),TEXT("FSM_Machine"),
		CFSM_Machine::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_Idle"),
		CFSM_Idle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_Move"),
		CFSM_Move::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_Jump"),
		CFSM_Jump::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_Crouch"),
		CFSM_Crouch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_Hand"),
		CFSM_Hand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_LeftHand"),
		CFSM_LeftHand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_RightHand"),
		CFSM_RightHand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("FSM_Dead"),
		CFSM_Dead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///////////////////////////////////////////보스꺼/////////////////////////////////////////

	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("FSM_Teacher_Idle"),
		CFSM_Teacher_IDLE::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("FSM_Teacher_Move"),
		CFSM_Teacher_Move::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("FSM_Teacher_Spawn"),
		CFSM_Teacher_Spawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("FSM_Teacher_Dead"),
		CFSM_Teacher_Daed::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//////////////////////////큐브ㅡㅡㅡ////////////////////////////////////////////////////
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Cube",
		Engine::CCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//////////////////////////트ㅡㅡㅡㅡㅡ리ㅣㅣㅣ거ㅓㅓㅓㅓㅓㅓ//////////////////////////////
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Door"),
		CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Lever"),
		CLever::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_RollupDoor"),
		CRollupDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_GreenElectric"),
		CGreenElectric::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_BlueElectric"),
		CBlueElectric::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_ElectricPole"),
		CElectricPole::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Battery"),
		CBattery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_BatteryCase"),
		CBatteryCase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_PoleHead"),
		CElectricPoleHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_ElectricPannel"),
		CElectricPannel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_LowerFlip"),
		CLowerFlip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_LowerFlip_Flip"),
		CLowerFlip_Flip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Triangle"),
		CTriAngle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Generator"),
		CGenerator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Elevator"),
		CElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Button"),
		CButton::Create(m_pDevice, m_pContext))))
		return E_FAIL; 

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Player_LIght"),
		CPlayer_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("OBJ_Instance_Mesh"),
		CVIBuffer_Instance_Mesh::Create(m_pDevice, m_pContext,nullptr))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Light"),
		CWorldLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("OBJ_Instancing_WorldObject"),
		CInstance_WorldObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, L"../../Navi.json", "Navi"))))
		return E_FAIL;

	CVIBuffer_Particle_Point::PARTICLE_INSTANCE_DESC ParticleDesc{};

	ParticleDesc.fRange = _float3(5.f, 20.f, 5.f);
	ParticleDesc.iNumInstances = 50;
	ParticleDesc.isLoop = true;
	ParticleDesc.vCenter = _float3(0, 0, 0);
	ParticleDesc.vGrid = _float2(0.f, 0.f);
	ParticleDesc.vLifeTime = _float2(1.f, 0.2f);
	ParticleDesc.vSize = _float2(0.005f, 0.4f);
	ParticleDesc.vSpeed = _float2(10.f, 15.f);
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Component_Buffer_Particle_Spark"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	ParticleDesc.fRange = _float3(0.f, 0.f, 1.f);
	ParticleDesc.iNumInstances = 1;
	ParticleDesc.isLoop = true;
	ParticleDesc.vCenter = _float3(0, 0, 0);
	ParticleDesc.vGrid = _float2(8.f, 8.f);
	ParticleDesc.vLifeTime = _float2(0.3f, 0.8f);
	ParticleDesc.vSize = _float2(15, 60.f);
	ParticleDesc.vSpeed = _float2(10.f, 15.f);
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Component_Buffer_Particle_Fog"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	ParticleDesc.fRange = _float3(0.f, 0.f, 1.f);
	ParticleDesc.iNumInstances = 1;
	ParticleDesc.isLoop = true;
	ParticleDesc.vCenter = _float3(0, 0, 0);
	ParticleDesc.vGrid = _float2(8.f, 8.f);
	ParticleDesc.vLifeTime = _float2(0.3f, 0.8f);
	ParticleDesc.vSize = _float2(30, 40.f);
	ParticleDesc.vSpeed = _float2(10.f, 15.f);
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Component_Buffer_Particle_Fog_Small"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	Load_Data(LEVEL::GAMEPLAY, L"../../Objects.json", L"Layer_WorldObject", L"OBJ_WorldObject", "GameObjects");
	CGameInstance::Get().Move_Tol_AllLayer(ETOUI(LEVEL::GAMEPLAY), L"Layer_WorldObject", m_Objects);
	m_Objects.clear();
	
	Load_Data(LEVEL::GAMEPLAY, L"../../Triggers.json", L"Layer_TriggerObject", L"OBJ_Trigger",   "Triggers");
	CGameInstance::Get().Move_Tol_AllLayer(ETOUI(LEVEL::GAMEPLAY), L"Layer_TriggerObject", m_Objects);
	m_Objects.clear();
	
	//Load_Data(LEVEL::GAMEPLAY, L"../../Decal.json", L"Layer_Decal", L"OBJ_Decal",				"Decals");
	//CGameInstance::Get().Move_Tol_AllLayer(ETOUI(LEVEL::GAMEPLAY), L"Layer_Decal", m_Objects);
	//m_Objects.clear();
	Load_ParticleData(LEVEL::GAMEPLAY, TEXT("OBJ_Particle"), ETOUI(LEVEL::GAMEPLAY), TEXT("../../ParticleObjectsTo_GAMEPLAY.json"), "Particles");
	Load_LightsData(LEVEL::GAMEPLAY, TEXT("OBJ_Light"), ETOUI(LEVEL::GAMEPLAY),TEXT("../../LightsTo_GAMEPLAY.json"), "Lights");

	m_isFinished = true;
	return S_OK;
}
HRESULT CLoader::Loading_For_GasProduction()
{

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Cat"), CBoss_Cat::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_CatFog"), CCat_Fog::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("FSM_Cat_Spawn"), CFSM_Cat_Spawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("FSM_Cat_Nightmare"), CFSM_Cat_Nightmare::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_WorldObject"),
		CWorldObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Terrian"),
		CStageone_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Trigger"),
		CTriggerObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Decal"),
		CDecalObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Light"),
		CWorldLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Instancing_WorldObject"),
		CInstance_WorldObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_Particle"),
		CWorldParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, L"../../NaviGasZone.json", "Navi"))))
		return E_FAIL;
	CVIBuffer_Particle_Point::PARTICLE_INSTANCE_DESC ParticleDesc{};


	ParticleDesc.fRange = _float3(1.f,0.2f,1.f);
	ParticleDesc.iNumInstances = 10;
	ParticleDesc.isLoop = false;
	ParticleDesc.vCenter= _float3(0,0,0);
	ParticleDesc.vGrid= _float2(8.f,8.f);
	ParticleDesc.vLifeTime = _float2(1.f,3.f);
	ParticleDesc.vSize = _float2(1.f,3.f);
	ParticleDesc.vSpeed= _float2(0.2f,0.6f);
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("Component_Particle_Point"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext,&ParticleDesc))))
		return E_FAIL;
	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::GASZONE), TEXT("Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, L"../../Navi.json", "Navi"))))
	//	return E_FAIL;
	

	Load_Data(LEVEL::GASZONE, L"../../GasZone_Objects.json", L"Layer_WorldObject", L"OBJ_WorldObject", "GasZone_Object");
	CGameInstance::Get().Move_Tol_AllLayer(ETOUI(LEVEL::GASZONE), L"Layer_WorldObject", m_Objects);
	m_Objects.clear();
	Load_Data(LEVEL::GASZONE, L"../../GasZone_Trigger.json", L"Layer_TriggerObject", L"OBJ_Trigger", "GasZone_Trigger");
	CGameInstance::Get().Move_Tol_AllLayer(ETOUI(LEVEL::GASZONE), L"Layer_TriggerObject", m_Objects);
	m_Objects.clear();
	//Load_Data(LEVEL::GASZONE, L"../../GasZone_Decal.json", L"Layer_Decal", L"OBJ_Decal", "GasZone_Decal");
	//CGameInstance::Get().Move_Tol_AllLayer(ETOUI(LEVEL::GASZONE), L"Layer_Decal", m_Objects);
	//m_Objects.clear();

	Load_LightsData(LEVEL::GASZONE, TEXT("OBJ_Light"), ETOUI(LEVEL::GASZONE), TEXT("../../LightsTo_GASZONE.json"), "Lights");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Data(LEVEL eLevel, const _wstring strFilePath, const _wstring strLayerName, const _wstring strObjName, const string strLoadDataName)
{
	json j;
	ifstream file(strFilePath);
	if (!file.is_open())
	{
		MSG_BOX("로드할 파일이 없음");
		return E_FAIL;
	}
	j = json::parse(file);
	int32_t index = 0;
	for (auto& iter : j[strLoadDataName])
	{
		CTrigger::TRIGGER_DESC desc{};
		desc.FileName = { "" };
		desc.j = iter;
		desc.index = index++;
		desc.eType = iter["MeshType"];
		desc.iLevel = ETOUI(eLevel);
		if (iter["TriggerValue"] != nullptr)
		{
			desc.strTriggerName = iter["TriggerValue"];
			desc.bTrigger = iter["OtherTriggerValue"];

			desc.fArrrowRotation = iter["TriggerArrow"];
			desc.fFrameTickTime =  iter["FrameTick"];
			desc.fMaxFrameTime =   iter["FrameMaxTime"];

			int32_t WorldEvent = iter["WorldEvent"];
			int32_t Rot = iter["TriggerRot"];
			desc.eWroldEvent = static_cast<WORLD_EVENT>(WorldEvent);
			desc.eRot= static_cast<TRIGGER_ROT>(Rot);
		}
		m_Objects.push_back(static_pointer_cast<CGameObject>(CGameInstance::Get().Clone_Prototype(ETOUI(eLevel),strObjName,&desc)));

	}

	file.close();

	return S_OK;
}

HRESULT CLoader::Load_ParticleData(LEVEL eLevel, const _wstring& strPrototypeTag, uint32_t iPrototypeLevel, const _wstring& strLoadFilePathName, const string& strjsonKeyName)
{
	json j;
	ifstream file(strLoadFilePathName);
	if (!file.is_open())
	{
		MSG_BOX("로드할 파티클 없음");
		return E_FAIL;
	}
	j = json::parse(file);
	for (auto& iter : j[strjsonKeyName])
	{
		CWorldParticle::WORLDPARTICLE_DESC Desc{};
		Desc.iLevel = ETOUI(eLevel);
		Desc.eFileType = FILE_MODE::LOAD;
		Desc.j = iter;
		CGameInstance::Get().Add_ParticleToPool(strPrototypeTag, iPrototypeLevel, ETOUI(eLevel), &Desc);
	}
	file.close();

	
	return S_OK;
}

HRESULT CLoader::Load_LightsData(LEVEL eLevel, const _wstring& strPrototypeTag, uint32_t iPrototypeLevel, const _wstring& strLoadFilePathName, const string& strjsonKeyName)
{
	json j;
	ifstream file(strLoadFilePathName);

	j = json::parse(file);

	
	CGameInstance::Get().Load_Lights(iPrototypeLevel, strPrototypeTag, ETOUI(eLevel), TEXT("Layer_Light"), j);
	

	file.close();
	return S_OK;
}

unique_ptr<CLoader> CLoader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex)
{
	auto	pInstance = unique_ptr<CLoader>(new CLoader(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
		MSG_BOX("Failed to Created : CLoader");

	return pInstance;
}
