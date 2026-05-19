#include "GameInstance.h"
#include "Player_RightHand.h"
#include "TriggerObject.h"
#include "Player_Arm.h"
#include "FSM_RightHand.h"
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
	
	CGameInstance::Get().Add_LightMtrl(m_PathName);
	
	_matrix offset = XMMatrixIdentity();
	offset *= XMMatrixScaling(-0.1f, 0.1f, 0.1f);
	offset.r[3] = XMVectorSet(-2.f,-1.26f,-2.970f ,1.f);
	XMStoreFloat4x4(&m_fOffsetMatrix, offset);
	return S_OK;
}
void CPLayer_RightHand::Priority_Update(_float fTimeDelta)
{

	Timer(fTimeDelta);
	m_pArm->Priority_Update(fTimeDelta);
}
void CPLayer_RightHand::Update(_float fTimeDelta)
{
	Hand_Pivot();
	State_Move();
	m_pStateMachine->Update_Machine(fTimeDelta);

	m_pArm->Set_SameFlag(m_iStateFlag);
	m_pArm->Update(fTimeDelta);


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(CPLayer_RightHand));

}
void CPLayer_RightHand::Late_Update(_float fTimeDelta)
{
	
	m_pArm->Late_Update(fTimeDelta);
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
		m_pShaderCom->Begin(0);
		iter->Bind_Resource();
		iter->Render();

	}
	m_pArm->Render();
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
