#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CBone : public CComponent
{
public:
	typedef struct strbonedesc
	{
		_bool  check = true;
		KEYANI Key;
		int32_t index;
		uint32_t ID;
		uint32_t	iNumPosition;
		uint32_t	iNumRotation;
		uint32_t	iNumScale;
		aiNodeAnim* pChannel;
	
	}BONE_DESC;

private:
	CBone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBone(const CBone& Prototype);
public:
	virtual ~CBone();

private:
	virtual HRESULT				Initialize(void* pArg);
	virtual HRESULT				Initialize_Prototype();
	void						SetAnimation(const int32_t& index, uint32_t ID, const aiNodeAnim* pChannel);
	

private:
	uint32_t						Get_PositionIndex(_float fTimeDelta);
	uint32_t						Get_RotationIndex(_float fTimeDelta);
	uint32_t						Get_ScaleIndex(_float fTImeDelta);

	_float						Get_ScaleFactor(_float lastTimeStamp, _float nextTimeStamp,_float fTimeDelta);
	
	_vector						Blend_Pos(const _float& fTimeDelta);
	_vector						Blend_Rot(const _float& fTimeDelta);
	_vector						Blend_Scale(const _float& fTimeDelta);

	XMMATRIX					InterpolatePosition(_float fTimeDelta);
	XMMATRIX					InterpolateRotation(_float fTimeDelta);
	XMMATRIX					InterpolateScale(_float fTimeDelta);

public:
	uint32_t					Get_Index()		{ return m_index; }
	uint32_t					Get_BoneID()	{ return m_ID; }


public:
	BONE_BLEND					Bone_Update_Blend(_float fTimeDelta);
	XMMATRIX					Bone_Update(_float fTimeDelta);

private:
	KEYANI				m_pBoneAnimation = {};
	uint32_t				m_iNumPosition = {0};
	uint32_t				m_iNumRotation = {0};
	uint32_t				m_iNumScale = {0};
	uint32_t				m_ID;

	_float4x4			m_matWorld = {};
	int32_t				m_index  = {};
	int32_t				m_iLastIndex = {};
public:
	static unique_ptr<CBone>Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};

NS_END

