#include "stdafx.h"
#include "..\Public\Aspiration.h"
#include "GameInstance.h"

CAspiration::CAspiration(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAspiration::CAspiration(const CAspiration & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAspiration::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAspiration::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	strcpy_s(m_szName, "CAspiration");
	m_Tag = L"CAspiration";

	m_vStartScale = _float3(0.035f, 0.035f, 0.035f);
	m_vMaxScale = _float3(0.04f, 0.04f, 0.04f);

	_float3 vPos;
	memcpy(&vPos, pArg, sizeof(_float3));
	m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 1.f));

	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 5, 0.f, rand() % 5, 1.f));*/
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f,10.f,30.f,1.f));
	return S_OK;
}

_bool CAspiration::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return false;
	if (m_bDestroy)
		return true;

	m_fCurrentTime += fTimeDelta;
	if (m_fCurrentTime >= m_fDuration)
	{
		return true;
	}

	return false;
}

void CAspiration::LateTick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	if (m_bDestroy)
		return;
	if (nullptr == m_pRendererCom)
		return;

	m_fTime += fTimeDelta * 5.f;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CAspiration::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTime, sizeof(_float))))
		return E_FAIL;
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

void CAspiration::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CAspiration::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CAspiration::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CAspiration::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Aspiration01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CAspiration * CAspiration::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAspiration*		pInstance = new CAspiration(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAspiration"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAspiration::Clone(void * pArg)
{
	CAspiration*		pInstance = new CAspiration(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAspiration"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAspiration::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
