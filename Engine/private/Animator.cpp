#include "Animator.h"
#include "Animation.h"
#include "Shader.h"
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
	m_pPreAnimation    = static_pointer_cast<CAnimation>(Desc->pCurretAnimation);
	m_iBoneCnt = Desc->iBoneCnt;
	m_BoneList = Desc->m_BoneList;
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
	if (m_bStop)
	{
		m_fStopTick += fTimeDelta;
		if (m_fStopTick > 0.1f)
		{
			m_fStopTick = 0.f;
			++m_fStopTime;
		}
			
		if (m_fStopTime >= 3.f)
		{
			m_fStopTime = 3.f;
			return;
		}
	}
	else
	{
		m_fStopTime = 0.f;
		m_fStopTick = 0.f;
	}
		
	if (NULL_TRUE(m_pCurrentAnimation))
		return;

	if (m_pCurrentAnimation->Get_NameList().size() == 0)
		return;

	m_fDeltaTime = fTimeDelta;
	if (m_bIsBlending)
	{
		if (m_bNoRoot)
		{
			m_fNoLoopTime += m_pCurrentAnimation->Get_Tick(m_iAnimationNumber) * fTimeDelta * m_fDoubleSpeed;
			m_fCurrentTime += m_pCurrentAnimation->Get_Tick(m_iAnimationNumber) * fTimeDelta * m_fDoubleSpeed;

		}
		else
		{
			m_fLerpTick += fTimeDelta;
			_float t = min(1.f, m_fLerpTick / 2.2f);
			m_fNoLoopTime = 0 + (m_pCurrentAnimation->Get_Duration(m_iAnimationNumber) - 0) * t *fTimeDelta;
			m_fCurrentTime = 0 + (m_pCurrentAnimation->Get_Duration(m_iAnimationNumber) - 0) * t*fTimeDelta;
		

		}
		_float fCurDuration = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);

		if (!m_bLoop)
		{
			if (!m_bFinished && m_fNoLoopTime >= m_pCurrentAnimation->Get_Duration(m_iAnimationNumber))
			{
				m_fCurrentTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);
			}
			else if (m_bFinished)
			{

				m_fNoLoopTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);
				m_fCurrentTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);

			}
		}
		else
		{
			m_fCurrentTime = fmod(m_fCurrentTime, m_pCurrentAnimation->Get_Duration(m_iAnimationNumber));
		}
	
		m_fBlendElapsed += fTimeDelta;
		_float fBlendTime = max(0.f,min(1.f, m_fBlendElapsed / m_fBlendDuration));
		
		BlendingBoneAnimation(&m_pCurrentAnimation->Get_RootNode(), &m_pPreAnimation->Get_RootNode(), XMMatrixIdentity() * XMLoadFloat4x4(&m_PreTransform), fBlendTime);
		if (fBlendTime >= 1.f)
		{

			m_iPreAnimation = UINT_MAX;
		
			//m_fPreCurrentTime = 0.f;
			//m_fPreNoLoopTime = 0.f;
			m_bIsBlending = false;


			
			CalculateBoneAnimation(&m_pCurrentAnimation->Get_RootNode(), XMMatrixIdentity() * XMLoadFloat4x4(&m_PreTransform));
		}
	}
	else
	{
		if (m_pCurrentAnimation)
		{
		
			if (m_bNoRoot)
			{
				m_fNoLoopTime += m_pCurrentAnimation->Get_Tick(m_iAnimationNumber) * fTimeDelta * m_fDoubleSpeed;
				m_fCurrentTime += m_pCurrentAnimation->Get_Tick(m_iAnimationNumber) * fTimeDelta * m_fDoubleSpeed;

			}
			else
			{
				m_fLerpTick += fTimeDelta;
				_float t = min(1.f,m_fLerpTick / 1.4f);
				m_fNoLoopTime = 0 + (m_pCurrentAnimation->Get_Duration(m_iAnimationNumber) - 0) * t;
				m_fCurrentTime = 0 + (m_pCurrentAnimation->Get_Duration(m_iAnimationNumber) - 0) * t;
			}
			

 			if (!m_bLoop)
			{
				if (!m_bFinished && m_fNoLoopTime >= m_pCurrentAnimation->Get_Duration(m_iAnimationNumber))
				{
					m_bFinished = true; //한번
					m_bForce = false;
					m_fCurrentTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);
				}
				else if (m_bFinished)
				{
					
					m_fNoLoopTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);
					m_fCurrentTime = m_pCurrentAnimation->Get_Duration(m_iAnimationNumber);
					
				}
			}
			else
			{
				m_fCurrentTime = fmod(m_fCurrentTime, m_pCurrentAnimation->Get_Duration(m_iAnimationNumber));

			}
			
			
			CalculateBoneAnimation(&m_pCurrentAnimation->Get_RootNode(), XMMatrixIdentity() * XMLoadFloat4x4(&m_PreTransform));
		}
	}

}


vector<string>& CAnimator::Get_NameList()
{
	return m_pCurrentAnimation.get()->Get_NameList();
}
_float4x4* CAnimator::Find_OriginBone(const string& Name)
{
	auto Index = m_BoneList.find(Name);

	if (Index == m_BoneList.end())
		return nullptr;

	for (size_t i = 0; i < m_pCurrentAnimation->Get_BoneInfo().size(); ++i)
	{
		if (m_pCurrentAnimation->Get_BoneInfo()[i].index == Index->second)
			return &m_pCurrentAnimation->Get_BoneInfo()[i].matBone;
		

	}
	
	return nullptr;
}
void CAnimator::Player_Animation(unique_ptr<CAnimation> pAin)
{
	m_pCurrentAnimation = std::move(pAin);
	m_fCurrentTime = 0.f;
}
void CAnimator::Bind_Resource_BoneMatrix(CShader* pShader, const _char* constName)
{
	pShader->Bind_Matrix_Array(constName, m_FinalBoneMatrices.data(), m_FinalBoneMatrices.size());
}
void CAnimator::CalculateBoneAnimation(const AssimpNodeData* node, FXMMATRIX parentsTrans)
{
	//JNT_R_Grabpack_Tube_01
	int32_t index = node->index;
	CBone* Bone = nullptr;
	if(index != -1)
		Bone = m_pCurrentAnimation->Find_Bone(index,m_iAnimationNumber);
	XMMATRIX nodeTransform = XMLoadFloat4x4(&node->transformation);
	
	if (Bone)
	{
		//if(m_bBeforeAnime)
		//	Bone->

		XMMATRIX nodeAnimation = Bone->Bone_Update(m_fCurrentTime);
		nodeTransform = nodeAnimation;
	}
	
	XMMATRIX globalTransform;
	
	globalTransform =  nodeTransform * parentsTrans ;
	if (index == -1 && !m_bNoRoot)
		globalTransform.r[3] = XMVectorSet(0, 0, 0, 1);


	auto& mesh = m_pCurrentAnimation->Get_BoneInfo();

	if (index < mesh.size())
	{
		int32_t idex = mesh[index].index;
		_matrix offset = XMLoadFloat4x4(&mesh[index].matBone);

		XMStoreFloat4x4(&m_FinalBoneMatrices[index], offset * globalTransform);
		m_beforeOffsetMatrix[index] = m_FinalBoneMatrices[index];

	}
	
	for (uint32_t i = 0; i < node->iChildrenCount; ++i)
		CalculateBoneAnimation(&node->vecChildern[i], globalTransform);

}

void CAnimator::BlendingBoneAnimation(const AssimpNodeData* Currentnode, const AssimpNodeData* PreNode, FXMMATRIX PreMatrix, const _float& fBlendTime)
{
	//JNT_R_Grabpack_Tube_0
	if (nullptr == Currentnode || nullptr == PreNode)
		return;
	uint32_t Curindex = Currentnode->index;
	CBone* CurBone = nullptr;
	CBone* PreBone = nullptr;
	if (Curindex != UINT32_MAX)
		CurBone = m_pCurrentAnimation->Find_Bone(Curindex, m_iAnimationNumber);

	const AssimpNodeData* pPreNode = Find_Node_By_Name(PreNode, Currentnode->index);

	uint32_t Preindex = pPreNode->index;
	if (Preindex != UINT32_MAX)
		PreBone = m_pPreAnimation->Find_Bone(Preindex, m_iPreAnimation);
	BONE_BLEND CurBoneBlend{};
	BONE_BLEND PreBoneBlend{};

	_matrix	  FinalLerpMatrix = XMLoadFloat4x4(&Currentnode->transformation);
	if (CurBone)
	{
		CurBoneBlend = CurBone->Bone_Update_Blend(m_fCurrentTime);

		FinalLerpMatrix = XMMatrixScalingFromVector(CurBoneBlend.vScale) * XMMatrixRotationQuaternion(CurBoneBlend.vRot) * XMMatrixTranslationFromVector(CurBoneBlend.vPos);
	}
		
	if (PreBone)
	{
		PreBoneBlend = PreBone->Bone_Update_Blend(m_fPreCurrentTime);

		FinalLerpMatrix = XMMatrixScalingFromVector(PreBoneBlend.vScale) * XMMatrixRotationQuaternion(PreBoneBlend.vRot) * XMMatrixTranslationFromVector(PreBoneBlend.vPos);
	}
	
	if (CurBone && PreBone)
	{
		_vector vFinalLerpPos = XMVectorLerp(PreBoneBlend.vPos, CurBoneBlend.vPos, fBlendTime);
		_vector vFinalLerpRot = XMQuaternionSlerp(PreBoneBlend.vRot, CurBoneBlend.vRot, fBlendTime);
		_vector vFinalLerpScale = XMVectorLerp(PreBoneBlend.vScale, CurBoneBlend.vScale, fBlendTime);

		
		FinalLerpMatrix = XMMatrixScalingFromVector(vFinalLerpScale)* XMMatrixRotationQuaternion(vFinalLerpRot) * XMMatrixTranslationFromVector(vFinalLerpPos);
	}
	XMMATRIX globalTransform;

	globalTransform =  FinalLerpMatrix * PreMatrix;


	if (Curindex == 0 && !m_bNoRoot)
		globalTransform.r[3] = XMVectorSet(0, 0, 0, 1);

	auto& mesh = m_pCurrentAnimation->Get_BoneInfo();

	if (Curindex < mesh.size())
	{
		uint32_t idex = mesh[Curindex].index;
		_matrix offset = XMLoadFloat4x4(&mesh[Curindex].matBone);

		XMStoreFloat4x4(&m_FinalBoneMatrices[Curindex], offset * globalTransform);
		m_beforeOffsetMatrix[Curindex] = m_FinalBoneMatrices[Curindex];

	}

	for (uint32_t i = 0; i < Currentnode->iChildrenCount; ++i)
		BlendingBoneAnimation(&Currentnode->vecChildern[i], PreNode, globalTransform,fBlendTime);
}

void CAnimator::CalculateFinalBoneMatrices()
{
	auto Boeninfo = m_pCurrentAnimation->Get_BoneInfo();
	for (size_t i =0 ;i < Boeninfo.size(); ++i)
	{
		auto matrix = Boeninfo[i].matBone;
		auto index = Boeninfo[i].index;
		
		_matrix offset = XMLoadFloat4x4(&matrix);

		XMStoreFloat4x4(&m_FinalBoneMatrices[index], offset * XMLoadFloat4x4(&m_GlobalBoneMatrices[index]));
		m_beforeOffsetMatrix[i] = m_FinalBoneMatrices[index];
	}


}
const AssimpNodeData* CAnimator::Find_Node_By_Name(const AssimpNodeData* pNode, const uint32_t& index)
{
	if (pNode == nullptr) return nullptr;
	if (pNode->index == index) return pNode; // 프로젝트의 이름 변수명(szName 등)에 맞추세요

	for (uint32_t i = 0; i < pNode->iChildrenCount; ++i)
	{
		auto pFound = Find_Node_By_Name(&pNode->vecChildern[i], index);
		if (pFound) return pFound;
	}
	return nullptr;
}
void CAnimator::Change_Animation(const string& name)
{
	uint32_t i = 0;
	for (auto& iter : m_pCurrentAnimation->Get_NameList())
	{
		if (name == iter)
		{
			if (m_iAnimationNumber != i)
			{
				m_iPreAnimation = m_iAnimationNumber;

				m_fPreCurrentTime = m_fCurrentTime;
				m_fPreNoLoopTime = m_fNoLoopTime;

				m_iAnimationNumber = i;

				m_fCurrentTime = 0.f;
				m_fNoLoopTime = 0.f;

				m_fBlendElapsed = 0.f;

				m_bIsBlending = true;
			}
			return;
		}
			++i;
	}
}

_bool CAnimator::Change_Animation_Enum(uint32_t iNumber, _bool bLoop,_bool bForce, _bool Blend)
{
	if (m_bForce)
		return false;


	if (m_iAnimationNumber != iNumber)
	{
		m_iPreAnimation = m_iAnimationNumber;

		m_fPreCurrentTime = m_fCurrentTime;

		m_iAnimationNumber = iNumber;

		m_fCurrentTime = 0.f;
		m_fNoLoopTime = 0.f;

		m_fBlendElapsed = 0.f;

		m_bIsBlending = Blend;
	}
	else
	{
		if (!m_bLoop)
		{

			if (m_bFinished)
			{
			}
			else
			{
				return true;
			}
		}
		else
		{
			m_fCurrentTime = 0.f;
			m_fNoLoopTime = 0.f;

		}
	}

	if (bForce)
		m_bForce = true;

	m_bLoop = bLoop;

	m_bFinished = false;

	return true;
}

void CAnimator::Change_Final_BoneMatices(const uint32_t str, _float4x4 boneMatrix)
{

	auto Index = m_GlobalBoneMap.find(str);

	if (Index != m_GlobalBoneMap.end())
	{
		m_GlobalBoneMatrices[Index->second] = boneMatrix;
	
	}

}

_matrix CAnimator::Find_ChangeBone(uint32_t index)
{
	auto Index = m_GlobalBoneMap.find(index);

	if (Index != m_GlobalBoneMap.end())
	{
		return XMLoadFloat4x4(&m_GlobalBoneMatrices[Index->second]);

	}

	return XMMatrixIdentity();
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
