#pragma once

#include "Component.h"
#include "3DModel.h"
#include "Mesh.h"
#include "Transform.h"
#include "MeshNonAnime.h"
#include "NonModel.h"
#include "Cube.h"
#include "Terrain.h"
#include "Shader.h"
#include "Animation.h"
#include "Animator.h"
#include "Door.h"
#include "Lever.h"
#include "RollupDoor.h"
#include "GreenEletric.h"
#include "Battery.h"
#include "BatteryCase.h"
#include "DebugLine.h"
#include "FSM_Machine.h"
#include "Rect.h"
#include "BlueElectric.h"
#include "ElectricPole.h"
#include "ElectricPannel.h"
#include "ElectricPoleHead.h"
#include "LowerFlip.h"
#include "Triangle.h"
#include "LowerFlip_Flip.h"
#include "Cell.h"
#include "Observer.h"
#include "Navigation.h"
#include "Generator.h"
#include "UIObject.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Instance_Mesh.h"
#include "Elevator.h"
#include "Light.h"
#include "LightEffect.h"
NS_BEGIN(Engine)
class CPrototype_Manager final
{
private:
	CPrototype_Manager();
public:
	~CPrototype_Manager();

public:
	HRESULT Initialize(uint32_t iNumLevels);
	HRESULT	Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<CPrototype> pPrototype);
	shared_ptr<CPrototype> Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(uint32_t iClearLevelIndex);
private:
	uint32_t m_iNumLevels = {};

private:
	unique_ptr<map<const _wstring, unique_ptr<CPrototype>>[]> m_pPrototypes = { nullptr };
	typedef map<const _wstring, unique_ptr<CPrototype>> PROTOTYPES;

private:
	CPrototype* Find_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag);

public:
	static unique_ptr<CPrototype_Manager> Create(uint32_t iNumLevels);

};

NS_END

