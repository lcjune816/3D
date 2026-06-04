#pragma once
#include "Client_Defines.h"
#include "ParticleObject.h"

NS_BEGIN(Engine)
class CShader;
class CCube;
class CVIBuffer_Particle_Point;
NS_END

NS_BEGIN(Client)
class CWorldParticle : public CParticleObject
{
public:
	typedef struct tagWorldParticle : public CParticleObject::PARTICLEOBJECT_DESC
	{
		uint32_t	iLevelIndex;
		PARTICLE	eParticleEmit;
	}WORLDPARTICLE_DESC;
private:
	CWorldParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWorldParticle(const CWorldParticle& Prototye);
public:
	virtual ~CWorldParticle();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Load_Data(void* pDesc, const json& j);
	virtual json	Save_Data();

private:
	HRESULT			Ready_Component();

private:
	shared_ptr<Engine::CCube>					 m_pBoxMesh = { nullptr };
	shared_ptr<Engine::CVIBuffer_Particle_Point> m_pVIBufferCom = { nullptr };
	shared_ptr<Engine::CShader>					 m_pShaderCom = { nullptr };

private:
	int32_t										m_iTextureID = { -1 };
	uint32_t									m_iLevelIndex = { ETOUI(LEVEL::END) };
	PARTICLE									m_eParticleEmitType = { PARTICLE::END };
public:
	static unique_ptr<CWorldParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;


};

NS_END

