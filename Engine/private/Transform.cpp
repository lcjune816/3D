#include "Transform.h"
#include "GameInstance.h"
CTransform::CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{
}


CTransform::~CTransform()
{
	int32_t i = 0;
}
_float3 CTransform::Get_Scaled()
{
	return _float3( XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
				    XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
				    XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))));
}
HRESULT CTransform::Bind_Matrix(shared_ptr<class CShader> pShader, const char* pBindName)
{
	return pShader->Bind_Matrix(pBindName, &m_WorldMatrix);
}
HRESULT CTransform::Initialize_Prototype()
{

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	return S_OK;
}
HRESULT CTransform::Initialize(void* Arg)
{
	auto Desc = static_cast<TRANSFORM_DESC*>(Arg);
	m_fOriginSpeed = m_fSpeedPerSec	  = Desc->m_fSpeedPerSec;
	m_fRotationPerSec = Desc->m_fRotationPerSec;

	if(Desc->bWorldCheck)
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	else
	{
		_vector Pos{};

		memcpy(&Pos, Desc->matWorld.m[3], sizeof _float3);
		Pos = XMVectorSetW(Pos, 1.f);

		memcpy(Desc->matWorld.m[3] ,&Pos, sizeof _float4);
		m_WorldMatrix = Desc->matWorld;

	}
	
	return S_OK;
}
void CTransform::Set_Matrix(_fmatrix matrix)
{
	XMStoreFloat4x4(&m_WorldMatrix ,matrix);
}
void CTransform::Go_Straight(_float fTimeDelta, shared_ptr<CNavigation> pNavigation)
{
	_vector		vPos = Get_State(STATE::POS);
	_vector		vLook = Get_State(STATE::LOOK);

		vPos += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation ||
		true == pNavigation->InMove(vPos))
		Set_State(STATE::POS, vPos);
}

void CTransform::Go_BackWard(_float fTimeDelta, shared_ptr<CNavigation> pNavigation)
{

	_vector		vPos = Get_State(STATE::POS);
	_vector		vLook = Get_State(STATE::LOOK);

	vPos -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;


	if (nullptr == pNavigation ||
		true == pNavigation->InMove(vPos))
		Set_State(STATE::POS, vPos);
}

void CTransform::Go_Left(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation)
{
	_vector		vPos = Get_State(STATE::POS);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPos -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;


	if (nullptr == pNavigation ||
		true == pNavigation->InMove(vPos))
		Set_State(STATE::POS, vPos);
}

void CTransform::Go_Right(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation)
{
	_vector		vPos = Get_State(STATE::POS);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPos += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;


	if (nullptr == pNavigation ||
		true == pNavigation->InMove(vPos))
		Set_State(STATE::POS, vPos);
}

void CTransform::Go_Up(_float fTimeDelta,_float m_fSpeed)
{
	_vector		vPos = Get_State(STATE::POS);
	_vector		vUp = Get_State(STATE::UP);

	vPos += XMVector3Normalize(vUp) * m_fSpeed * fTimeDelta;

	Set_State(STATE::POS, vPos);

}

void CTransform::Apply_Rotation(_fvector vAxis, _float fAngle)
{

	_float3		vScaled = Get_Scaled();
	   

	_vector		vRight = XMVector3Normalize(Get_State(STATE::RIGHT))* vScaled.x;
	_vector		vUp    = XMVector3Normalize(Get_State(STATE::UP))   * vScaled.y;
	_vector		vLook  = XMVector3Normalize(Get_State(STATE::LOOK)) * vScaled.z;


	_matrix		RotMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight,RotMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp,RotMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook,RotMatrix));
}

void CTransform::MoveToAstar(shared_ptr<class CNavigation> pNavi, const uint32_t endLayerIndex, const _wstring& LayerName, const _char* tagName, const _float& fTimeDelta)
{
	_float3 vLookPos{};
	if (pNavi->AStartAlgorithm(endLayerIndex, LayerName, tagName, Get_State(STATE::POS)))
	{

		Set_State(STATE::POS, pNavi->MoveToAstar(Get_State(STATE::POS), m_fSpeedPerSec, fTimeDelta, &vLookPos));

		LookAt(XMLoadFloat3(&vLookPos));
	}
}

void CTransform::Rotation(_float fAngleX, _float fAngleY, _float fAngleZ)
{
	_float3 vScale = Get_Scaled();

	_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatirx = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw
	(XMConvertToRadians(fAngleX), XMConvertToRadians(fAngleY), XMConvertToRadians(fAngleZ)));

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatirx));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatirx));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatirx));
}

void CTransform::Rotation(_fvector vAxis, _float fAngle)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.0f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp    = XMVectorSet(0.0f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook  = XMVectorSet(0.0f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotMatrix));

}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{

	_float3     vScaled = Get_Scaled();

	_vector     vRight = Get_State(STATE::RIGHT);
	_vector     vUp = Get_State(STATE::UP);
	_vector     vLook = Get_State(STATE::LOOK);

	_matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(m_fRotationPerSec) * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	if(XMVectorGetX(XMVector3Length(vAt - Get_State(STATE::POS))) < 3.f)
		return;

	_vector		vLook = XMVectorSetW(XMVector3Normalize(vAt - Get_State(STATE::POS)),0.f);
	
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::Chase_Target(const _float& fTimeDelta)
{
	_vector vLook = Get_State(STATE::LOOK);
	_vector vPos = Get_State(STATE::POS);

	vPos += vLook * fTimeDelta* m_fSpeedPerSec;
	
	Set_State(STATE::POS, vPos);
}

shared_ptr<CTransform> CTransform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = shared_ptr<CTransform>(new CTransform(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed Transform");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CTransform::Clone(void* Arg)
{
	return nullptr;
}
