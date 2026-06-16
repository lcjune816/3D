#pragma once
#include "Client_Defines.h"
#include "LightEffect.h"
#include "Observer.h" 
NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)
class CWorldLight : public CLightEffect, public CObserver
{
public:
	typedef struct tagLightObjectDesc : public CLightEffect::LIGHTEFFECT_DESC
	{
		INIT_TYPE eType{ INIT_TYPE::END };
		LIGHT_DESC LightDesc{};
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

	void    Set_LightState(LIGHT_STATE eType) { m_eLocalEventType = eType; }
	virtual void OnNotify(const EVENT& eEvent) override;
	LIGHT_DESC* Get_LightDescPtr();
private:
	void			Light_Blink (LIGHT_DESC* pDesc,  const _float& fTimeDelta);
	void			Light_Blink2(LIGHT_DESC* pDesc,  const _float& fTimeDelta);
	void			Light_Blink3(LIGHT_DESC* pDesc,  const _float& fTimeDelta);
	void			Light_Blink4(LIGHT_DESC* pDesc, const _float& fTimeDelta);
	void			Light_ON(LIGHT_DESC* pDesc, const _float& fTimeDelta);

	void			Light_SLOW_OFF(LIGHT_DESC* pDesc, const _float& fTimeDelta);
	void			Light_SLOW_ON(LIGHT_DESC* pDesc, const _float& fTimeDelta);
	void			Light_OFF(LIGHT_DESC* pDesc, const _float& fTimeDelta);


	void			Light_Blink_EVENT(LIGHT_DESC* pDesc, const _float& fTimeDelta);
private:
	HRESULT			Ready_Component();
	void			Ready_LightType(LIGHT_STATE eState ,LIGHT_DESC* pLightDesc);
	void			Ready_WorldEvent(WORLD_EVENT eEvent,LIGHT_DESC* pLightDesc);
private:

	shared_ptr<Engine::CLight>			m_pLight{ nullptr };

	LIGHT_DESC		m_LightOrigin{};
	LIGHT_DESC		m_LightCurrent{};

	LIGHT_STATE		m_PreState;
	_float			m_fTick{}, m_fTickTwo{};

	INIT_TYPE		m_eInit = INIT_TYPE::END;
	_bool			m_bControl{ false };
	_bool			m_bUpdateOriginDesc{ false };
	uint32_t		m_iTickCnt{};
	array<_bool,8>			m_LightPatternTable;
public:
	static unique_ptr<CWorldLight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;


};

NS_END

