#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"
#include "GameMgr.h"
CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	

	
	CGameMgr* pGameMgr = GET_INSTANCE(CGameMgr);
	pGameMgr->RegisterCamera(this);
	RELEASE_INSTANCE(CGameMgr);
	XMStoreFloat3(&m_vPivot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

_bool CCamera_Free::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//Layer_Yantari
	if (pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		if (nullptr == m_pTargetTransform)
		{
			m_pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Yantari", L"Com_Transform", 0);;
			if (nullptr != m_pTargetTransform)
			{
				Safe_AddRef(m_pTargetTransform);
			}
		}
	}

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	_long	MouseMove = 0;

	/*if (pGameInstance->Get_DIMKeyState(DIMK_RBUTTON))
	{
		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
		}

		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
		}
	}*/
	
	/*if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
	}

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
	}
*/
	RELEASE_INSTANCE(CGameInstance);



	return false;
}

void CCamera_Free::LateTick(_float fTimeDelta)
{
	if (nullptr != m_pTargetTransform)
	{
		//타겟위치
		_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);

		//플레이어 위치
		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		//	//내가 봐야할 곳의 좌표 = 타겟 - 플레이어의 사이
		//	_vector vLookAtPos = XMVectorSetW((vTargetPos - vPlayerPos) * 0.5f + vPlayerPos, 1.f);

		//	//둘 사이의 거리
		//	_vector vDistance = XMVector3Length((vTargetPos - vPlayerPos) * 0.5f);

		//	//카메라의 현재위치
		//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		//	_vector vCamPos = XMLoadFloat3(&m_vCamPosition);

		//	if (XMVectorGetX(vDistance) > 3.f)
		//	{
		//		//XMStoreFloat3(&m_vCamPosition, XMVectorSet(0.f, 1.f, -5.f, 1.f) * XMVectorGetX(vDistance));
		//		//XMStoreFloat3(&m_vCamPosition, XMVectorSet(0.f, 1.f, -5.f, 1.f) - XMVector3Normalize(XMVectorSetY(vTargetPos - vMyPos, 0.0f)) * XMVectorGetX(vDistance));
		//		 
		//	}
		//	else
		//	{
		//		XMStoreFloat3(&m_vCamPosition, XMVectorSet(0.f, 1.f, -10.f, 1.f));
		//	}

		//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//	_matrix ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
		//	_matrix	ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
		//	RELEASE_INSTANCE(CGameInstance);

		//	
		//	_vector vViewTargetPos = XMVector3TransformCoord(vTargetPos, ViewMatrix);
		//	_vector vProjTargetPos = XMVector3TransformCoord(vViewTargetPos, ProjMatrix);

		//	_vector vLook = XMVectorSetY(XMVector3Normalize(vTargetPos - vMyPos), 0.f);
		//	_float fRotSpeed = 7.f;
		//	//m_pTransformCom->TurnQuat(vLook, fTimeDelta * fRotSpeed);
		//	
		//	//가운데를 기준으로 오른쪽에 있다면.
		//	if (0 < XMVectorGetX(vProjTargetPos))
		//	{
		//		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fRotSpeed);
		//		m_pTransformCom->TurnQuatByAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLookAtPos, fTimeDelta * fRotSpeed);
		//	}
		//	else
		//	{
		//		m_pTransformCom->TurnQuatByAxis(XMVectorSet(0.f, -1.f, 0.f, 0.f), vLookAtPos, fTimeDelta * fRotSpeed);
		//	}
		//	
		//	//내가 보려고 하는 위치로 이동 후
		//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLookAtPos);
		//	
		//	_vector vDestPos = XMVector3TransformCoord(XMLoadFloat3(&m_vCamPosition), m_pTransformCom->Get_WorldMatrix());

		//	vDestPos = XMVectorLerp(vMyPos, vDestPos, fTimeDelta);
		//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
		//}

		_vector vMyPos = XMLoadFloat3(&m_vPivot);
		_vector vTarget = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 3.f, 0.f, 0.f);

		XMStoreFloat3(&m_vPivot, m_pTransformCom->MoveToWards(vMyPos, vTarget, XMVectorGetX(XMVector3Length(vMyPos - vTarget)) * fTimeDelta * 3.f));
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.f));
		
		m_pTransformCom->LookAt((vTargetPos + vPlayerPos) * 0.5f);
		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vMyPos -= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMyPos);
	}
	__super::Tick(fTimeDelta);
}

HRESULT CCamera_Free::Render()
{

	return S_OK;
}

_float3 CCamera_Free::GetNormalizeDir(_uint eState)
{
	_float3 vDir;
	XMStoreFloat3(&vDir, m_pTransformCom->Get_State((CTransform::STATE)eState));
	XMStoreFloat3(&vDir, XMVector3Normalize(XMLoadFloat3(&vDir)));
	vDir.y = 0.0f;
	return vDir;
}

HRESULT CCamera_Free::Set_Player(CTransform * pPlayerTransform)
{
	m_pPlayerTransform = pPlayerTransform;
	if (nullptr == m_pPlayerTransform)
	{
		return E_FAIL;
	}

	//XMStoreFloat3(&m_vCamPosition, XMVectorSet(0.f, 1.f, -5.f, 0.f));
	//_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//vPlayerPos = vPlayerPos + XMVectorSet(0.f, 0.f, 3.f, 0.f);
	//m_pTransformCom->LookAt(vPlayerPos);
	Safe_AddRef(m_pPlayerTransform);
	m_fCamDistance = 12.f;
	return S_OK;
}

CCamera_Free * CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Free::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Free*		pInstance = new CCamera_Free(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pPlayerTransform);
}
