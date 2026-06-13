#pragma once
#include "Client_Defines.h"
#include "LightEffect.h"
#include "Observer.h" 


NS_BEGIN(Client)
class CWorldLight : public CLightEffect, public CObserver
{
public:
	typedef struct tagLightObjectDesc : public CLightEffect::LIGHTEFFECT_DESC
	{
		
	}WORLDLIGHT_DESC;
private:
	CWorldLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWorldLight(const CWorldLight& Prototye);
public:
	virtual ~CWorldLight();

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Load_Data(void* pDesc, const json& j);
	virtual json	Save_Data();

	virtual void OnNotify(const EVENT& eEvent) override;
private:
	void			Light_Blink (LIGHT_DESC* pDesc,  const _float& fTimeDelta);
	void			Light_Blink2(LIGHT_DESC* pDesc,  const _float& fTimeDelta);
	void			Light_Blink3(LIGHT_DESC* pDesc,  const _float& fTimeDelta);
	void			Light_Blink4(LIGHT_DESC* pDesc, const _float& fTimeDelta);
	void			Light_ON(LIGHT_DESC* pDesc, const _float& fTimeDelta);
	void			Light_OFF(LIGHT_DESC* pDesc, const _float& fTimeDelta);


	void			Light_Blink_EVENT(LIGHT_DESC* pDesc, const _float& fTimeDelta);
private:
	HRESULT			Ready_Component();

private:
	LIGHT_DESC		m_LightOrigin{};
	LIGHT_DESC		m_LightCurrent{};
	_float			m_fTick{}, m_fTickTwo{};

	_bool			m_bControl{ false };
	_bool			m_bUpdateOriginDesc{ false };
	uint32_t		m_iTickCnt{};
	array<_bool,8>			m_LightPatternTable{ true, true ,false,true,false,true,false,true };
public:
	static unique_ptr<CWorldLight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;


};

NS_END

