#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
public:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(); /* 월드스페이스 상의 마우스레이, 레이포스를 구한다. */
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, class CTransform* pTransform);
	
	//월드스페이스 상의 레이, 포스를 가져온다.
	void GetRayWorldInfo(_float3* pRayDir, _float3* pRayPos);
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	HWND					m_hWnd;
	_uint					m_iWinCX, m_iWinCY;

	_float3					m_vRayDir;
	_float3					m_vRayPos;

public:
	virtual void Free() override;
};

END