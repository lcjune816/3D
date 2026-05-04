#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPrototype abstract : public enable_shared_from_this<CPrototype>
{
protected:
	CPrototype();
public:
	virtual ~CPrototype();

public:
	virtual shared_ptr<CPrototype> Clone(void* Arg) = 0;
};

NS_END

