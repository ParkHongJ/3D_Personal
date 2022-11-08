#include "stdafx.h"
#include "..\Public\Projectile.h"
#include "GameInstance.h"
#include <time.h>
CProjectile::CProjectile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile & rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	memcpy(&m_ProjInfo, pArg, sizeof(ProjectileInfo));
	
	SetDir(XMLoadFloat3(&m_ProjInfo.vDir));
	
	if (m_ProjInfo.ePhase == PHASE1)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_ProjInfo.vPos), 1.f));
	}
	else
	{
		XMStoreFloat3(&m_vDistance, XMLoadFloat3(&m_ProjInfo.vPos));
	}

	return S_OK;
}

_bool CProjectile::Tick(_float fTimeDelta)
{
	//PHASE1 이라면.
	if (m_ProjInfo.ePhase == CProjectile::PHASE1)
	{
		if (m_ProjInfo.fLimitY > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			CreateExplosion();
			m_bDestroy = true;
			return true;
		}

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos += XMLoadFloat3(&m_vDir) * m_fSpeed * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
	}
	else if (m_ProjInfo.ePhase == CProjectile::PHASE2 && !m_bReady)
	{
		m_fCurrentTime += fTimeDelta;
		//Offset을 기준으로 회전하다가 일정시간이 지나면 중심부로 이동. 후 발사
		if (m_fCurrentTime >= m_ProjInfo.fDelayTime)
		{
			//중심점으로 이동했다면
			if (XMVector3Equal(XMLoadFloat3(&m_ProjInfo.vOffset), m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			{
				//발솨
				if (!m_bReady)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					CTransform* pTarget = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Transform", 0);
					XMStoreFloat3(&m_vDir, XMVector3Normalize(pTarget->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
					m_ProjInfo.fLimitY = XMVectorGetY(pTarget->Get_State(CTransform::STATE_POSITION));
					RELEASE_INSTANCE(CGameInstance);
					m_bReady = true;
				}
			}
			else
			{
				//중심점으로 이동하지 않았다면 이동
				m_pTransformCom->MoveToWards(XMLoadFloat3(&m_ProjInfo.vOffset), fTimeDelta * 15.f);
			}
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_ProjInfo.vOffset), 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMLoadFloat3(&m_vDistance), m_pTransformCom->Get_WorldMatrix()));

			m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 0.5f);
		}
	}

	if (m_bReady)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = vPos + XMLoadFloat3(&m_vDir) * fTimeDelta * m_fSpeed;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
		if (m_ProjInfo.fLimitY > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			//폭발해야함
			m_bDestroy = true;
			return true;
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	return false;
}

void CProjectile::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom || m_bDestroy)
		return;

	m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif
}

HRESULT CProjectile::Render()
{
	/*if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;*/

		/*CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);*/



	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (_uint i = 0; i < iNumMeshes; ++i)
	//{
	//	if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
	//		return E_FAIL;
	//	/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
	//	return E_FAIL;*/


	//	if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
	//		return E_FAIL;
	//}


	return S_OK;
}

void CProjectile::SetDir(_fvector vDir)
{
	XMStoreFloat3(&m_vDir, vDir);
}

void CProjectile::CreateExplosion()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	for (_uint i = 0; i < 5; ++i)
	{
		/*vector v(rand(-1, 1), rand(-1, 1), rand(-1, 1));
		v.normalize();
		v *= rand(0, radius);*/
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		//rand()%(b-a+1)+a ==> a부터 b의 범위이다.
		_float3 vRandPos = { (_float)(rand() % 3 - 1), (_float)(rand() % 3 - 1) , (_float)(rand() % 3 - 1) };
		XMStoreFloat3(&vRandPos, XMVector3Normalize(XMLoadFloat3(&vRandPos)));
		_float fRadius = (_float)(rand() % 1) + 3.5f;

		vRandPos.x *= fRadius;
		vRandPos.y *= fRadius;
		vRandPos.z *= fRadius;

		XMStoreFloat3(&vRandPos, XMLoadFloat3(&vRandPos) + vPos);

		pGameInstance->Add_GameObjectToLayer(L"Prototype_GameObject_Explosion", LEVEL_GAMEPLAY, L"Effect", &vRandPos);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CProjectile::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CProjectile::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CProjectile::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CProjectile::Ready_Components()
{
	CTransform::TRANSFORMDESC transformDesc;
	ZeroMemory(&transformDesc, sizeof(CTransform::TRANSFORMDESC));
	transformDesc.fRotationPerSec = 5.f;
	transformDesc.fSpeedPerSec = 3.f;
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &transformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;


	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CProjectile * CProjectile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CProjectile*		pInstance = new CProjectile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CProjectile::Clone(void * pArg)
{
	CProjectile*		pInstance = new CProjectile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
