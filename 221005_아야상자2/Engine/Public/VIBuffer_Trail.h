#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

typedef struct TrailVertex
{
	// Stores the starting position of the particle.
	_float3 vPosition;

	// Stores TexCoords
	_float2 TextureCoordinate;

	// Visibility term
	float Visibility;
	/*static VertexDeclaration VertexDeclaration = new VertexDeclaration
	(
		new VertexElement(0, VertexElementFormat.Vector3,
			VertexElementUsage.Position, 0),
		new VertexElement(12, VertexElementFormat.Vector2,
			VertexElementUsage.TextureCoordinate, 0),
		new VertexElement(20, VertexElementFormat.Single,
			VertexElementUsage.TextureCoordinate, 0)
	);*/

}TRAIL_INFO;

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	TRAIL_INFO* m_pTrailVertices = nullptr;
	_uint		m_iSegmentsUsed = 0;
	_uint		m_iSegments;
	
	//±Ê¿Ã
	_float		m_fSegmentLength;
	_float3		m_vLastSegmentPosition;
	_float		m_fWidth = 1.f;

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END