#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
NS_BEGIN(Engine)
class CVIBuffer_Particle_Point;
class CShader;
NS_END

NS_BEGIN(Client)
class CCat_Fog final : public CGameObject
{
public:
	typedef struct tagCatFogDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4*			pParentMatrix;
	}CATFOG_DESC;
protected:
	CCat_Fog(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCat_Fog(const CCat_Fog& pPrototype);
public:
	virtual ~CCat_Fog();

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render()override;

private:
	HRESULT					Ready_Component();
	void					CombinedMatrix();
private:
	shared_ptr<CShader>							m_pShaderCom{ nullptr };
	shared_ptr<CVIBuffer_Particle_Point>		m_pVIBufferCom{ nullptr };

	_float4x4*									m_pParentMatrix = { nullptr };
	_float4x4									m_CombinedMatrix = {};
	uint32_t									m_iTextureID;
public:
	static		unique_ptr<CCat_Fog> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual		shared_ptr<CPrototype> Clone(void* pArg)override;

};

NS_END