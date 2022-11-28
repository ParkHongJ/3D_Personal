#include "stdafx.h"
#include "..\Public\Particle_Mesh.h"
#include "GameInstance.h"
#include <time.h>
CParticle_Mesh::CParticle_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CParticle_Mesh::CParticle_Mesh(const CParticle_Mesh & rhs)
	: CGameObject(rhs)
{

}

HRESULT CParticle_Mesh::Initialize_Prototype()
{
	srand(_uint(time(NULL)));
	return S_OK;
}

HRESULT CParticle_Mesh::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float3 vPos;
	if (nullptr != pArg)
	{
		memcpy(&vPos, pArg, sizeof(_float3));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	}

	_float3 vRandomPos;
	//double a = random() * 2 * PI
	//double r = R * sqrt(random())

	//// If you need it in Cartesian coordinates
	//double x = r * cos(a)
	//double y = r * sin(a)
	vRandomPos.y = vPos.y + 16.f;
	_float a = (_float)rand() / (_float)RAND_MAX * 2.f * 3.14f;
	_float r = sqrtf((_float)rand() / (_float)RAND_MAX);

	vRandomPos.x = r * cosf(a);/*(((_float)rand() - (_float)rand()) / RAND_MAX) * 15.f;*/
	vRandomPos.z = r * sinf(a); //(((_float)rand() - (_float)rand()) / RAND_MAX) * 15.f;

	XMStoreFloat3(&m_vDir, XMVector3Normalize(XMLoadFloat3(&vRandomPos) - XMLoadFloat3(&vPos)));
	m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));

	return S_OK;
}

_bool CParticle_Mesh::Tick(_float fTimeDelta)
{
	//Jump
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos += XMLoadFloat3(&m_vDir) * fTimeDelta * m_fMoveSpeed;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return false;
}

void CParticle_Mesh::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CParticle_Mesh::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticle_Mesh::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rock01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticle_Mesh::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CParticle_Mesh * CParticle_Mesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Mesh*		pInstance = new CParticle_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CParticle_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Mesh::Clone(void * pArg)
{
	CParticle_Mesh*		pInstance = new CParticle_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CParticle_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Mesh::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
