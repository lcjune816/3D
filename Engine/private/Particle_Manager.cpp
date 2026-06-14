#include "Particle_Manager.h"
#include "GameInstance.h"
#include "ParticleObject.h"
CParticle_Manager::CParticle_Manager()
{

}

CParticle_Manager::~CParticle_Manager()
{

}

HRESULT CParticle_Manager::Initialize(uint32_t iNumLevels)
{
	m_Particles.resize(iNumLevels);
	m_ParticlesPool.resize(iNumLevels);
	m_iNumLevels = iNumLevels;
	return S_OK;
}
void    CParticle_Manager::Priority_Update_Particle(_float fTimeDelta)
{


	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Particles[i])
		{
			pair->Priority_Update(fTimeDelta);
		}
		for (auto& Pool : m_ParticlesPool[i])
		{
			Pool->Priority_Update(fTimeDelta);
		}
	}

}
void    CParticle_Manager::Update_Particle(_float fTimeDelta)
{
	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto iter = m_Particles[i].begin(); iter != m_Particles[i].end();)
		{
			auto Next = std::next(iter);
			_bool bEndCycle = (*iter)->EndCycle();

			if (bEndCycle)
			{
				m_ParticlesPool[i].splice(m_ParticlesPool[i].end(), m_Particles[i], iter);
			}
			else	
				(*iter)->Update(fTimeDelta);
		
			iter = Next;
		}
	
		//for (auto Pool = m_ParticlesPool[i].begin(); Pool != m_ParticlesPool[i].end();)
		//{
		//	(*Pool)->Update(fTimeDelta);
		//
		//	if ((*Pool)->Get_Dead())
		//	{
		//		Pool = m_ParticlesPool[i].erase(Pool);
		//		continue;
		//	}
		//
		//	++Pool;
		//}
	}

}

void    CParticle_Manager::Late_Update_Particle(_float fTimeDelta)
{
	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Particles[i])
		{
			pair->Late_Update(fTimeDelta);
		}

		//for (auto& Pool : m_ParticlesPool[i])
		//{
		//	Pool->Late_Update(fTimeDelta);
		//}
	}

}

void CParticle_Manager::Clear(uint32_t iClearLevelIndex)
{
	if (iClearLevelIndex >= m_iNumLevels)
		return;

	for (auto& pObj : m_Particles[iClearLevelIndex])
	{
		pObj.reset();
	}
	for (auto& pObj : m_ParticlesPool[iClearLevelIndex])
	{
		pObj.reset();
	}
	m_ParticlesPool[iClearLevelIndex].clear();
	m_Particles[iClearLevelIndex].clear();
}
HRESULT CParticle_Manager::Add_ParticleToPool(const _wstring strPrototypeTag,  uint32_t iPrototypeLevel, uint32_t iGameLevel,  void* pArg)
{

	auto pObj = static_pointer_cast<CParticleObject>(CGameInstance::Get().Clone_Prototype(iPrototypeLevel, strPrototypeTag,pArg));
	if (NULL_TRUE(pObj))
		return E_FAIL;

	m_ParticlesPool[iGameLevel].push_back(pObj);
	
	return S_OK;
}


void  CParticle_Manager::Save_ParticleData(uint32_t iNumLevel, _wstring path, const string& pSaveArrayName)
{
	json j;

	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& iter : m_ParticlesPool[i])
		{
			if (iter->Save_Data() == nullptr)
				continue;

			j[pSaveArrayName].push_back(iter->Save_Data());
		}
		for (auto& iter : m_Particles[i])
		{
			if (iter->Save_Data() == nullptr)
				continue;

			j[pSaveArrayName].push_back(iter->Save_Data());
		}
	}

	ofstream file(path);
	file << j.dump(4);

	file.close();

	MSG_BOX("파티클 된듯?");
}

weak_ptr<class CParticleObject> CParticle_Manager::Select_Particle_Object(_fvector vOrigin, _fvector fDir)
{
	_float fMaxDist = FLT_MAX;

	shared_ptr<CParticleObject> pObj{};
	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& iter : m_ParticlesPool[i])
		{
			auto pTransform = iter->Get_Transform().lock();
			_float fDist{};

			BoundingBox box;

			_matrix SrcMatrix = pTransform->Get_World();
			_matrix SrcInverseWorld = XMMatrixInverse(nullptr, SrcMatrix);
			_float3 fMax = { 1,1,1 };
			_float3 fMin = { -1,-1,-1 };
			
			_vector pSrcLocalMax = XMLoadFloat3(&fMax);//XMVector3TransformCoord(XMLoadFloat3(&fMax), SrcInverseWorld);
			_vector pSrcLocalMin = XMLoadFloat3(&fMin);// XMVector3TransformCoord(XMLoadFloat3(&fMin), SrcInverseWorld);

			XMStoreFloat3(&box.Center, (pSrcLocalMax + pSrcLocalMin) * 0.5f);
			XMStoreFloat3(&box.Extents, (pSrcLocalMax - pSrcLocalMin) * 0.5f);

			_vector pDestLocalPos = XMVector3TransformCoord(vOrigin, SrcInverseWorld);
			_vector pDestLocalDir = XMVector3TransformNormal(fDir, SrcInverseWorld);

			if(box.Intersects(pDestLocalPos,pDestLocalDir,fDist))
			{
				if (fMaxDist > fDist)
				{
					pObj = iter;
					fMaxDist = fDist;
				}
			}

		}
	}
	return pObj;
}



void CParticle_Manager::Particle_Emit(WORLD_EVENT eParticleType)
{
	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto iter = m_ParticlesPool[i].begin(); iter != m_ParticlesPool[i].end();)
		{
			auto Next = std::next(iter);

			if (NULL_FALSE((*iter)) && (*iter)->CompareParticleType(eParticleType))
			{
				m_Particles[i].splice(m_Particles[i].end(), m_ParticlesPool[i], iter);
			}

			iter = Next;
		}
	}
}

unique_ptr<CParticle_Manager> CParticle_Manager::Create(uint32_t iNumLevels)
{
	auto		pInstance = unique_ptr<CParticle_Manager>(new CParticle_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CParticle_Manager");
		return nullptr;
	}

	return pInstance;
}