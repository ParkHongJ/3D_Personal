#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"
#include "Transform.h"
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

	/*CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Transform", 0);;
	if (nullptr == m_pPlayerTransform)
	{
		return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
	}
	Safe_AddRef(m_pPlayerTransform);
	RELEASE_INSTANCE(CGameInstance);*/

	
	CGameMgr* pGameMgr = GET_INSTANCE(CGameMgr);
	pGameMgr->RegisterCamera(this);
	RELEASE_INSTANCE(CGameMgr);
	
	return S_OK;
}

_bool CCamera_Free::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

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
	
	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
	}

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
	}

	Safe_Release(pGameInstance);



	return false;
}

void CCamera_Free::LateTick(_float fTimeDelta)
{
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMLoadFloat3(&m_vCamPosition), m_pTransformCom->Get_WorldMatrix()));

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

	XMStoreFloat3(&m_vCamPosition, XMVectorSet(0.f, 1.f, -5.f, 0.f));
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPlayerPos = vPlayerPos + XMVectorSet(0.f, 0.f, 3.f, 0.f);
	m_pTransformCom->LookAt(vPlayerPos);
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
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerTransform);
}
