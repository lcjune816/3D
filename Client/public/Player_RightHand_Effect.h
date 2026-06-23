#pragma once
#include "FSM_RightHand.h"

NS_BEGIN(Engine)
class CVIBuffer_Particle_Point;
class CShader;
class CMeshNonAnime;
NS_END

NS_BEGIN(Client)
class CPlayer_RightHand_Effect :public CPLayer_RightHand
{
public: 
	typedef struct strRightHandEffect : public CPLayer_RightHand::RIGHT_HAND_DESC
	{
		RIGHTEFFECT eRightEffectType;
		_float3		vOffsetScale, vOffsetPos;

		uint32_t* piFlag;
	}RHAND_EFFECT_DESC;
private:
	CPlayer_RightHand_Effect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer_RightHand_Effect(const CPlayer_RightHand_Effect& Prototype);
public:
	virtual ~CPlayer_RightHand_Effect();


	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render()override;

	virtual HRESULT			Render_Bloom() override;
private:
	shared_ptr<Engine::CVIBuffer_Particle_Point>		m_pVIBuffer{ nullptr };
	shared_ptr<Engine::CMeshNonAnime>					m_pVIBufferNonAnime{ nullptr };
	RIGHTEFFECT											m_eType{};

	vector<uint32_t>									m_MeshNameList;

	uint32_t*											m_piFlag;
	uint32_t											m_iPatternCnt{}, m_iArrayCnt{};

	_float4x4*											m_pParentMatirx{};
	_float4x4											m_OffsetMatrix{};
	_float4x4											m_FinalMatrix{};

	_float												m_fTime{}, m_fTimeTick{}, m_fOffset{};
	_float												m_fAngle{};
	int32_t												m_iTextureId;

	array<_bool, 8>									m_iElectricPattern;
public:
	static		unique_ptr<CPlayer_RightHand_Effect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual		shared_ptr<CPrototype>		Clone(void* pArg) override;


};

NS_END
