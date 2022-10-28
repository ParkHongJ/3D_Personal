#include "stdafx.h"
#include "..\Public\Ras_Hands3.h"
#include "GameInstance.h"
#include "Ras_Samrah.h"
#include "Projectile.h"

CRas_Hands3::CRas_Hands3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRas_Hands3::CRas_Hands3(const CRas_Hands3 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRas_Hands3::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRas_Hands3::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	strcpy_s(m_szName, "Ras_Samrah_Hands3");
	m_Tag = L"Ras_Samrah_Hands3";
	
	m_eState = HAND_IDLE;
	m_pModelCom->Set_AnimIndex(HAND_IDLE);
	return S_OK;
}

_bool CRas_Hands3::Tick(_float fTimeDelta)
{
	if (m_bDestroy)
		return true;
	if (!m_bActive)
		return false;
	Set_State(m_eState, fTimeDelta);
	return false;
}

void CRas_Hands3::LateTick(_float fTimeDelta)
{
	if (m_bDestroy)
		return;
	if (!m_bActive)
		return;
	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRas_Hands3::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}

void CRas_Hands3::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CRas_Hands3::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CRas_Hands3::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CRas_Hands3::GetDamaged(_float fDamage)
{
	((CRas_Samrah*)m_pRasTransform->GetOwner())->GetDamaged(fDamage);
}

void CRas_Hands3::Set_State(STATE_ANIM eState, _float fTimeDelta)
{
	switch (eState)
	{
	case CRas_Hands3::HAND_DEATH:
		if (m_bAnimEnd)
		{
			m_bActive = false;
		}
		break;
	case CRas_Hands3::HAND_IDLE:
		break;
	case CRas_Hands3::HAND_PATTERN3:
		m_pModelCom->SetSpeed(HAND_PATTERN3,18.0f);
		//패턴이 끝났다면 IDLE로 돌아감
		if (m_bAnimEnd)
		{
			//3발을 쐈다면
			if (m_iProjectileCount >= m_iProjectileCountMax)
			{
				m_pModelCom->Change_Animation(HAND_IDLE);
				if (m_fCurrentDelay >= m_fDelayMax)
				{
					m_eState = HAND_IDLE;
					m_pModelCom->Change_Animation(HAND_IDLE);
					MoveToOffsetIdle();
					m_iProjectileCount = 0;
					m_fCurrentBulletTime = 0.0f;
					m_fCurrentDelay = 0.0f;
				}
				else
					m_fCurrentDelay += fTimeDelta;
			}
		}
		else
		{
			//3발을 쐈다면
			if (m_iProjectileCount >= m_iProjectileCountMax)
			{
				//m_eState = HAND_IDLE;
				//m_pModelCom->Change_Animation(HAND_IDLE);
				//MoveToOffsetIdle();
				//m_iProjectileCount = 0;
				//m_fCurrentBulletTime = 0.0f;
				m_fCurrentDelay += fTimeDelta;
				break;
			}
			//아니라면 계속 플레이어를 바라보고 투사체를 발사함. 최대 3번
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (nullptr != m_pTarget)
			{
				_vector vTargetPos = m_pTarget->Get_State(CTransform::STATE_POSITION);

				//Look 조절해야함
				_vector vLook = XMVector3Normalize(vTargetPos - vPosition);

				vLook = XMVectorSetY(vLook, 0.0f);

				m_pTransformCom->LookDir(vLook);

				//XMVectorSetY(vLook, 0.0f);
			}

			m_fCurrentBulletTime += 18.0f * fTimeDelta;
			if (m_fCurrentBulletTime > 32.f && m_iProjectileCount < m_iProjectileCountMax)
			{
				//발사횟수를 하나 증가시키고
				m_iProjectileCount++;
				m_fCurrentBulletTime = 0.0f;
				//발사
				CProjectile::ProjectileInfo Projectile;
				ZeroMemory(&Projectile, sizeof(CProjectile::ProjectileInfo));

				_matrix WorldMat = m_pRasTransform->Get_WorldMatrix();
				_vector vPos = XMVector3TransformCoord(XMLoadFloat3(&m_vOffsetAttack), WorldMat);
				vPos += XMLoadFloat3(&m_vOffsetAttack);
				vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 10.f);
				XMStoreFloat3(&Projectile.vPos, vPos);
				XMStoreFloat3(&Projectile.vDir, XMVector3Normalize(m_pTarget->Get_State(CTransform::STATE_POSITION) - vPos));
				Projectile.fLimitY = XMVectorGetY(m_pTarget->Get_State(CTransform::STATE_POSITION));
				Projectile.ePhase = CProjectile::PHASE1; //Test
				if (FAILED(Ready_Layer_GameObject(L"Prototype_GameObject_Projectile", L"Layer_Projectile", &Projectile)))
					return;
			}
		}
		break;
	default:
		break;
	}
}

void CRas_Hands3::Set_Death()
{
	m_eState = HAND_DEATH;
	m_pModelCom->Change_Animation(HAND_DEATH, 0.25f, false);
	m_bAnimEnd = false;
}

void CRas_Hands3::SetRas_Samrah(CTransform * pRasTransform)
{
	m_pRasTransform = pRasTransform;
	Safe_AddRef(m_pRasTransform);
}

void CRas_Hands3::Set_Target(CTransform* pTarget)
{
	//타겟이 없거나 이미 있다면.
	if (nullptr == pTarget || nullptr != m_pTarget)
		return;
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

void CRas_Hands3::Set_Pattern(STATE_ANIM eState)
{
	m_eState = eState;	
	m_pModelCom->Change_Animation(HAND_PATTERN3);
	MoveToOffsetAttack();
}

void CRas_Hands3::Set_OffsetPos(CTransform * pRasTransform)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pRasTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat3(&m_vOffsetPosition, XMVectorSet(-11.f, 20.f, 0.f, 1.f));
	XMStoreFloat3(&m_vOffsetAttack, XMVectorSet(0.f, -3.f, -12.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMLoadFloat3(&m_vOffsetPosition), m_pTransformCom->Get_WorldMatrix()));
	m_pTransformCom->Set_Scale(XMVectorSet(0.4f, 0.4f, 0.4f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, -1.f, 0.f, 0.f), XMConvertToRadians(-90.f));


}

void CRas_Hands3::MoveToOffsetIdle()
{
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	//이전상태는 Attack.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pRasTransform->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMLoadFloat3(&m_vOffsetPosition), m_pTransformCom->Get_WorldMatrix()));
	m_pTransformCom->Set_Scale(XMVectorSet(0.4f, 0.4f, 0.4f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, -1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
}

void CRas_Hands3::MoveToOffsetAttack()
{
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pRasTransform->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(XMLoadFloat3(&m_vOffsetAttack), m_pTransformCom->Get_WorldMatrix()));
	m_pTransformCom->Set_Scale(XMVectorSet(0.4f, 0.4f, 0.4f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
}

HRESULT CRas_Hands3::Ready_Layer_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, LEVEL_GAMEPLAY, pLayerTag, pArg)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CRas_Hands3::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CRas_Hands3 * CRas_Hands3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRas_Hands3*		pInstance = new CRas_Hands3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRas_Hands3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRas_Hands3::Clone(void * pArg)
{
	CRas_Hands3*		pInstance = new CRas_Hands3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRas_Hands3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRas_Hands3::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pRasTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
