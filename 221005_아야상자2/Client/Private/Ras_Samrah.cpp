#include "stdafx.h"
#include "..\Public\Ras_Samrah.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Ras_Hands.h"
#include "Ras_Hands2.h"
#include "Ras_Hands3.h"
#include "Cell.h"
#include "Projectile.h"
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
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 10.f, 42.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vOffsetPos;
	XMStoreFloat3(&vOffsetPos, vPos);
	vOffsetPos.y += 10.f;
	vOffsetPos.z -= 30.f;
	
	m_vOffsetPattern = vOffsetPos;

	if (FAILED(Ready_Hands()))
		return E_FAIL;

	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/CellIndex.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;
	while (true)
	{
		//매트릭스 로드
		_uint iIndex = 0;
		ReadFile(hFile, &iIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}
		m_iNaviIndices.push_back(iIndex);
	}
	
	CloseHandle(hFile);
	SetNaviTypes(CCell::CELLTYPE::CANTMOVE);

	hFile = CreateFile(TEXT("../Bin/Data/CellSpawnIndex.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//읽은 바이트 
	dwByte = 0;
	dwStrByte = 0;

	while (true)
	{
		//인덱스 로드
		_uint iIndex = 0;
		ReadFile(hFile, &iIndex, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}
		m_iCylinderIndices.push_back(iIndex);
	}

	CloseHandle(hFile);
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

	//Pattern1 : 망치 휘두르기
	if (pGameInstance->Key_Down(DIK_U))
	{
		m_eCurrentAnimState = Pattern1;
		m_pModelCom->Change_Animation(Pattern1);
	}
	//Pattern3 : 총알발사, 물기둥
	if (pGameInstance->Key_Down(DIK_L))
	{
		m_ePhase = PHASE_2;
		m_eCurrentAnimState = Pattern3;
		m_pModelCom->Change_Animation(Pattern3);
	}
	RELEASE_INSTANCE(CGameInstance);

	if (m_bCylinder)
	{
		m_fCylinderTime += fTimeDelta;
		if (m_fCylinderTime > m_fCylinderTimeMax)
		{
			m_fCylinderTime = 0.0f;
			if (m_iCylinderCount <= m_iCylinderCountMax)
			{
				m_iCylinderCount++; 

				_uint i = rand() % m_iCylinderIndices.size();
				_float3 vPos;
				XMStoreFloat3(&vPos, m_pNavigationCom->GetCellPos(i));
				Ready_Layer_GameObject(L"Prototype_GameObject_Cylinder", L"Layer_Cylinder", &vPos);
			}
			else
			{				
				m_bCylinder = false;
			}
		}
	}
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
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f);
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_Cut", &m_fCut, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DissolveTexture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iPass)))
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

			m_pHand1->Set_Death();
			m_pHand2->Set_Death();
			m_pHand3->Set_Death();

			for (auto& iIndex : m_iNaviIndices)
			{
				m_pNavigationCom->SetCellType(iIndex, CCell::CELLTYPE::CANTMOVE);
			}
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

void CRas_Samrah::SetNaviTypes()
{
	for (auto& iIndex : m_iNaviIndices)
	{
		m_pNavigationCom->SetCellType(iIndex, CCell::CELLTYPE::MOVE);
	}
}

HRESULT CRas_Samrah::Ready_Layer_GameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, void * pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, LEVEL_GAMEPLAY, pLayerTag, pArg)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CRas_Samrah::SetNaviTypes(CCell::CELLTYPE eType)
{
	for (auto& iIndex : m_iNaviIndices)
	{
		m_pNavigationCom->SetCellType(iIndex, eType);
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

HRESULT CRas_Samrah::Ready_Projectile()
{
	CProjectile::ProjectileInfo Projectile;
	ZeroMemory(&Projectile, sizeof(CProjectile::ProjectileInfo));

	Projectile.vOffset = m_vOffsetPattern;
	Projectile.fLimitY = XMVectorGetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
	Projectile.ePhase = CProjectile::PHASE2; //Test

	Projectile.vPos = _float3(5.f,0.f,0.f); //공전의 기준점을 기준으로 어디에 있을건지.
	Projectile.fDelayTime = 2.0f;
	Ready_Layer_GameObject(L"Prototype_GameObject_Projectile", L"Layer_Projectile", &Projectile);

	Projectile.vPos = _float3(0.f, 5.f, 0.f); //공전의 기준점을 기준으로 어디에 있을건지.
	Projectile.fDelayTime = 3.0f;
	Ready_Layer_GameObject(L"Prototype_GameObject_Projectile", L"Layer_Projectile", &Projectile);
	
	Projectile.vPos = _float3(-5.f, 0.f, 0.f); //공전의 기준점을 기준으로 어디에 있을건지.
	Projectile.fDelayTime = 4.0f;
	Ready_Layer_GameObject(L"Prototype_GameObject_Projectile", L"Layer_Projectile", &Projectile);
	
	Projectile.vPos = _float3(0.f, -5.f, 0.f); //공전의 기준점을 기준으로 어디에 있을건지.
	Projectile.fDelayTime = 4.5f;
	Ready_Layer_GameObject(L"Prototype_GameObject_Projectile", L"Layer_Projectile", &Projectile);

	return S_OK;
}

HRESULT CRas_Samrah::Ready_Cylinder()
{
	
	return S_OK;
}

void CRas_Samrah::Set_State(STATE_ANIM eAnim, PHASE ePhase, _float fTimeDelta)
{
	switch (m_ePhase)
	{
	case CRas_Samrah::PHASE_1:
		switch (m_eCurrentAnimState)
		{
		case CRas_Samrah::Idle1:
		{
			//5초에 한번 패턴발동, 랜덤으로 내려치기 발동
			m_fPatternDelay += fTimeDelta;
			if (m_fPatternDelay > m_fPatternMaxDelay)
			{
				m_fPatternDelay = 0.0f;
				m_bHand2 = !m_bHand2;
				if (m_bHand2)
				{
					m_pHand2->Set_Pattern(CRas_Hands2::HAND_PATTERN2);
				}
				else
				{
					m_pHand3->Set_Pattern(CRas_Hands3::HAND_PATTERN3);
				}
				_uint iRand = rand() % 10;
				if (iRand >= 0 && iRand <= 7)
				{
					m_pHand1->Set_Pattern(CRas_Hands::STATE_ANIM::HAND_SLAM_FLY);
				}
			}
		}
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
			m_fCurrentEffectTime += fTimeDelta;
			if (!m_bEffectEnable && m_fCurrentEffectTime > m_fMaxEffectTime)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_float3 vPos = { -14.f, 40.f, 40.f };
				if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Prototype_GameObject_Aspiration", LEVEL_GAMEPLAY, L"Layer_Effect", &vPos)))
					E_FAIL;
				RELEASE_INSTANCE(CGameInstance);
				m_bEffectEnable = true;

			}
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
			if (m_bAnimEnd)
			{
				m_bDestroy = true;
				
				break;
			}
			break;
		case CRas_Samrah::Fly:
			if (m_bAnimEnd)
			{
				m_eCurrentAnimState = Idle2;
				m_pModelCom->Change_Animation(Idle2);
			}
			break;
		case CRas_Samrah::Idle2:
			//2페이즈 Idle
		{
			_uint iRand = rand() % 3;
			switch (iRand)
			{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			default:
				break;
			}
		}
			break;
		case CRas_Samrah::Pattern1:
			break;
		case CRas_Samrah::Pattern3:
			if (m_bAnimEnd)
			{
				Ready_Projectile();

					
				m_bCylinder = true;
				m_eCurrentAnimState = Idle1;
				m_pModelCom->Change_Animation(Idle1);


			}

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

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRas_Samrah::Ready_Hands()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//플레이어 셋팅
	m_pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Transform", 0);
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	Safe_AddRef(m_pTargetTransform);


	//Hand1
	m_pHand1 = (CRas_Hands*)(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_RasHands", L"Com_Transform", 0)->GetOwner());
	Safe_AddRef(m_pHand1);

	m_pHand1->Set_Target(m_pTargetTransform);
	m_pHand1->SetRas_Samrah(m_pTransformCom);

	//Hand2
	m_pHand2 = (CRas_Hands2*)(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_RasHands", L"Com_Transform", 1)->GetOwner());
	Safe_AddRef(m_pHand2);

	m_pHand2->Set_Target(m_pTargetTransform);
	m_pHand2->SetRas_Samrah(m_pTransformCom);
	m_pHand2->Set_OffsetPos(m_pTransformCom);

	//Hand3
	m_pHand3 = (CRas_Hands3*)(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_RasHands", L"Com_Transform", 2)->GetOwner());
	Safe_AddRef(m_pHand3);

	m_pHand3->Set_Target(m_pTargetTransform);
	m_pHand3->SetRas_Samrah(m_pTransformCom);
	m_pHand3->Set_OffsetPos(m_pTransformCom);




	RELEASE_INSTANCE(CGameInstance);
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
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pHand1);
	Safe_Release(m_pHand2);
	Safe_Release(m_pHand3);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
