#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(_float fSize = 0.5f);
	virtual HRESULT Initialize(void* pArg);

public:
	void SetSize(_float fX, _float fY);
public:
	static CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fSize = 0.5f);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END