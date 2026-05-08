#include "Bone.h"
#include "GameInstance.h"

CBone::CBone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext): CComponent(pDevice,pContext){}
CBone::CBone(const CBone& Prototype): CComponent(Prototype){}
CBone::~CBone()
{

}

HRESULT CBone::Initialize(void* pArg)
{
	auto Desc = static_cast<BONE_DESC*>(pArg);
	
	if (Desc->check != true)
	{
		m_pBoneAnimation = move(Desc->Key);
		m_index = Desc->index;
		m_iNumPosition = Desc->iNumPosition;
		m_iNumRotation = Desc->iNumRotation;
		m_iNumScale = Desc->iNumScale;
		return S_OK;
	}

	if (nullptr != Desc)
		SetAnimation(Desc->index, Desc->ID, Desc->pChannel);
	else return E_FAIL;

	return S_OK;
}
HRESULT CBone::Initialize_Prototype()
{

	return S_OK;
}
void CBone::SetAnimation(const int32_t& index, uint32_t ID, const aiNodeAnim* pChannel)
{
	m_index = index;
	m_ID = ID;
	m_iNumPosition = pChannel->mNumPositionKeys;
	
	for (uint32_t i = 0; i < m_iNumPosition; ++i)
	{
		aiVector3D aiPosition = pChannel->mPositionKeys[i].mValue;
		_float	   fTimeStamp = pChannel->mPositionKeys[i].mTime;
		KeyPosition data{};
		data.fPos = { aiPosition.x,aiPosition.y,aiPosition.z };
		data.fTimeStamp = fTimeStamp;
		m_pBoneAnimation.vecPos.push_back(data);

	}
	m_iNumRotation = pChannel->mNumRotationKeys;
	for (uint32_t i = 0; i < m_iNumRotation; ++i)
	{
		aiQuaternion aiOrigin = pChannel->mRotationKeys[i].mValue;
		_float		 fTimeStamp = pChannel->mRotationKeys[i].mTime;
		KeyRotation data{};
		data.orientation = { aiOrigin.x,aiOrigin.y,aiOrigin.z, aiOrigin.w };
		data.fTimeStamp = fTimeStamp;
		m_pBoneAnimation.vecRot.push_back(data);
	}
	m_iNumScale = pChannel->mNumScalingKeys;
	for (uint32_t i = 0; i < m_iNumScale; ++i)
	{
		aiVector3D scale = pChannel->mScalingKeys[i].mValue;
		_float	   fTimeStamp = pChannel->mScalingKeys[i].mTime;
		KeyScale	data{};
		data.fScale = { scale .x,scale .y,scale .z};
		data.fTimeStamp = fTimeStamp;
		m_pBoneAnimation.vecScale.push_back(data);
	}
	Save_Binary_Data_SizeT(m_iNumPosition, CGameInstance::Get().Get_Binary_Path());
	Save_Binary_Data_SizeT(m_iNumRotation, CGameInstance::Get().Get_Binary_Path());
	Save_Binary_Data_SizeT(m_iNumScale, CGameInstance::Get().Get_Binary_Path());
	m_pBoneAnimation.Save_Data(CGameInstance::Get().Get_Binary_Path());
}

uint32_t CBone::Get_PositionIndex(_float fTimeDelta)
{

	for (uint32_t i = 0; i < m_iNumPosition - 1; ++i)
	{
		
		if (fTimeDelta < m_pBoneAnimation.vecPos[i + 1].fTimeStamp)
			return i;
		
	}

	return m_iNumPosition -2;
}
uint32_t CBone::Get_RotationIndex(_float fTimeDelta)
{
	
	for (uint32_t i = 0; i < m_iNumRotation-1 ; ++i)
	{
		if (fTimeDelta < m_pBoneAnimation.vecRot[i + 1].fTimeStamp)
			return i;
		
	}

	return m_iNumRotation-2;
}
uint32_t CBone::Get_ScaleIndex(_float fTImeDelta)
{
	
	for (uint32_t i = 0; i < m_iNumScale -1; ++i)
	{
		if (fTImeDelta < m_pBoneAnimation.vecScale[i + 1].fTimeStamp)
			return i;
		
	}


	return m_iNumScale-2;
}

_float CBone::Get_ScaleFactor(_float lastTimeStamp, _float nextTimeStamp, _float fTimeDelta)
{
	_float scaleFactor = 0.f;
	_float midWayLength = fTimeDelta - lastTimeStamp;
	_float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;

	return scaleFactor;
}

XMMATRIX CBone::InterpolatePosition(_float fTimeDelta)
{
	if (1 == m_iNumPosition)
	{
		return XMMatrixTranslationFromVector(XMLoadFloat3(&m_pBoneAnimation.vecPos[0].fPos));

	}
	uint32_t p0Index = Get_PositionIndex(fTimeDelta);
	uint32_t p1Index = p0Index + 1;
	_float	scaleFactor = Get_ScaleFactor(m_pBoneAnimation.vecPos[p0Index].fTimeStamp,
										  m_pBoneAnimation.vecPos[p1Index].fTimeStamp, fTimeDelta);

	 XMVECTOR vPos = XMVectorLerp(XMLoadFloat3(&m_pBoneAnimation.vecPos[p0Index].fPos),
								  XMLoadFloat3(&m_pBoneAnimation.vecPos[p1Index].fPos), scaleFactor);
	 return  XMMatrixTranslationFromVector(vPos);
}

XMMATRIX CBone::InterpolateRotation(_float fTimeDelta)
{
	if (1 == m_iNumRotation)
	{
		return XMMatrixRotationQuaternion(XMQuaternionNormalize(XMLoadFloat4(&m_pBoneAnimation.vecRot[0].orientation)));
	}
	
	uint32_t p0Index = Get_RotationIndex(fTimeDelta);
	uint32_t p1Index = p0Index + 1;

	_float fScaleFactor = Get_ScaleFactor(m_pBoneAnimation.vecRot[p0Index].fTimeStamp,
										  m_pBoneAnimation.vecRot[p1Index].fTimeStamp, fTimeDelta);
	
	XMVECTOR vRot = XMQuaternionSlerp(XMLoadFloat4(&m_pBoneAnimation.vecRot[p0Index].orientation),
									  XMLoadFloat4(&m_pBoneAnimation.vecRot[p1Index].orientation), fScaleFactor);

	return XMMatrixRotationQuaternion(XMQuaternionNormalize(vRot));
}

XMMATRIX CBone::InterpolateScale(_float fTimeDelta)
{
	if (1 == m_iNumScale)
	{
		return XMMatrixScalingFromVector(XMLoadFloat3(&m_pBoneAnimation.vecScale[0].fScale));
	}

	uint32_t p0Index = Get_ScaleIndex(fTimeDelta);
	uint32_t p1Index = p0Index + 1;
	_float fScaleFactor = Get_ScaleFactor(m_pBoneAnimation.vecScale[p0Index].fTimeStamp,
										  m_pBoneAnimation.vecScale[p1Index].fTimeStamp, fTimeDelta);

	XMVECTOR vScale = XMVectorLerp(XMLoadFloat3(&m_pBoneAnimation.vecScale[p0Index].fScale),
						      	   XMLoadFloat3(&m_pBoneAnimation.vecScale[p1Index].fScale), fScaleFactor);
	return  XMMatrixScalingFromVector(vScale);
}

XMMATRIX  CBone::Bone_Update(_float fTimeDelta)
{
	XMMATRIX mat = XMMatrixIdentity();
	XMMATRIX Trans =  InterpolatePosition(fTimeDelta);
	XMMATRIX Rot   =  InterpolateRotation(fTimeDelta);
	XMMATRIX Scale =  InterpolateScale(fTimeDelta);
	mat = Scale * Rot * Trans;
	
	return mat;
}

unique_ptr<CBone> CBone::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CBone>(new CBone(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed Bone");
		return nullptr;
	}
		
	return pInstance;
}

shared_ptr<CPrototype> CBone::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CBone>(new CBone(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed Bone_Clone");
		return nullptr;
	}

	return pInstance;
}
