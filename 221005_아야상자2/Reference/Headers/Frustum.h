#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

public:
	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRadius);

private:
	_float3			m_vOriginalPoint[8];
	_float4			m_WorldPlane[6];

public:
	virtual void Free() override;
};

END