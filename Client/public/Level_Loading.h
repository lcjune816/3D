#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 레벨로서의 기본역활 */
/* -> 이 레벨에 필요한 객체를 생성ㅎ나다. */
/* -> 생성한 객체의 반복적인 업데이트 렌더등을 호출해준다. */

/* 다음 레벨을 보여주기위해 필요한 자원을 미리 준비해주는 역활 */

NS_BEGIN(Client)

class CLevel_Loading final :public CLevel
{
private:
	CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel_Loading();

public:
	virtual HRESULT Initialize(LEVEL eNextLevelIndex);
	virtual void Update(_float fTimeDelta);// override;
	virtual HRESULT Render();// override;

private:
	LEVEL							m_eNextLevelIndex = { LEVEL::END };
	unique_ptr<class CLoader>		m_pLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);


public:
	static unique_ptr<CLevel_Loading> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex);
};

NS_END