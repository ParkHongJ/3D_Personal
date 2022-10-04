#include "..\Public\Picking.h"
#include "Transform.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_hWnd = hWnd;

	m_iWinCX = iWinCX;

	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	POINT			ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	_float3			vMousePos;

	/* ������������ ���콺 ��ǥ�� ���Ѵ�. */
	vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePos.z = 0.f;


	/* �佺���̽� ���� ��ǥ�� ���Ѵ�. */
	_float4x4		ProjMatrixInv;
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	XMStoreFloat4x4(&ProjMatrixInv, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ProjMatrixInv)));

	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&ProjMatrixInv)));

	/* �佺���̽� ���� ���콺 ���̿� ���������� ������. */
	m_vRayPos = _float3(0.f, 0.f, 0.f);

	XMStoreFloat3(&m_vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vRayPos));

	/* ���彺���̽� ���� ���콺 ���̿� ���������� ������. */	
	_float4x4		ViewMatrixInv;
	XMStoreFloat4x4(&ViewMatrixInv, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrixInv)));
	
	XMVECTOR xmRayPos = XMLoadFloat3(&m_vRayPos);
	XMVECTOR xmRayDir = XMLoadFloat3(&m_vRayDir);
	xmRayPos = XMVector3TransformCoord(xmRayPos, XMLoadFloat4x4(&ViewMatrixInv));
	xmRayDir = XMVector3TransformNormal(xmRayDir, XMLoadFloat4x4(&ViewMatrixInv));

	XMStoreFloat3(&m_vRayPos, xmRayPos);
	XMStoreFloat3(&m_vRayDir, xmRayDir);
	RELEASE_INSTANCE(CPipeLine);
}

void CPicking::Compute_LocalRayInfo(_float3 * pRayDir, _float3 * pRayPos, CTransform * pTransform)
{
	
	_float4x4		WorldMatrixInv;
	XMStoreFloat4x4(&WorldMatrixInv, pTransform->Get_WorldMatrixInverse());

	XMStoreFloat3(&*pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), XMLoadFloat4x4(&WorldMatrixInv)));
	XMVECTOR Temp = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), XMLoadFloat4x4(&WorldMatrixInv));
	XMStoreFloat3(&*pRayPos, XMLoadFloat3(&m_vRayPos));
	XMStoreFloat3(&*pRayDir, XMLoadFloat3(&m_vRayDir));
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
