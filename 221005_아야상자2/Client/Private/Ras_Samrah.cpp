#include "stdafx.h"
#include "..\Public\Ras_Samrah.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Ras_Hands.h"

CRas_Samrah::CRas_Samrah(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRas_Samrah::CRas_Samrah(const CRas_Samrah & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRas_Samrah::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRas_Samrah::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	strcpy_s(m_szName, "Ras_Samrah");
	m_Tag = L"Ras_Samrah";

	m_ePhase = PHASE_1;
	m_eCurrentAnimState = Idle1;
	m_pModelCom->Set_AnimIndex(Idle1);
	m_pTransformCom->Set_Scale(XMVectorSet(0.07f, 0.07f, 0.07f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 10.f, 38.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180));


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	m_pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Transform", 0);
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	Safe_AddRef(m_pTargetTransform);
	
	m_pHand1 = (CRas_Hands*)(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_RasHands", L"Com_Transform", 0)->GetOwner());
	Safe_AddRef(m_pHand1);

	m_pHand1->Set_Target(m_pTargetTransform);
	m_pHand1->SetRas_Samrah(m_pTransformCom);
	RELEASE_INSTANCE(CGameInstance);


	
	
	return S_OK;
}

_bool CRas_Samrah::Tick(_float fTimeDelta)
{
	if (m_bTimeCheck)
	{
		m_fHammerSpawnTime += fTimeDelta;
	}

	Set_State(m_eCurrentAnimState, m_ePhase, fTimeDelta);

	//일정시간마다 손의 이벤트를 만들어야함
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_U))
	{
		m_pHand1->Set_Pattern(CRas_Hands::STATE_ANIM::HAND_SLAM_FLY);
	}
	RELEASE_INSTANCE(CGameInstance);


	Update_Weapon();

	m_Parts->Tick(fTimeDelta);

	return false;
}

void CRas_Samrah::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	//나중에 이거 수정해라
	if (m_bActiveHammer)
	{
		m_Parts->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts);
	}
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRas_Samrah::Render()
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

void CRas_Samrah::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CRas_Samrah::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CRas_Samrah::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CRas_Samrah::GetDamaged(_float fDamage)
{
	m_fHp -= fDamage;
	if (50 >= m_fHp)
	{
		if (m_ePhase != PHASE_2)
		{
			m_ePhase = PHASE_2;
			m_eCurrentAnimState = HitPhase2;
			m_pModelCom->Change_Animation(HitPhase2, 0.0f, false);
			return;
		}
	}

	if (0 >= m_fHp)
	{
		m_fHp = 0.f;
		m_eCurrentAnimState = Death;
		m_pModelCom->Change_Animation(Death);
	}
	else
	{
		if (m_ePhase == PHASE_2)
		{
			//나중에 이거 수정해라
			m_eCurrentAnimState = Jug_FlyHit1;
			m_pModelCom->Change_Animation(Jug_FlyHit1);
		}
	}
}

HRESULT CRas_Samrah::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("Root_Hammer");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CRas_Samrah::Ready_Parts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Hammer */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hammer"));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_Parts = pGameObject;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CRas_Samrah::Set_State(STATE_ANIM eAnim, PHASE ePhase, _float fTimeDelta)
{
	switch (m_ePhase)
	{
	case CRas_Samrah::PHASE_1:
		switch (m_eCurrentAnimState)
		{
		case CRas_Samrah::Death:
			break;
		case CRas_Samrah::Fly:
			break;
		case CRas_Samrah::Idle1:
			break;
		case CRas_Samrah::Pattern1:
			break;
		case CRas_Samrah::Pattern3:
			break;
		case CRas_Samrah::WalkGround:
			break;
		default:
			break;
		}
		break;
	case CRas_Samrah::PHASE_2:
		switch (m_eCurrentAnimState)
		{
		case CRas_Samrah::Jug_FlyHit1:
			if (m_bAnimEnd)
			{
				m_eCurrentAnimState = Idle2;
				m_pModelCom->Change_Animation(Idle2);
			}
			break;
		case CRas_Samrah::Jug_FlyHit2:
			break;
		case CRas_Samrah::HitPhase2:
			if (m_bAnimEnd)
			{
				m_eCurrentAnimState = SpawnHammer;
				m_pModelCom->Change_Animation(SpawnHammer, 0.0f, false);
			}
			break;
		case CRas_Samrah::SpawnHammer:
			m_bTimeCheck = true;
			if (m_fHammerSpawnTime > m_fHammerSpawnMaxTime)
			{
				m_bActiveHammer = true;
			}
			if (m_bAnimEnd)
			{
				m_eCurrentAnimState = Fly;
				m_pModelCom->Change_Animation(Fly, 0.0f, false);
			}
			break;
		case CRas_Samrah::Death:
			break;
		case CRas_Samrah::Fly:
			if (m_bAnimEnd)
			{
				m_eCurrentAnimState = Idle2;
				m_pModelCom->Change_Animation(Idle2);
			}
			break;
		case CRas_Samrah::Idle2:
			break;
		case CRas_Samrah::Pattern1:
			break;
		case CRas_Samrah::Pattern3:
			break;
		case CRas_Samrah::WalkGround:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	
}

HRESULT CRas_Samrah::Update_Weapon()
{
	if (nullptr == m_Sockets[PART_WEAPON])
		return E_FAIL;

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	_matrix WeaponMatrix = m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		* m_pTransformCom->Get_WorldMatrix();

	m_Parts->SetUp_State(WeaponMatrix);
	return S_OK;
}

HRESULT CRas_Samrah::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasSamrah"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CRas_Samrah * CRas_Samrah::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRas_Samrah*		pInstance = new CRas_Samrah(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRas_Samrah"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRas_Samrah::Clone(void * pArg)
{
	CRas_Samrah*		pInstance = new CRas_Samrah(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRas_Samrah"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRas_Samrah::Free()
{
	__super::Free();

	Safe_Release(m_Parts);

	Safe_Release(m_pHand1);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
