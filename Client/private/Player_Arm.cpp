#include "Player_Arm.h"
#include "GameInstance.h"
#include"TriggerObject.h"

CPlayer_Arm::CPlayer_Arm(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CPlayer(pDevice, pContext)
{

}
CPlayer_Arm::CPlayer_Arm(const CPlayer_Arm& Prototye) :CPlayer(Prototye)
{
}
CPlayer_Arm::~CPlayer_Arm()
{
}

HRESULT CPlayer_Arm::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CPlayer_Arm::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 0.f;
	desc.m_fSpeedPerSec = 0.f;

	if (FAILED(CGameObject::Initialize(&desc)))
		return E_FAIL;

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../Resource/Character/Player/PlayerCable.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::NONANIME;

	if (FAILED(CGameObject::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_ArrayShader"),	TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"), TEXT("Com_BoxShader"), m_pBoxShader)))
		return E_FAIL;

	m_pBoxMesh = static_pointer_cast<CCube>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Cube", &pArg));

	CGameInstance::Get().ImportModel_NonAnime(importModel, m_pTransform, m_MeshNameList);

	m_ArmMatrix.Matrix.resize(800);
	XMStoreFloat4x4(&m_matOffset, XMMatrixIdentity());

	_float4 vOffsetPos = _float4(0, 0.5, 0.1,1.f);
	memcpy(&m_matOffset.m[3], &vOffsetPos, sizeof _float4);
	return S_OK;
}
void CPlayer_Arm::Priority_Update(_float fTimeDelta)
{

	Timer(fTimeDelta);
}
void CPlayer_Arm::Update(_float fTimeDelta)
{
	for (auto iter = m_ArmMatrix.CollisionIndex.begin(); iter != m_ArmMatrix.CollisionIndex.end();)
	{
		auto pObj = CGameInstance::Get().Matrix_Check_Collision(XMLoadFloat4x4(&m_ArmMatrix.Matrix[*iter]), COLLISION::TRIGGER).lock();

		if (NULL_FALSE(pObj))
		{
			if(static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerPtr()->Check_Trigger_Event(TRIGGER_EVENT::ELECTRICPOLE))
				static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerPtr()->Set_Trigger();
		}
		else
		{
			iter = m_ArmMatrix.CollisionIndex.erase(iter);
			continue;
		}

		++iter;

	}
}
void CPlayer_Arm::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_ArmMatrix.Matrix.size(); ++i)
	{
		XMStoreFloat4x4(&m_ArmMatrix.Matrix[i], XMLoadFloat4x4(&m_matOffset) * XMLoadFloat4x4(&m_ArmMatrix.Matrix[i]));
	}
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONLIGHT, SHARED_THIS(CPlayer_Arm));
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLOOM, SHARED_THIS(CPlayer_Arm));
}
HRESULT CPlayer_Arm::Render()
{
	_float4x4 matWorld{};
	_float4 fColor = {0.f, 0.f, 0.f,1.f};
	XMStoreFloat4x4(&matWorld, m_pTransform->Get_World());

	uint32_t iArraySize = m_ArmMatrix.Matrix.size();


	m_pShaderCom->Bind_Matrix_Array("g_World", m_ArmMatrix.Matrix.data(), iArraySize);
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	Bind_ResourceFromFlag_Default(m_pShaderCom.get(), "g_Color");

	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		if (pMesh == nullptr)
			continue;
		
		m_pShaderCom->Begin(0);
		pMesh->Bind_Resource();

		pMesh->Render_Array(iArraySize);

	}

	for (auto& iter : m_ArmMatrix.CollisionIndex)
	{
		fColor = { 1,0,0,1 };
		_matrix mat = XMLoadFloat4x4(&m_ArmMatrix.Matrix[iter]);

		for (uint32_t i = 0; i < 3; ++i)
		{
			_float fScale;
			if (i == 1)
				fScale = 1.5f;
			else fScale = 3.f;
			mat.r[i] = XMVector3Normalize(mat.r[i]) * fScale;

		}
			
		_float4x4 Combinemat{};
		XMStoreFloat4x4(&Combinemat, mat);
		m_pBoxShader->Bind_Matrix("g_World", &Combinemat);
		m_pBoxShader->Bind_RawValue("g_Color", &fColor, sizeof _float4);
		m_pBoxShader->Begin(0);
		m_pBoxMesh->Bind_Resource();
		m_pBoxMesh->Render();
	}
	
	return S_OK;
}

HRESULT CPlayer_Arm::Render_Bloom()
{

	uint32_t iArraySize = m_ArmMatrix.Matrix.size();

	m_pShaderCom->Bind_Matrix_Array("g_World", m_ArmMatrix.Matrix.data(), iArraySize);
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	Bind_ResourceFromFlag(m_pShaderCom.get(), "g_Color");

	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		if (pMesh == nullptr)
			continue;
		
		m_pShaderCom->Begin(1);
		pMesh->Bind_Resource();

		pMesh->Render_Array(iArraySize);

	}
	return S_OK;
}

void CPlayer_Arm::Bind_ResourceFromFlag(CShader* pShader, const _char* pConstantName)
{
	_float4 fColor{ 0,0,0,1 };
	_float4 fEmissive{ 0,0,0,1 };
	if (Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
	{
		fEmissive =  { 0.15f ,200.f ,0.15f ,1.f };
	}
	else if (Flag_Check(ETOUI(PLAYER_FLAG::CONNECTHAND)) || Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_LONG)))
	{
		
		fEmissive = { 0.2f,0.8f,100.f,1.f };
	}

	pShader->Bind_RawValue("g_Emissive", &fEmissive, sizeof _float4);
}

void CPlayer_Arm::Bind_ResourceFromFlag_Default(CShader* pShader, const _char* pConstantName)
{
	_float4 fColor{ 0,0,0,1 };
	_float4 fEmissive{ 0,0,0,1 };
	if (Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
	{
		fColor =  { 0.f ,3.f ,0.0f, 1.f };
	}
	else if (Flag_Check(ETOUI(PLAYER_FLAG::CONNECTHAND)) || Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_LONG)))
	{
		fColor = { 0.f,1.f,1.f,1 }; //*CGameInstance::Get().ColorTester();// { 0.f,0.0f,3.f,1 };
	}

	pShader->Bind_RawValue(pConstantName, &fColor, sizeof _float4);
}

HRESULT CPlayer_Arm::Ready_Component()
{

	return S_OK;
}
unique_ptr<CPlayer_Arm> CPlayer_Arm::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPlayer_Arm>(new CPlayer_Arm(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed WorldObject");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CPlayer_Arm::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CPlayer_Arm(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed WorldObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
