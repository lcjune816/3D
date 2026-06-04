#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)
class CParticle_Manager final
{
private:
	CParticle_Manager();
public:
	virtual ~CParticle_Manager();

public:
	HRESULT Initialize(uint32_t iNumLevels);
	void Priority_Update_Particle(_float fTimeDelta);
	void Update_Particle(_float fTimeDelta);
	void Late_Update_Particle(_float fTimeDelta);

	void Clear(uint32_t iClearLevelIndex);
public:
	HRESULT Add_ParticleToPool(const _wstring strPrototypeTag, uint32_t iPrototypeLevel,  uint32_t iGameLevel , void* pArg);
	void	   Particle_Emit(WORLD_EVENT eParticleType);

	void	   Save_Data(uint32_t iNumLevel, _wstring path, const _wstring& strLayerName, const string& pSaveArrayName);
	weak_ptr <class CParticleObject>	Select_Particle_Object(_fvector vOrigin, _fvector fDir);


private:
	uint32_t		m_iNumLevels = {};

private:
	
	vector<list<shared_ptr<class CParticleObject>>>			m_Particles{};
	vector<list<shared_ptr<class CParticleObject>>>			m_ParticlesPool{};
public:
	class CLayer* Find_Layer(uint32_t iLayerPr, const _wstring& strLayerTag);

public:
	static unique_ptr<CParticle_Manager> Create(uint32_t iNumLevels);
};

NS_END
