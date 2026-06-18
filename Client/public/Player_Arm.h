#pragma once
#include "Player.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
class CCube;
NS_END
NS_BEGIN(Client)
class CPlayer_Arm final : public CPlayer
{
	typedef struct strGrabarm
	{
		vector<_float4x4> Matrix;
		vector<uint32_t>   CollisionIndex;
	}GRAB_ARM;
private:
	CPlayer_Arm(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer_Arm(const CPlayer_Arm& Prototye);
public:
	virtual ~CPlayer_Arm();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();
	virtual HRESULT			Render_Bloom() override;
	void					Bind_ResourceFromFlag(CShader* pShader,const _char* pConstantName);
	void					Bind_ResourceFromFlag_Default(CShader* pShader, const _char* pConstantName);
	void					Set_Shoot(_bool bShoot) { m_bShoot = bShoot; }

	void					Set_SameFlag(uint32_t iFlag) { m_iStateFlag = iFlag; }
public:
	strGrabarm&				Get_ArmMatrix()	{ return m_ArmMatrix; }

public:
	string					Model_Animation(const vector<string>& pNames);
private:
	HRESULT					Ready_Component();
private:
	
	_bool								m_bShoot = false;
	_float4x4							m_offSet = {};
	shared_ptr<Engine::CCube>			m_pBoxMesh = { nullptr };

	_float4x4							m_matOffset{};
	vector<uint32_t>					m_MeshNameList;
	strGrabarm							m_ArmMatrix;
public:
	static unique_ptr<CPlayer_Arm> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

