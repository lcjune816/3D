#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CTransform :  public CComponent
{
public:
	typedef struct tagTransformdesc
	{
		_float		m_fSpeedPerSec;
		_float		m_fRotationPerSec;
		
		_bool		bWorldCheck = true;

		_float4x4	matWorld;
	}TRANSFORM_DESC;
private:
	CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CTransform();

public:
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ETOUI(eState)][0]));
	}

	_matrix Get_World()const { return XMLoadFloat4x4(&m_WorldMatrix); }
	_float4x4* Get_WorldPtr() { return &m_WorldMatrix; }
	_float3 Get_Scaled();

	_float3									Get_Max() { return m_fMax; }
	_float3									Get_Min() { return m_fMin; }

	void									Set_MaxMin(_float3 fMin, _float3 fMax) { m_fMin = fMin; m_fMax = fMax; }
	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOUI(eState)][0]), vState);
	}

	HRESULT		Bind_Matrix(shared_ptr<class CShader> pShader, const char* pBindName);
	void		CombinedMatrix(const _float4x4* pMatrix)	
	{
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(pMatrix));
	}
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_BackWard(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta, _float m_fSpeed);
	void Set_Matrix(_fmatrix matrix);
	void Apply_Rotation(_fvector vAxis, _float fAngle);

	void Rotation(_float fAngleX, _float fAngleY, _float fAngleZ);
	void Rotation(_fvector vAxis, _float fAngle);
	
	void Turn(_fvector vAxis, _float fTimeDelta);
	
	void LookAt(_fvector vAt);
	void Chase_Target(const _float& fTimeDelta);
	void Velocity_Speed(_float fSpeed) { m_fSpeedPerSec = m_fOriginSpeed + fSpeed; }
private:

private:
	_float							m_fOriginSpeed;
	_float							m_fSpeedPerSec = {};
	_float							m_fRotationPerSec = { };
	_float3							m_fMin = {};
	_float3							m_fMax = {};


	_float4x4						m_WorldMatrix = {};
public:
    static shared_ptr<CTransform> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* Arg) override;

};

NS_END