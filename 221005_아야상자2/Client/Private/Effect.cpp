#include "stdafx.h"
#include "..\Public\Effect.h"
#include "GameInstance.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}
HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		/*_float3 vPos;
		memcpy(&vPos, pArg, sizeof(_float3));
		vPos.y += 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
*/
		ZeroMemory(&m_tEffectDesc, sizeof(EFFECT_DESC));
		memcpy(&m_tEffectDesc, pArg, sizeof(EFFECT_DESC));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_tEffectDesc.vPosition));
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	//m_pVIBufferCom->SetSize(10.f, 10.f);
	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 0.f));
	strcpy_s(m_szName, "Distortion");
	return S_OK;
}

_bool CEffect::Tick(_float fTimeDelta)
{
	//m_iCurrentTex += fTimeDelta * 24.f;
	//if (m_iCurrentTex >= 16.f)
	//{
	//	m_iCurrentTex = 0.f;
	//	//return true;
	//}
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4 vCamPos = pGameInstance->Get_CamPosition();


	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat4(&vCamPos), 1.f));
	//m_pTransformCom->LookDir(XMLoadFloat4(&vCamPos) - XMLoadFloat3(&vPos));
	RELEASE_INSTANCE(CGameInstance);

	if (m_fTime >= 0.5f)
	{
		m_fTime = 0.0f;
		return true;
	}
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	return false;
}

void CEffect::LateTick(_float fTimeDelta)
{
	m_fTime += fTimeDelta * m_tEffectDesc.eSign;
	m_pShaderCom->Set_RawValue("g_Time", &m_fTime, sizeof(_float));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CEffect::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", pGameInstance->Get_SRV(L"Target_Original"))))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Ready_Components()
{	
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPoint"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect*		pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect*		pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
