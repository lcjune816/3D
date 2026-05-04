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
		string name;
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
	void						SetAnimation(const string& name, uint32_t ID, const aiNodeAnim* pChannel);
	

private:
	uint32_t						Get_PositionIndex(_float fTimeDelta);
	uint32_t						Get_RotationIndex(_float fTimeDelta);
	uint32_t						Get_ScaleIndex(_float fTImeDelta);

	_float						Get_ScaleFactor(_float lastTimeStamp, _float nextTimeStamp,_float fTimeDelta);
	
	XMMATRIX					InterpolatePosition(_float fTimeDelta);
	XMMATRIX					InterpolateRotation(_float fTimeDelta);
	XMMATRIX					InterpolateScale(_float fTimeDelta);

public:
	string						Get_Name()		{ return m_strName; }
	uint32_t					Get_BoneID()	{ return m_ID; }


public:
	XMMATRIX					Bone_Update(_float fTimeDelta);

private:
	KEYANI				m_pBoneAnimation = {};
	uint32_t				m_iNumPosition = {0};
	uint32_t				m_iNumRotation = {0};
	uint32_t				m_iNumScale = {0};
	uint32_t				m_ID;

	_float4x4			m_matWorld = {};
	string				m_strName  = {};
public:
	static unique_ptr<CBone>Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};

NS_END

