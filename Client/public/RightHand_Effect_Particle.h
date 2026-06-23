#pragma once
#include "FSM_RightHand.h"

NS_BEGIN(Engine)
class CVIBuffer_Particle_Point;
class CShader;
class CMeshNonAnime;
NS_END

enum class TEXTUREID {DIFFUSE, NOISE, END};
NS_BEGIN(Client)
class CRightHand_Effect_Particle :public CPLayer_RightHand
{
public:
	typedef struct strRightHandEffect : public CPLayer_RightHand::RIGHT_HAND_DESC
	{
		RIGHTEFFECT eRightEffectType;
		_float3		vOffsetScale, vOffsetPos;

		uint32_t* piFlag;
	}RHAND_PARTICLE_DESC;
private:
	CRightHand_Effect_Particle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CRightHand_Effect_Particle(const CRightHand_Effect_Particle& Prototype);
public:
	virtual ~CRightHand_Effect_Particle();


	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render()override;

	virtual HRESULT			Render_Bloom() override;
private:
	shared_ptr<Engine::CVIBuffer_Particle_Point>		m_pVIBuffer{ nullptr };
	RIGHTEFFECT											m_eType{};

	uint32_t* m_piFlag;
	uint32_t											m_iPatternCnt{}, m_iArrayCnt{};

	_float4x4*											m_pParentMatirx{};
	_float4x4											m_OffsetMatrix{};
	_float4x4											m_FinalMatrix{};

	_float												m_fTime{}, m_fTimeTick{}, m_fOffset{};
	int32_t												m_iTextureId[ETOUI(TEXTUREID::END)];

	array<_bool, 8>									m_iElectricPattern;
public:
	static		unique_ptr<CRightHand_Effect_Particle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual		shared_ptr<CPrototype>		Clone(void* pArg) override;


};

NS_END
