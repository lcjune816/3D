#include "Animator.h"
#include "Animation.h"
CAnimator::CAnimator(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{

}
CAnimator::CAnimator(const CAnimator& Prototype) : CComponent(Prototype)
{
	m_PreTransform = Prototype.m_PreTransform;
}

CAnimator::~CAnimator()
{

}

HRESULT CAnimator::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	auto Desc = static_cast<ANIMATOR_DESC*>(pArg);

	m_pCurrentAnimation = static_pointer_cast<CAnimation>(Desc->pCurretAnimation);
	m_iBoneCnt = Desc->iBoneCnt;
	m_PreTransform = Desc->PreTransform;
	m_FinalBoneMatrices.reserve(BONE_MATRIX);
	m_GlobalBoneMatrices.reserve(BONE_MATRIX);
	_float4x4 mat;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	for (uint32_t i = 0; i < BONE_MATRIX; ++i)
	{
		m_FinalBoneMatrices.push_back(mat);
		m_GlobalBoneMatrices.push_back(mat);
	}

	return S_OK;
}

HRESULT CAnimator::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_PreTransform, XMMatrixIdentity());
	return S_OK;
}

void CAnimator::Update(_float fTimeDelta)
{
	if (NULL_TRUE(m_pCurrentAnimation))
		return;

	if (m_pCurrentAnimation->Get_NameList().size() == 0)
		return;

	m_fDeltaTime = fTimeDelta;
	
	if (m_pCurrentAnimation)
	{
		m_fNoLoopTime += m_pCurrentAnimation->Get_Tick(m_iAnimationNumber) * fTimeDelta;
		m_fCurrentTime += m_pCurrentAnimation->Get_Tick(m_iAnimationNumber) * fTimeDelta;
		if (!m_bLoop)
		{
			if (!m_bFinished && m_fNoLoopTime >= m_pCurrentAnimation->Get_Duration(m_iAnimationNumber))
			{
				m_bFinished = true; //วัน๘
				m_fCurrentTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);
			}
		}
			
		m_fCurrentTime = fmod(m_fCurrentTime, m_pCurrentAnimation->Get_Duration(m_iAnimationNumber));
		CalculateBoneAnimation(&m_pCurrentAnimation->Get_RootNode(), XMMatrixIdentity() *XMLoadFloat4x4(&m_PreTransform));
	}

}

void CAnimator::Update_Socket(const _matrix& fMat)
{
	if (NULL_TRUE(m_pCurrentAnimation))
		return;


	CalculateSocketBoneMatrix(&m_pCurrentAnimation->Get_RootNode(), fMat * XMLoadFloat4x4(&m_PreTransform));
	
}

vector<string>& CAnimator::Get_NameList()
{
	return m_pCurrentAnimation.get()->Get_NameList();
}
void CAnimator::Player_Animation(unique_ptr<CAnimation> pAin)
{
	m_pCurrentAnimation = std::move(pAin);
	m_fCurrentTime = 0.f;
}

void CAnimator::CalculateBoneAnimation(const AssimpNodeData* node, FXMMATRIX parentsTrans)
{
	//JNT_R_Grabpack_Tube_01
	string nodeName = node->name;
	CBone* Bone = m_pCurrentAnimation->Find_Bone(nodeName,m_iAnimationNumber);
	XMMATRIX nodeTransform = XMLoadFloat4x4(&node->transformation);
	
	if (Bone)
	{
		XMMATRIX nodeAnimation = Bone->Bone_Update(m_fCurrentTime);
		nodeTransform = nodeAnimation;
	}
	
	XMMATRIX globalTransform;
	
	globalTransform =  nodeTransform * parentsTrans ;
	

	auto& mesh = m_pCurrentAnimation->Get_BoneInfo();

	if (mesh.find(nodeName) != mesh.end())
	{
		uint32_t idex = mesh[nodeName].index;

		XMStoreFloat4x4(&m_GlobalBoneMatrices[idex],globalTransform);
		m_GlobalBoneMap[nodeName] = idex;

		_float4x4 insertMatrix;
		XMStoreFloat4x4(&insertMatrix, globalTransform);
		m_beforeOffsetMatrix[nodeName] = insertMatrix;
		
	}
	
	for (uint32_t i = 0; i < node->iChildrenCount; ++i)
		CalculateBoneAnimation(&node->vecChildern[i], globalTransform);

}

void CAnimator::CalculateFinalBoneMatrices()
{
	
	for (auto& mesh : m_pCurrentAnimation->Get_BoneInfo())
	{
		auto matrix = mesh.second.matBone;
		auto index = mesh.second.index;
		
		_matrix offset = XMLoadFloat4x4(&matrix);

		XMStoreFloat4x4(&m_FinalBoneMatrices[index], offset * XMLoadFloat4x4(&m_GlobalBoneMatrices[index]));
		m_beforeOffsetMatrix[mesh.first] = m_FinalBoneMatrices[index];
	}


}

void CAnimator::CalculateSocketBoneMatrix(const AssimpNodeData* node, FXMMATRIX parentsTrans)
{
	string nodeName =  node->name;
	Bone* Bone = m_pCurrentAnimation->Find_SocketBone(nodeName);

	XMMATRIX nodeTransform = XMLoadFloat4x4(&node->transformation);

	if (Bone)
	{
		XMMATRIX nodeAnimation = XMLoadFloat4x4(&Bone->matBone);
		nodeTransform = nodeAnimation;
	}

	XMMATRIX globalTransform;

	globalTransform = nodeTransform * parentsTrans;

	auto& mesh = m_pCurrentAnimation->Get_BoneInfo();

	if (mesh.find(nodeName) != mesh.end())
	{
		uint32_t idex = mesh[nodeName].index;

		XMMATRIX offset = XMLoadFloat4x4(&mesh[nodeName].matBone);

		XMMATRIX final =  globalTransform;
		XMStoreFloat4x4(&m_FinalBoneMatrices[idex], (final));

	}

	for (uint32_t i = 0; i < node->iChildrenCount; ++i)
		CalculateSocketBoneMatrix(&node->vecChildern[i], globalTransform);
}

void CAnimator::Change_Animation(const string& name)
{
	uint32_t i = 0;
	for (auto& iter : m_pCurrentAnimation->Get_NameList())
	{
		if (name == iter)
		{
			m_iAnimationNumber = i;
			return;
		}
			++i;
	}
}

void CAnimator::Change_Animation_Enum(uint32_t iNumber, _bool bLoop)
{
	m_iAnimationNumber = iNumber;
	m_bLoop = bLoop;
	m_fNoLoopTime = 0.f;
	m_fCurrentTime = 0.f;


	m_bFinished = bLoop;
}

void CAnimator::Change_Final_BoneMatices(const string& str, _float4x4 boneMatrix)
{

	auto Index = m_GlobalBoneMap.find(str);

	if (Index != m_GlobalBoneMap.end())
	{
		m_GlobalBoneMatrices[Index->second] = boneMatrix;
	
	}

}

_bool CAnimator::Animation_End()
{
	return m_bFinished;
}

unique_ptr<CAnimator> CAnimator::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CAnimator>(new CAnimator(pDevice,pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CAnimator");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CAnimator::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CAnimator>(new CAnimator(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CAnimator_Clone");
		return nullptr;
	}

	return pInstance;
}
