#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraNS abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vEye, vAt;
		_float		fFovy, fNear, fFar;
 }CAMERA_DESC;

protected:
	CCameraNS(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCameraNS(const CCameraNS& Prototype);
public:
	virtual ~CCameraNS();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float			m_fFovy{}, m_fNear{}, m_fFar{};

protected:

	void Update_PipeLine();

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

NS_END

