#include "3DModel.h"
#include "GameInstance.h"
C3DModel::C3DModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CMeshModel(pDevice, pContext)
{

}
C3DModel::C3DModel(const C3DModel& Prototype) :CMeshModel(Prototype)
{

}
C3DModel::~C3DModel()
{

}
HRESULT C3DModel::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	auto pDest = static_cast<MODEL_DESC*>(pArg);

	return S_OK;
}
HRESULT C3DModel::Initialize_Prototype()
{
	return S_OK;
}

void C3DModel::Add_Animator(shared_ptr<class CAnimator> pAnimator, uint32_t	iBoneCnt)
{
	if (nullptr != pAnimator)
	{
		m_pAnimator = pAnimator;
		m_iBoneCount = iBoneCnt;
	}
	else
		pAnimator = nullptr;
		
}
HRESULT C3DModel::Update(_float fTimeDelta)
{
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Update(fTimeDelta);
	}
		
	return S_OK;
}
HRESULT C3DModel::Render()
{
	
	if (!m_pMeshList.empty())
	{
		for (auto& iter : m_pMeshList)
		{
			//iter->Bind_ResourceSRV(m_pShader.lock().get());
			iter->Bind_Resource();
			iter->Render();

		}
	}
		return S_OK;
}
vector<_float4x4>		C3DModel::Get_FinalBoneMatrix()
{
	return m_pAnimator->Get_FinalBoneMatrix();
}
void  C3DModel::Change_Animation(const string& name)
{
	m_pAnimator->Change_Animation(name);
}
void C3DModel::Change_Animation_Enum(uint32_t iAnimationNumber, _bool bLoop)
{
	m_pAnimator->Change_Animation_Enum(iAnimationNumber,bLoop);
}
_bool C3DModel::Animation_End()
{
	 return m_pAnimator->Animation_End(); 
}
const vector<string>& C3DModel::Get_NameList()
{
	if (nullptr == m_pAnimator)
		return {};

	return m_pAnimator->Get_NameList();
}
unique_ptr<C3DModel> C3DModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>pContext)
{
	auto pInstance = unique_ptr<C3DModel>(new C3DModel(pDevice,pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed 3DModel");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> C3DModel::Clone(void* pArg)
{
	auto pInstance = shared_ptr<C3DModel>(new C3DModel(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("3DMdoel Create Failed");
		return nullptr;
	}
	
	return pInstance;
}
