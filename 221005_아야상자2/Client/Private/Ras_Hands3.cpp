#include "stdafx.h"
#include "..\Public\Ras_Hands3.h"
#include "GameInstance.h"
#include "Ras_Samrah.h"

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
	m_pModelCom->Set_AnimIndex(HAND_IDLE);
	m_pTransformCom->Set_Scale(XMVectorSet(0.3f, 0.3f, 0.3f, 1.f));
	return S_OK;
}

_bool CRas_Hands3::Tick(_float fTimeDelta)
{
	Set_State(m_eState, fTimeDelta);
	return false;
}

void CRas_Hands3::LateTick(_float fTimeDelta)
{
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
		break;
	case CRas_Hands3::HAND_IDLE:
		break;
	case CRas_Hands3::HAND_PATTERN3:
		break;
	default:
		break;
	}
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
	m_pModelCom->Change_Animation(eState, 0.25f, false);
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
