#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_float3 Get_Point(POINT ePoint) const {
		return m_vPoints[ePoint];
	}

	_int Get_Index() const {
		return m_iIndex;
	}

	void Set_NeighborIndex(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndex[eLine] = pNeighbor->Get_Index();
	}


public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare(const _float3& vSourPoint, const _float3& vDestPoint);

#ifdef _DEBUG
public:
	HRESULT Render_Cell();
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	_int					m_iIndex = 0;
	_float3					m_vPoints[POINT_END];
	_float3					m_vNormal[LINE_END];

	_int					m_iNeighborIndex[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
	class CVIBuffer_Cell*	m_pVIBuffer = nullptr;
	class CShader*			m_pShader = nullptr;
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

END