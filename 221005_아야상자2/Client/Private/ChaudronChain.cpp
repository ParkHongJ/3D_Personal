#include "stdafx.h"
#include "..\Public\ChaudronChain.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "EndChain.h"

CChaudronChain::CChaudronChain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CChaudronChain::CChaudronChain(const CChaudronChain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChaudronChain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChaudronChain::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.7f, 0.7f, 0.7f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -6.f, 30.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180));
	strcpy_s(m_szName, "ChaudronChain");
	m_Tag = L"ChaudronChain";

	return S_OK;
}

_bool CChaudronChain::Tick(_float fTimeDelta)
{
	Update_Weapon();

	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);

	return false;
}

void CChaudronChain::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pPart : m_Parts)
	{
		pPart->LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CChaudronChain::Render()
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

void CChaudronChain::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CChaudronChain::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CChaudronChain::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CChaudronChain::GetDamaged(_float fDamage)
{

}

HRESULT CChaudronChain::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChaudronChain"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
		
	return S_OK;
}

HRESULT CChaudronChain::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pChaudronSocket = m_pModelCom->Get_HierarchyNode("Chaudron");
	if (nullptr == pChaudronSocket)
		return E_FAIL;

	m_Sockets.push_back(pChaudronSocket);
	CHierarchyNode*		pChainSocket01 = m_pModelCom->Get_HierarchyNode("ChainEnd01");
	if (nullptr == pChaudronSocket)
		return E_FAIL;

	m_Sockets.push_back(pChainSocket01);
	CHierarchyNode*		pChainSocket02 = m_pModelCom->Get_HierarchyNode("ChainEnd02");
	if (nullptr == pChainSocket02)
		return E_FAIL;

	m_Sockets.push_back(pChainSocket02);
	CHierarchyNode*		pChainSocket03 = m_pModelCom->Get_HierarchyNode("ChainEnd03");
	if (nullptr == pChainSocket03)
		return E_FAIL;

	m_Sockets.push_back(pChainSocket03);
	CHierarchyNode*		pChainSocket04 = m_pModelCom->Get_HierarchyNode("ChainEnd04");
	if (nullptr == pChainSocket04)
		return E_FAIL;

	m_Sockets.push_back(pChainSocket04);
	return S_OK;
}

HRESULT CChaudronChain::Ready_Parts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Chaudron */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Chaudron"));

	if (nullptr == pGameObject)
	{
		return E_FAIL;
	}

	m_Parts.push_back(pGameObject);

	/* For.Chain */
	for (_uint i = 0; i < 4; ++i)
	{
		CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EndChain"));

		if (nullptr == pGameObject)
		{
			return E_FAIL;
		}

		m_Parts.push_back(pGameObject);
	}
	
	//1번은 항아리.
	//총 4개의 사슬중에 2개만 킴
	((CEndChain*)m_Parts[1])->SetEnable(true);
	((CEndChain*)m_Parts[3])->SetEnable(true);


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CChaudronChain::Update_Weapon()
{

	//_matrix WeaponMatrix = /*m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
	//					   **/ m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
	//	* m_pModelCom->Get_PivotMatrix()
	//	* m_pTransformCom->Get_WorldMatrix();

	//m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	* 뼈의 행렬(CombinedTransformation)
	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	for (_uint i = 0; i < PART_END; ++i)
	{
		if (nullptr == m_Sockets[i])
			return E_FAIL;

		_matrix WeaponMatrix = /*m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
							   **/ m_Sockets[i]->Get_CombinedTransformation()
			* m_pModelCom->Get_PivotMatrix()
			* m_pTransformCom->Get_WorldMatrix();

		m_Parts[i]->SetUp_State(WeaponMatrix);
	}
	return S_OK;
}

CChaudronChain * CChaudronChain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChaudronChain*		pInstance = new CChaudronChain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChaudronChain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChaudronChain::Clone(void * pArg)
{
	CChaudronChain*		pInstance = new CChaudronChain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChaudronChain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaudronChain::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
