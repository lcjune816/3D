#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CObserver abstract
{
public:
	virtual ~CObserver() = default;

public:
	virtual void OnNotify(const EVENT& event) PURE;


};

NS_END

