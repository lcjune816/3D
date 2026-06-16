#pragma once
#include "Player.h"

NS_BEGIN(Engine)
class CRect;
NS_END

NS_BEGIN(Client)
class CPlayer_Light : public CPlayer
{
public:
	typedef struct strPlayerLight : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4* pParentMatrix;
	}PLAYER_LIGHT_DESC;
protected:
	CPlayer_Light(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer_Light(const CPlayer& CPrototype);
public:
	virtual ~CPlayer_Light();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					CombineMatrix();
	HRESULT				Ready_Component(void* pArg, uint32_t iLevelIndex);

private:
	shared_ptr<Engine::CRect>		m_pVIBufferCom = { nullptr };
	shared_ptr<class CWorldLight>			m_pLight;
private:
	_float4x4*				m_pParentMatrix{};
	_float4x4				m_OffsetMatrix{};
	_bool					m_bLightOnOff{false};

	int32_t					m_iTextureID;
public:
	static unique_ptr<CPlayer_Light> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>	Clone(void* pArg) override;

};


NS_END