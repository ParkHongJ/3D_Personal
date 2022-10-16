#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Collider.h"
#include "Sphere.h"
#include "Picking.h"
CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_float CCell::Compute_Height(_fvector vTargetPos)
{
	_vector vResult = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&m_vPoints[POINT_C]));

	return (-XMVectorGetX(vResult) * XMVectorGetX(vTargetPos) - XMVectorGetZ(vResult) * XMVectorGetZ(vTargetPos) - XMVectorGetW(vResult)) / XMVectorGetY(vResult);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	_vector		vLine[LINE_END];


	vLine[LINE_AB] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	vLine[LINE_BC] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	vLine[LINE_CA] = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));

	m_vNormal[LINE_AB] = _float3(XMVectorGetZ(vLine[LINE_AB]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_AB]));
	m_vNormal[LINE_BC] = _float3(XMVectorGetZ(vLine[LINE_BC]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_BC]));
	m_vNormal[LINE_CA] = _float3(XMVectorGetZ(vLine[LINE_CA]) * -1.f, 0.f, XMVectorGetX(vLine[LINE_CA]));

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
	
	for (_uint i = 0; i < 3; ++i)
	{
		m_pCollider[i] = CSphere::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE);
		if (nullptr == m_pCollider)
			return E_FAIL;
		CCollider::COLLIDERDESC desc;
		desc.vCenter = _float3(0.f, 0.f, 0.f);
		desc.vRotation = _float3(0.f, 0.f, 0.f);
		desc.vSize = _float3(0.5f, 0.5f, 0.5f);
		m_pCollider[i]->Initialize(&desc);
	}
#endif

	return S_OK;
}

_bool CCell::Compare(const _float3 & vSourPoint, const _float3 & vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int * pNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormal[i]))))
		{

			*pNeighborIndex = m_iNeighborIndex[i];
			return false;
		}
	}

	return true;
}

_vector CCell::GetSliding(_fvector vPosition, _float3 * vCurrentPosition)
{
	//이동한 위치가 못나간 위치였을때(네비에서 계산함.)
	//가장 가까운 라인을 구함. 법선벡터 구하려고
	_int iLine = GetLine(vPosition);

	//현재 위치와 이동하고자 하는 위치를 빼서 이동한 벡터를구함
	//슬라이딩공식 S = P + n(-P*n) 중에서 P에 해당함
	_vector vDist = vPosition - XMLoadFloat3(&*vCurrentPosition);

	//현재 노말은 바깥방향이라 -를 곱해서 안쪽으로 만들어줌
	_vector vNormal = -XMLoadFloat3(&m_vNormal[iLine]);

	//슬라이딩벡터를 구한다. S = P + n(-P*n)
	_vector vSlide = vDist + vNormal * XMVector3Dot(-vDist, vNormal);

	return vSlide;
}

_int CCell::GetLine(_fvector vPosition)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormal[i]))))
		{
			return i;
		}
	}
	return 0;
}

#ifdef _DEBUG
void CCell::Update(_float2 vPickPos)
{
	/*m_pCollider[0]->Collision()*/
	//m_pVIBuffer->Picking()
}
HRESULT CCell::Render_Cell(_float fHeight, _float4 vColor)
{
	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);
	_float4x4			WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix.m[1][3] = fHeight;

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	m_pShader->Begin(0);

	m_pVIBuffer->Render();

	_float4x4 mat;
	for (_uint i = 0; i < 3; ++i)
	{
		XMStoreFloat4x4(&mat, XMMatrixIdentity());
		mat._41 = m_vPoints[i].x;
		mat._42 = m_vPoints[i].y;
		mat._43 = m_vPoints[i].z;

		m_pCollider[i]->Update(XMLoadFloat4x4(&mat));
		m_pCollider[i]->Render();
	}
	return S_OK;
}
_bool CCell::Picking(_uint* iNumPoint)
{
	CPicking*		pPicking = GET_INSTANCE(CPicking);
		
	_float3 vRayDir, vRayPos;
	_float fDist = 0.f;
	pPicking->GetRayWorldInfo(&vRayDir, &vRayPos);
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));
	for (_uint i = 0; i < 3; ++i)
	{
		_bool IsColl = m_pCollider[i]->Picking(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), fDist);
		if (IsColl)
		{
			*iNumPoint = i;
			RELEASE_INSTANCE(CPicking);
			return true;
		}
	}
	RELEASE_INSTANCE(CPicking);
	return false;
}
#endif // _DEBUG

void CCell::EditCell(_uint iNumIndex, _float3 vPos)
{
	m_pVIBuffer->EditVerteces(iNumIndex, vPos, m_vPoints[iNumIndex]);
}

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*			pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CCell"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
#endif // _DEBUG
	for (auto& pCollider : m_pCollider)
	{
		Safe_Release(pCollider);
	}
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
