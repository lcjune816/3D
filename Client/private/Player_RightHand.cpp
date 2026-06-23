#include "GameInstance.h"
#include "Player_RightHand.h"
#include "TriggerObject.h"
#include "Player_Arm.h"
#include "FSM_RightHand.h"
#include "WorldLight.h"
#include "RightHand_Effect_Particle.h"
#include "Player_RightHand_Effect.h"
CPLayer_RightHand::CPLayer_RightHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CPlayer (pDevice, pContext)
{

}
CPLayer_RightHand::CPLayer_RightHand(const CPLayer_RightHand& Prototye) : CPlayer(Prototye)
{
}
CPLayer_RightHand::~CPLayer_RightHand()
{
};

shared_ptr<CGameObject> CPLayer_RightHand::Get_Arm()
{
	return static_pointer_cast<CPlayer_Arm>(m_pArm);
}

void CPLayer_RightHand::Hand_Pivot()
{
	auto pPlayer = m_pPlayer.lock();
	if (NULL_TRUE(pPlayer))
		return;

	_float4x4 CombinedMatrix{};
	_float4x4 BoneMatrix = pPlayer->GetAnimator()->Find_Matrix(m_iOffsetIndex);
	_matrix FinalMatrix = XMLoadFloat4x4(&BoneMatrix);

	for (uint32_t i = 0; i < 3; ++i)
		FinalMatrix.r[i] = XMVector3Normalize(FinalMatrix.r[i]);
	
	XMStoreFloat4x4(&CombinedMatrix, XMLoadFloat4x4(&m_fOffsetMatrix) *FinalMatrix * XMLoadFloat4x4(m_ParentsMatrix));
	m_pTransform->CombinedMatrix(&CombinedMatrix);
}


void	CPLayer_RightHand::State_Move()
{
	if (!m_tagHandState.bShoot&& CGameInstance::Get().Get_DIMouseOneClick(DIMK::RBUTTON, ENGINE_MOUSE::B_CLICK))
	{
		m_tagHandState.bShoot = true;
		m_pStateMachine->Change_State(FSM::HAND);
	}
}
HRESULT CPLayer_RightHand::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Character/Player/Right_Hand.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;

	_matrix mat = XMMatrixIdentity();
	mat = XMMatrixRotationY(XMConvertToRadians(180.f));
	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList, m_pAnimator, m_pTransform, mat);

	m_pArm = static_pointer_cast<CPlayer_Arm>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_Arm", nullptr));
	return S_OK;
}
HRESULT CPLayer_RightHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPLayer_RightHand::Initialize(void* pArg)
{
	auto pDesc = static_cast<RIGHT_HAND_DESC*>(pArg);
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 0.f;
	desc.m_fSpeedPerSec = 60.f;
	m_ParentsMatrix = pDesc->ParentsMatrix;
	if (FAILED(CGameObject::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	
	
	_matrix offset = XMMatrixIdentity();
	offset *= XMMatrixScaling(-0.12f, 0.12f, 0.12f);
	offset.r[3] = XMVectorSet(-2.f,-1.31f,-2.980f ,1.f);
	XMStoreFloat4x4(&m_fOffsetMatrix, offset);

	CWorldLight::WORLDLIGHT_DESC Light{};
	Light.eType = INIT_TYPE::OBJECT;
	Light.LightDesc.eUseType = USETYPE::CLIENT;
	Light.LightDesc.eType = LIGHT::POINT;
	Light.LightDesc.eLocalEventType = LIGHT_STATE::NONE;
	Light.LightDesc.eWorldEventType = WORLD_EVENT::END;
	Light.LightDesc.fRange = _float2(9.f, 0.f);
	Light.LightDesc.vDiffuse = _float4(0.4f, 0.8f, 0.4f, 1.f);
	Light.LightDesc.vAmbient = _float4(0.2f, 40.f, 0.2f, 1.f);
	Light.LightDesc.vSpecular = _float4(0.4f, 0.4f, 0.4f, 1.f);
	XMStoreFloat4(&Light.LightDesc.vPos, m_pTransform->Get_State(STATE::POS) + XMVectorSet(0, 55, 15, 0));
	m_pLight = static_pointer_cast<CWorldLight>(CGameInstance::Get().Clone_Prototype(pDesc->iLevel, L"OBJ_Light", &Light));
	//OBJ_Player_RightHand_Effect
	CPlayer_RightHand_Effect::RHAND_EFFECT_DESC RHandEffectDesc;

	RHandEffectDesc.ParentsMatrix = m_pTransform->Get_WorldPtr();
	RHandEffectDesc.eRightEffectType = RIGHTEFFECT::SPARK1;
	RHandEffectDesc.piFlag = &m_iStateFlag;
	RHandEffectDesc.vOffsetPos = _float3(-0.13f, 0.5f, -0.1f);
	RHandEffectDesc.vOffsetScale= _float3(0.036f, 0.07f, 0.031f);

	auto pObj1 = static_pointer_cast<CPlayer_RightHand_Effect>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("OBJ_Player_RightHand_Effect"), &RHandEffectDesc));

	if (NULL_TRUE(pObj1)) return E_FAIL;
	m_pRHandEffect.push_back(pObj1);

	RHandEffectDesc.eRightEffectType = RIGHTEFFECT::SPARK1;
	RHandEffectDesc.piFlag = &m_iStateFlag;
	RHandEffectDesc.vOffsetPos = _float3(-0.13f, 1.2f, -0.1f);
	RHandEffectDesc.vOffsetScale = _float3(1.5f,1.5f,1.5f);
	auto pObj2  = static_pointer_cast<CRightHand_Effect_Particle>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("OBJ_RightHand_Effect_Particle"), &RHandEffectDesc));
	
	if (NULL_TRUE(pObj2)) return E_FAIL;
	m_pRHandEffect.push_back(pObj2);
	
	
	//RHandEffectDesc.eRightEffectType = RIGHTEFFECT::SPARK2;
	//RHandEffectDesc.piFlag = &m_iStateFlag;
	//RHandEffectDesc.vOffsetPos = _float3(-0.13f, 1.f, -0.1f);
	//RHandEffectDesc.vOffsetScale = _float3(1.f, 1.f, 1.f);
	//auto pObj3= static_pointer_cast<CRightHand_Effect_Particle>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC),
	//	TEXT("OBJ_RightHand_Effect_Particle"), &RHandEffectDesc));
	//
	//if (NULL_TRUE(pObj3)) return E_FAIL;
	//m_pRHandEffect.push_back(pObj3);

	return S_OK;
}
void CPLayer_RightHand::Priority_Update(_float fTimeDelta)
{

	Timer(fTimeDelta);
	m_pArm->Priority_Update(fTimeDelta);
	for (auto& iter : m_pRHandEffect)
		iter->Priority_Update(fTimeDelta);
}
void CPLayer_RightHand::Update(_float fTimeDelta)
{
	Hand_Pivot();
	State_Move();
	m_pStateMachine->Update_Machine(fTimeDelta);

	m_pArm->Set_SameFlag(m_iStateFlag);
	m_pArm->Update(fTimeDelta);

	if (Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
	{
		XMStoreFloat4(&m_pLight->Get_LightDescPtr()->vPos, m_pTransform->Get_State(STATE::POS) + XMVectorSet(0, 0, 5, 0));
		m_pLight->Set_LightState(LIGHT_STATE::LIGHT_SLOWON);
	}
	else
		m_pLight->Set_LightState(LIGHT_STATE::LIGHT_OFF);
		
	Set_Flag(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT), FLAGVALUE::ENABLE);
	m_pLight->Update(fTimeDelta);

	for(auto& iter : m_pRHandEffect)
		iter->Update(fTimeDelta);
}
void CPLayer_RightHand::Late_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(CPLayer_RightHand));
	
	m_pLight->Late_Update(fTimeDelta);
	m_pArm->Late_Update(fTimeDelta);
	for (auto& iter : m_pRHandEffect)
		iter->Late_Update(fTimeDelta);
}
HRESULT CPLayer_RightHand::Render()
{
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pAnimator->Bind_Resource_BoneMatrix(m_pShaderCom.get(), "g_Bone");
	Bind_ResourceFromFlag(m_pShaderCom.get(), "g_Color");
	for (auto iter : m_pMeshList)
	{
		iter->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
		iter->Bind_ResourceSRV(m_pShaderCom.get(), "g_NormalTexture", aiTextureType_NORMALS, 0);
		m_pShaderCom->Begin(0);
		iter->Bind_Resource();
		iter->Render();

	}
	return S_OK;
}
void CPLayer_RightHand::Connet_Player(shared_ptr<CGameObject> pPlayer, FSM HAND, shared_ptr<CFSM_Machine> pFsmMachine, shared_ptr<class CFSM_RightHand> pState, int32_t iKey)
{
	m_pPlayer = static_pointer_cast<CPlayer>(pPlayer);
	m_iOffsetIndex = iKey;
	pState->Set_RightHand(SHARED_THIS(CPLayer_RightHand), m_pArm);
	m_pStateMachine = pFsmMachine;
	m_pStateMachine->Set_Owner(pPlayer);
	m_pStateMachine->Add_State(HAND, pState);

}
void CPLayer_RightHand::Bind_ResourceFromFlag(CShader* pShader, const _char* pConstantName)
{
	_float4 fColor{ 0,0,0,1 };
	if (Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
		fColor = { 0,1,0,1, };
	else if (Flag_Check(ETOUI(PLAYER_FLAG::CONNECTHAND)))
		fColor = { 1,0,1,1 };
	else
		fColor = { 1,1,1,1, };

	pShader->Bind_RawValue(pConstantName, &fColor, sizeof _float4);

}
unique_ptr<CPLayer_RightHand> CPLayer_RightHand::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPLayer_RightHand>(new CPLayer_RightHand(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CPLayer_RightHand");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CPLayer_RightHand::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CPLayer_RightHand>(new CPLayer_RightHand(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CPLayer_RightHand ");
		return pInstance;
	}
	return pInstance;
}
