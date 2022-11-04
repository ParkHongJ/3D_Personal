#include "stdafx.h"
#include "..\Public\Chaudron.h"
#include "GameInstance.h"

CChaudron::CChaudron(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CChaudron::CChaudron(const CChaudron & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChaudron::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChaudron::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90));
	m_pTransformCom->Set_Scale(XMVectorSet(0.007f, 0.007f, 0.007f, 1.f));
	strcpy_s(m_szName, "Chaudron");
	m_Tag = L"Chaudron";
	return S_OK;
}

_bool CChaudron::Tick(_float fTimeDelta)
{
	return false;
}

void CChaudron::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	//m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CChaudron::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
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

HRESULT CChaudron::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CChaudron::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CChaudron::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CChaudron::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CChaudron::GetDamaged(_float fDamage)
{

}

HRESULT CChaudron::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_ParentTransform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chaudron"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CChaudron * CChaudron::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChaudron*		pInstance = new CChaudron(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChaudron"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChaudron::Clone(void * pArg)
{
	CChaudron*		pInstance = new CChaudron(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChaudron"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaudron::Free()
{
	__super::Free();
	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
