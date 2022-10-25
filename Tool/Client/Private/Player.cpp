#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	/*if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;*/
	
	RELEASE_INSTANCE(CGameInstance);
	m_pModelCom->Set_AnimIndex(0);

	m_eCurrentState = CPlayer::STATE_IDLE;
	//m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	
	//StateMachine(m_eCurrentState);

	//Update_Weapon();
	//for (auto& pPart : m_Parts)
	//	pPart->Tick(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	
	m_pModelCom->Play_Animation(fTimeDelta);

//	for (auto& pPart : m_Parts)
//		pPart->LateTick(fTimeDelta);


	for (auto& pPart : m_Parts)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
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

HRESULT CPlayer::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("Bone_sword");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Sword */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_Parts.push_back(pGameObject);

	return S_OK;
}

HRESULT CPlayer::Update_Weapon()
{
	if (nullptr == m_Sockets[PART_WEAPON])
		return E_FAIL;

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	_matrix WeaponMatrix = /*m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
						   **/ m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		* m_pTransformCom->Get_WorldMatrix();

	m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);

	return S_OK;
}

_bool CPlayer::Key_Down(_uchar KeyInput)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!m_bKeyState[KeyInput] && (pGameInstance->Get_DIKState(KeyInput) & 0x80)) {
		m_bKeyState[KeyInput] = true;
		Safe_Release(pGameInstance);
		return true;
	}

	for (_uint i = 0; i < 256; ++i) {
		if (m_bKeyState[i] && !(pGameInstance->Get_DIKState(i) & 0x80)) {
			m_bKeyState[i] = false;
		}
	}

	Safe_Release(pGameInstance);
	return false;
}

_bool CPlayer::Key_Pressing(_uchar KeyInput)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKState(KeyInput) & 0x80) {
		m_bKeyState[KeyInput] = true;
		Safe_Release(pGameInstance);
		return true;
	}

	Safe_Release(pGameInstance);
	return false;
}

void CPlayer::SetState(STATE_PLAYER eState)
{
	if (m_eCurrentState != eState)
	{
		m_eCurrentState = eState;
		switch (m_eCurrentState)
		{
		case Client::CPlayer::STATE_IDLE:
			break;
		case Client::CPlayer::STATE_WALK:
			m_pModelCom->Change_Animation((_uint)m_eCurrentState);
			break;
		case Client::CPlayer::STATE_RUN:
			break;
		case Client::CPlayer::STATE_ATTACK:
			break;
		case Client::CPlayer::STATE_JUMP:
			break;
		case Client::CPlayer::STATE_END:
			break;
		default:
			break;
		}
	}
}

void CPlayer::StateMachine(STATE_PLAYER eState)
{
	switch (eState)
	{
	case Client::CPlayer::STATE_IDLE:
		Idle_State();
		break;
	case Client::CPlayer::STATE_WALK:
		Walk_State();
		break;
	case Client::CPlayer::STATE_RUN:
		break;
	case Client::CPlayer::STATE_ATTACK:
		break;
	case Client::CPlayer::STATE_JUMP:
		break;
	case Client::CPlayer::STATE_END:
		break;
	default:
		break;
	}
}

void CPlayer::Idle_State()
{
	if (Key_Down(DIK_UP))
	{
		m_eCurrentState = CPlayer::STATE_WALK;
	}
	else if (Key_Down(DIK_UP))
	{
		m_eCurrentState = CPlayer::STATE_WALK;
	}
	else if (Key_Down(DIK_UP))
	{
		m_eCurrentState = CPlayer::STATE_WALK;
	}
	else if (Key_Down(DIK_UP))
	{
		m_eCurrentState = CPlayer::STATE_WALK;
	}
}

void CPlayer::Walk_State()
{
	if (Key_Pressing(DIK_UP))
	{
		m_pModelCom->Change_Animation(38);
	}
	else if (Key_Pressing(DIK_DOWN))
	{
		m_pModelCom->Change_Animation(38);
	}
	else if (Key_Pressing(DIK_LEFT))
	{
		m_pModelCom->Change_Animation(38);
	}
	else if (Key_Pressing(DIK_RIGHT))
	{
		m_pModelCom->Change_Animation(38);
	}
	else
	{
		m_pModelCom->Change_Animation(14);
		m_eCurrentState = CPlayer::STATE_IDLE;
	}
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
