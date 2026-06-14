#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Point  final : public CVIBuffer_Instance
{
public:
	typedef struct tagParticleInstanceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float2 vSpeed = {};
		_float2 vLifeTime = {};
		_bool	isLoop = { false };
		_float2 vGrid = {};
		_float3 vPivot = {};

	}PARTICLE_INSTANCE_DESC;
protected:
	CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CVIBuffer_Particle_Point();

public:
	virtual HRESULT	 Initialize_Prototype(void* pArg)override;
	virtual HRESULT  Initialize(void* pArg)override;
	virtual HRESULT  Bind_Resource() override;
	virtual HRESULT  Render()		override;

public:
	void Fog_Spread(const _float& fTimeDelta);
	void Spark(const _float& fTimeDelta);
	_bool Steam(const _float& fTimeDelta);
private:
	shared_ptr<VTXINSTANCE_PARTICLE[]>			m_pInstanceData = { nullptr };
	shared_ptr<_float[]>						m_pSpeeds = { nullptr };
	_bool										m_isLoop = { false };
	_float3										m_vPivot = {};
	_float2										m_vGrid = {};
	D3D11_BUFFER_DESC							m_InstanceBufferDesc = {};

	

public:
	static unique_ptr<CVIBuffer_Particle_Point> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};


NS_END
