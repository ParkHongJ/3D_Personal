#include "..\Public\Transform.h"
#include "Navigation.h"
CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CTransform::MoveToWards(_fvector target, _float MaxDistanceDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	
	_float3		vCurrentPosition;
	XMStoreFloat3(&vCurrentPosition, Get_State(CTransform::STATE_POSITION));

	_vector		vDistance = target - vPosition;

	_float		fMagnitude;
	XMStoreFloat(&fMagnitude, XMVector3Length(vDistance));

	_bool		isMove = true;

	if (fMagnitude <= MaxDistanceDelta || fMagnitude == 0.0f)
	{
		if (nullptr != pNavigation)
			isMove = pNavigation->isMove(target, &vCurrentPosition);

		if (true == isMove)
			Set_State(CTransform::STATE_POSITION, target);
		else
		{
			_float3		vCurPos;
			XMStoreFloat3(&vCurPos, Get_State(CTransform::STATE_POSITION));

			if (isMove = pNavigation->isMove(Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition), &vCurPos))
			{
				Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition));
			}
		}
	}
	else
	{
		_vector vFinalPos = vPosition + vDistance / fMagnitude * MaxDistanceDelta;
		if (nullptr != pNavigation)
			isMove = pNavigation->isMove(vFinalPos, &vCurrentPosition);

		if (true == isMove)
			Set_State(CTransform::STATE_POSITION, vFinalPos);
		else
		{
			_float3		vCurPos;
			XMStoreFloat3(&vCurPos, Get_State(CTransform::STATE_POSITION));

			if (isMove = pNavigation->isMove(Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition), &vCurPos))
			{
				Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition));
			}
		}
	}
	
	//current + a / magnitude * maxDistanceDelta;
	/* 프렐이어가 움직이고 난 이후의 위치를 네비게이션에 전달하여. */
	/* 현재 상황에서 움직일 수 있늕지 체크한다. */

	
	/*_float3 MoveTowards(_float3 current, _float3 target, float maxDistanceDelta)
	{
		_float3 a = target - current;
		float magnitude = D3DXVec3Length(&a);
		if (magnitude <= maxDistanceDelta || magnitude == 0.f)
		{
			return target;
		}
		return current + a / magnitude * maxDistanceDelta;
	}*/
	
}

void CTransform::MoveToWards(_fvector target, _float MaxDistanceDelta)
{
	/*public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
	{
		Vector3 a = target - current;
		float magnitude = a.magnitude;
		if (magnitude <= maxDistanceDelta || magnitude == 0f)
		{
			return target;
		}
		return current + a / magnitude * maxDistanceDelta;
	}*/
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	_float3		vCurrentPosition;
	XMStoreFloat3(&vCurrentPosition, Get_State(CTransform::STATE_POSITION));

	_vector		vDistance = target - vPosition;

	_float		fMagnitude;
	XMStoreFloat(&fMagnitude, XMVector3Length(vDistance));

	if (fMagnitude <= MaxDistanceDelta || fMagnitude == 0.0f)
	{
		Set_State(CTransform::STATE_POSITION, target);
	}
	else
	{
		_vector vFinalPos = vPosition + vDistance / fMagnitude * MaxDistanceDelta;

		Set_State(CTransform::STATE_POSITION, vFinalPos);
	}


}

HRESULT CTransform::Initialize_Prototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());	

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	_float3		vCurrentPosition;
	XMStoreFloat3(&vCurrentPosition, Get_State(CTransform::STATE_POSITION));

	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
	/* 프렐이어가 움직이고 난 이후의 위치를 네비게이션에 전달하여. */
	/* 현재 상황에서 움직일 수 있늕지 체크한다. */
	_bool		isMove = true;

	if (nullptr != pNavigation)
		isMove = pNavigation->isMove(vPosition, &vCurrentPosition);

	if (true == isMove)
	{
		/*_float fHeight = ;*/
		vPosition =	XMVectorSetY(vPosition, pNavigation->GetHeight(vPosition));
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else
	{
		_float3		vCurPos;
		XMStoreFloat3(&vCurPos, Get_State(CTransform::STATE_POSITION));

		if (isMove = pNavigation->isMove(Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition), &vCurPos))
		{
			Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition));
		}
	}
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta);
	
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_float3		vCurrentPosition;
	XMStoreFloat3(&vCurrentPosition, Get_State(CTransform::STATE_POSITION));

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
	/* 프렐이어가 움직이고 난 이후의 위치를 네비게이션에 전달하여. */
	/* 현재 상황에서 움직일 수 있늕지 체크한다. */
	_bool		isMove = true;

	if (nullptr != pNavigation)
		isMove = pNavigation->isMove(vPosition, &vCurrentPosition);

	if (true == isMove)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
		Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition));
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	_float3		vCurrentPosition;
	XMStoreFloat3(&vCurrentPosition, Get_State(CTransform::STATE_POSITION));

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
	/* 프렐이어가 움직이고 난 이후의 위치를 네비게이션에 전달하여. */
	/* 현재 상황에서 움직일 수 있늕지 체크한다. */
	_bool		isMove = true;

	if (nullptr != pNavigation)
		isMove = pNavigation->isMove(vPosition, &vCurrentPosition);

	if (true == isMove)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
		Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition));
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	_float3		vCurrentPosition;
	XMStoreFloat3(&vCurrentPosition, Get_State(CTransform::STATE_POSITION));

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
	/* 프렐이어가 움직이고 난 이후의 위치를 네비게이션에 전달하여. */
	/* 현재 상황에서 움직일 수 있늕지 체크한다. */
	_bool		isMove = true;

	if (nullptr != pNavigation)
		isMove = pNavigation->isMove(vPosition, &vCurrentPosition);

	if (true == isMove)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
		Set_State(CTransform::STATE_POSITION, Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vCurrentPosition));
}

void CTransform::Set_Scale(_fvector vScaleInfo)
{
	Set_State(CTransform::STATE_RIGHT, 
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
	Set_State(CTransform::STATE_UP, 
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
	Set_State(CTransform::STATE_LOOK, 
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));	
}

_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK))));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);
	
	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		Scale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));
}

void CTransform::TurnQuat(_fvector vDir, _float fTimeDelta)
{
	XMVectorSetY(vDir, 0.f);
	_vector		vLook = XMVector3Normalize(vDir);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_matrix DestRotationMatrix;
	DestRotationMatrix.r[0] = vRight;
	DestRotationMatrix.r[1] = vUp;
	DestRotationMatrix.r[2] = vLook;
	DestRotationMatrix.r[3] = Get_State(CTransform::STATE_POSITION);

	_vector		vDestRot;
	vDestRot = XMQuaternionRotationMatrix(DestRotationMatrix);

	_vector		vSourRot;
	vSourRot = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_WorldMatrix));

	if (XMQuaternionEqual(vSourRot, vDestRot))
	{
		return;
	}

	vSourRot = XMQuaternionSlerp(vSourRot, vDestRot, fTimeDelta);	

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&Get_Scale()), XMVectorSet(0.f, 0.f, 0.f, 1.f), vSourRot, XMVectorSetW(Get_State(CTransform::STATE_POSITION), 1.f));

	XMStoreFloat4x4(&m_WorldMatrix, TransformationMatrix);
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookDir(_fvector vDir)
{
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);

	_vector		vUp = XMVector3Cross(vDir, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vDir) * vScale.z);
}


void CTransform::LookAt_ForLandObject(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	


	_float3		vScale = Get_Scale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;	

	vLook = XMVector3Normalize(XMVector3Cross(vRight, Get_State(CTransform::STATE_UP))) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_LOOK, vLook);

}

void CTransform::Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if(fDistance > fLimitDistance)
		vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*			pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*			pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
