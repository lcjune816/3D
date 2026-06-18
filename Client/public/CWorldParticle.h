#pragma once
#include "Client_Defines.h"
#include "ParticleObject.h"
#include "Observer.h"
NS_BEGIN(Engine)
class CShader;
class CCube;
class CVIBuffer_Particle_Point;
NS_END

enum class  PATHNAME{SHADER,BUFFER,TEXTURE,END};
NS_BEGIN(Client)
class CWorldParticle : public CParticleObject, public CObserver
{
public:
	typedef struct tagWorldParticle : public CParticleObject::PARTICLEOBJECT_DESC
	{
		_tchar	PathName[ETOUI(PATHNAME::END)][256]{};
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
	virtual HRESULT Bind_Resource(const _char* pConstantName, shared_ptr<class CShader> pShader) override;
	virtual void OnNotify(const EVENT& eEvent) override;
private:
	HRESULT			Ready_Component();
	void			Fog_Controller(const _float& fTimeDelta);
private:
	shared_ptr<Engine::CCube>					 m_pBoxMesh = { nullptr };
	shared_ptr<Engine::CVIBuffer_Particle_Point> m_pVIBufferCom = { nullptr };
	shared_ptr<Engine::CShader>					 m_pShaderCom = { nullptr };

private:
	int32_t										m_iTextureID = { -1 };
	uint32_t									m_iLevelIndex = { ETOUI(LEVEL::END) };
	PARTICLE									m_eParticleEmitType = { PARTICLE::END };
	PARTICLE									m_eParticleEmitTypeOrigin = { PARTICLE::END };

	_wstring									m_strPathName[ETOUI(PATHNAME::END)] = {};

	 uint32_t									m_iPassIndex;
	 uint32_t									m_iTickCnt{};
	 _bool										m_bStart{ false };
	 _float									    m_fTick{ 0 };
	 _float4									m_vFog{};
	 _float										m_fFogDistance{ 10000 };
public:
	static unique_ptr<CWorldParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;


};

NS_END

