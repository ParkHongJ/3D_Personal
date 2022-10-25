#include "stdafx.h"
#include "..\Public\Yantari.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CYantari::CYantari(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CYantari::CYantari(const CYantari & rhs)
	: CGameObject(rhs)
{
}

HRESULT CYantari::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYantari::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	strcpy_s(m_szName, "Yantari");
	m_Tag = L"Yantari";

	m_eAnimState = IDLE;
	m_pModelCom->Change_Animation(IDLE);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->GetCellPos(0));
	return S_OK;
}

_bool CYantari::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_O))
	{
		m_eAnimState = CYantari::ATTACK1_2;
		m_pModelCom->Change_Animation(ATTACK1_2, 0.25f, false);
	}
	if (pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		m_eAnimState = CYantari::ATTACK2_2;
		m_pModelCom->Change_Animation(ATTACK2_2, 0.25f, false);
	}
	if (pGameInstance->Key_Down(DIK_NUMPAD3))
	{
		m_eAnimState = CYantari::ATTACK3_2;
		m_pModelCom->Change_Animation(ATTACK3_2, 0.25f, false);
	}
	if (pGameInstance->Key_Down(DIK_NUMPAD4))
	{
		m_eAnimState = CYantari::ATTACK4_2;
		m_pModelCom->Change_Animation(ATTACK4_2, 0.25f, false);
	}
	RELEASE_INSTANCE(CGameInstance);
	Set_State(m_eAnimState);
	
	Update_Weapon();

	for (auto& pPart : m_Parts)
	{
		pPart->Tick(fTimeDelta);
	}

	return false;
}

void CYantari::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	//나중에 이거 수정해라
	for (auto& pPart : m_Parts)
	{
		pPart->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
	}


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f);
	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CYantari::Render()
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

void CYantari::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CYantari::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CYantari::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CYantari::Ready_Layer_GameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, void * pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, LEVEL_GAMEPLAY, pLayerTag, pArg)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CYantari::Set_State(ANIM_STATE eState)
{
	switch (eState)
	{
	case CYantari::ATTACK1_2:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_3;
			m_pModelCom->Change_Animation(ATTACK1_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_3:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_4;
			m_pModelCom->Change_Animation(ATTACK1_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_4:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK5_BIS;
			m_pModelCom->Change_Animation(ATTACK5_BIS, 0.f, false);
		}
		break;

	case CYantari::ATTACK5_BIS:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK5_BIS2;
			m_pModelCom->Change_Animation(ATTACK5_BIS2, 0.f, false);
		}
		break;
	case CYantari::ATTACK5_BIS2:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_6;
			m_pModelCom->Change_Animation(ATTACK1_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_6:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_7;
			m_pModelCom->Change_Animation(ATTACK1_7, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_7:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK1_8:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_9;
			m_pModelCom->Change_Animation(ATTACK1_9, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_9:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK2_2:
		m_pModelCom->SetSpeed(ATTACK2_2, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_3;
			m_pModelCom->Change_Animation(ATTACK2_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_3:
		m_pModelCom->SetSpeed(ATTACK2_3, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_4;
			m_pModelCom->Change_Animation(ATTACK2_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_4:
		m_pModelCom->SetSpeed(ATTACK2_4, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_5;
			m_pModelCom->Change_Animation(ATTACK2_5, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_5:
		m_pModelCom->SetSpeed(ATTACK2_5, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_6;
			m_pModelCom->Change_Animation(ATTACK2_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_6:
		m_pModelCom->SetSpeed(ATTACK2_6, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE, 0.f, false);
		}
		break;
		//ATTACK2 END, START 셋트로 한콤보임
	case CYantari::ATTACK2_END:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK2_START:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_END;
			m_pModelCom->Change_Animation(ATTACK2_END, 0.f, false);
		}
		break;
		//ATTACK3 자체가 한콤보
	case CYantari::ATTACK3:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK3_2:
		m_pModelCom->SetSpeed(ATTACK3_2, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_3;
			m_pModelCom->Change_Animation(ATTACK3_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_3:
		m_pModelCom->SetSpeed(ATTACK3_3, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_4;
			m_pModelCom->Change_Animation(ATTACK3_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_4:
		m_pModelCom->SetSpeed(ATTACK3_4, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_5;
			m_pModelCom->Change_Animation(ATTACK3_5, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_5:
		m_pModelCom->SetSpeed(ATTACK3_5, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_6;
			m_pModelCom->Change_Animation(ATTACK3_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_6:
		m_pModelCom->SetSpeed(ATTACK3_6, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_7;
			m_pModelCom->Change_Animation(ATTACK3_7, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_7:
		m_pModelCom->SetSpeed(ATTACK3_7, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_8;
			m_pModelCom->Change_Animation(ATTACK3_8, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_8:
		m_pModelCom->SetSpeed(ATTACK3_8, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK4_2:
		m_pModelCom->SetSpeed(ATTACK4_2, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_3;
			m_pModelCom->Change_Animation(ATTACK4_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_3:
		m_pModelCom->SetSpeed(ATTACK4_3, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_4;
			m_pModelCom->Change_Animation(ATTACK4_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_4:
		m_pModelCom->SetSpeed(ATTACK4_4, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_5;
			m_pModelCom->Change_Animation(ATTACK4_5, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_5:
		m_pModelCom->SetSpeed(ATTACK4_5, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_6;
			m_pModelCom->Change_Animation(ATTACK4_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_6:
		m_pModelCom->SetSpeed(ATTACK4_6, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_7;
			m_pModelCom->Change_Animation(ATTACK4_7, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_7:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::CAST:
		break;
	case CYantari::DEATH:
		break;
	case CYantari::GETUP:
		break;
	case CYantari::HIT:
		break;
	case CYantari::HIT_DEBUT:
		break;
	case CYantari::HIT_FIN:
		break;
	case CYantari::IDLE:
		break;
	case CYantari::POST_CRITIC:
		break;
	case CYantari::WALK_BACK:
		break;
	case CYantari::WALK_FRONT:
		break;
	case CYantari::WALK_LEFT:
		break;
	case CYantari::WALK_RIGHT:
		break;
	default:
		break;
	}
}

HRESULT CYantari::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("Root_Axe_R");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CYantari::Ready_Parts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.YantariWeapon */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_YantariWeapon"));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_Parts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CYantari::Update_Weapon()
{
	/*if (nullptr == m_Sockets[PART_WEAPON])
		return E_FAIL;*/

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	_matrix WeaponMatrix = m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		* m_pTransformCom->Get_WorldMatrix();

	m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);
	return S_OK;
}

HRESULT CYantari::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yantari"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

CYantari * CYantari::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CYantari*		pInstance = new CYantari(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CYantari"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CYantari::Clone(void * pArg)
{
	CYantari*		pInstance = new CYantari(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CYantari"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYantari::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
	{
		Safe_Release(pPart);
	}
	m_Parts.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
