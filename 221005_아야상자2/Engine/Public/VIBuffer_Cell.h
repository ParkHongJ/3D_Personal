#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Render();
	void EditVerteces(_uint iNumIndex, _float3 vPos, _float3& vOriginPos) {
	
		D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
		ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);
		
		//실제 리소스가 있는 데이터
		XMStoreFloat3(&((VTXCOL*)MappedSubResource.pData)[iNumIndex].vPosition,
			XMLoadFloat3(&((VTXCOL*)MappedSubResource.pData)[iNumIndex].vPosition) + 
			XMLoadFloat3(&vPos));

		m_pContext->Unmap(m_pVB, 0);
		XMStoreFloat3(&vOriginPos, XMLoadFloat3(&vOriginPos) + XMLoadFloat3(&vPos));
	}
public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END