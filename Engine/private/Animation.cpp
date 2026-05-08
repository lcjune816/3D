#include "Animation.h"

CAnimation::CAnimation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{

}
CAnimation::CAnimation(const CAnimation& Prototype) :CComponent(Prototype){}

CAnimation::~CAnimation() {}

HRESULT CAnimation::Initialize(const string& animationPath, map<string, Bone>& mesh, uint32_t& boneCount)
{
	return S_OK;
}

 HRESULT CAnimation::Initialize_Prototype()
{

	 return S_OK;
}
HRESULT	CAnimation::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;
	auto Desc = static_cast<ANIMATION_DESC*>(pArg);
	
	m_AniTime = move(Desc->AniTime);
	m_AnimantionName = move(Desc->AnimantionName);
	m_Bones    = move(Desc->Bones);
	m_BoneInfo = move(Desc->BoneInfo);
	m_RootNode = move(Desc->RootNode);

	return S_OK;
}


CBone* CAnimation::Find_Bone(const uint32_t index , uint32_t i)

{
	auto iter = find_if(m_Bones[i].begin(), m_Bones[i].end(),
		[&](const shared_ptr<CBone>& Bone)
		{
			return Bone.get()->Get_Index() == index;
		}
	);

	if (iter == m_Bones[i].end())
		return nullptr;
	else return (iter)->get();
}



unique_ptr<CAnimation> CAnimation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CAnimation>(new CAnimation(pDevice,pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed Animation");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<CPrototype> CAnimation::Clone(void* pArg)
{

	auto pInstance = shared_ptr<CAnimation>(new CAnimation(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("3DMdoel Create Failed");
		return nullptr;
	}

	return pInstance;
}