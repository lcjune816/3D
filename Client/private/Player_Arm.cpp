#include "Player_Arm.h"
#include "GameInstance.h"

CPlayer_Arm::CPlayer_Arm(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CPlayer_Arm::CPlayer_Arm(const CPlayer_Arm& Prototye) :CGameObject(Prototye)
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

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../Resource/Character/Player/PlayerCable.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::NONANIME;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_ArrayShader"),	TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"), TEXT("Com_BoxShader"), m_pBoxShader)))
		return E_FAIL;

	m_pBoxMesh = static_pointer_cast<CCube>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Cube", &pArg));

	CGameInstance::Get().ImportModel_NonAnime(importModel, m_pTransform, m_MeshNameList);
	CGameInstance::Get().Add_LightMtrl(m_PathName);
	return S_OK;
}
void CPlayer_Arm::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(CPlayer_Arm));
}
void CPlayer_Arm::Update(_float fTimeDelta)
{
}
void CPlayer_Arm::Late_Update(_float fTimeDelta)
{
	if (0 == m_ArmMatrix.Matrix.size())
	{
		m_ArmMatrix.Matrix.resize(100);

		m_ArmMatrix.fColor.resize(100);
	}

}
HRESULT CPlayer_Arm::Render()
{

	_float4x4 matWorld{};
	_float4 fColor = {0.f, 0.f, 0.f,1.f};
	XMStoreFloat4x4(&matWorld, m_pTransform->Get_World());

	uint32_t iArraySize = m_ArmMatrix.Matrix.size();

	m_pShaderCom->Bind_Vector_Array("g_Color", m_ArmMatrix.fColor.data(), iArraySize);
	m_pShaderCom->Bind_Matrix_Array("g_World", m_ArmMatrix.Matrix.data(), iArraySize);
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

	for (auto iter : m_MeshNameList)
	{
		CMeshNonAnime* pMesh = CGameInstance::Get().Find_Mesh(iter);
		if (pMesh == nullptr)
			continue;
		pMesh->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
			
			
 		//m_pShaderCom->Bind_RawValue("g_Arm", m_ArmMatrix.data(), iArraySize);
		m_pShaderCom->Begin(0);
		pMesh->Bind_Resource();

		pMesh->Render_Array(iArraySize);

	}
	_bool bcheck = false;
	if (!m_EdgePoses.Matrix.empty())
	{

		m_pBoxShader->Bind_Vector_Array("g_ArrayColor", m_EdgePoses.fColor.data(), iArraySize);
		m_pBoxShader->Bind_Matrix_Array("g_ArrayWorld", m_EdgePoses.Matrix.data(), iArraySize);
		m_pBoxShader->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
		m_pBoxShader->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
		m_pBoxShader->Bind_RawValue("g_bChoice", &bcheck, sizeof bcheck);

		m_pBoxShader->Begin(0);
		m_pBoxMesh->Bind_Resource();
		m_pBoxMesh->Render_Array(iArraySize);
	}


	return S_OK;
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
