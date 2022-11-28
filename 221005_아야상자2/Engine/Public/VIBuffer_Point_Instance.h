#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer
{
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg);

public:
	void Update(_float fTimeDelta);

	_float GetRandomFloat(_float fLowBound, _float fHighBound);
	void GetRandomVector(_float3* vOut, _float3* vMin, _float3* vMax);

	void ResetParticle(_uint iIndex);
public:
	virtual HRESULT Render();

private:
	_uint					m_iNumInstance = 0;
	_uint					m_iInstanceStride = 0;
	ID3D11Buffer*			m_pVBInstance = nullptr;

	_float*					m_pInstanceSpeeds = nullptr;
	
	_float					m_fParticleDeviationX = 0.5f;
	_float					m_fParticleDeviationY = 0.1f;
	_float					m_fParticleDeviationZ = 2.0f;

	_float					m_fSpeed = 10.0f;
	vector<_float3>			m_vDirection;
	vector<_float>			m_fJumpTime;
public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END