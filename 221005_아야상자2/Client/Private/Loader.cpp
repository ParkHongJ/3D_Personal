#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Monster.h"
#include "Player.h"
#include "ForkLift.h"
#include "Sword.h"
#include "Hammer.h"
#include "Ras_Samrah.h"
#include "ChaudronChain.h"
#include "Chaudron.h"
#include "EndChain.h"
#include "Ras_Hands.h"
#include "Ras_Hands2.h"
#include "Ras_Hands3.h"
#include "StaticObject.h"
#include "Homonculus.h"
#include "Totem.h"
#include "Projectile.h"
#include "Cylinder.h"
#include "Yantari.h"
#include "YantariWeapon.h"
#include "Aspiration.h"
#include "Sky.h"
#include "Hit_Rect.h"
#include "ProgressBar.h"
#include "Explosion.h"
//#include "Effect.h"
//#include "UI.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_YANTARI:
		pLoader->Loading_ForGameYantariLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 개ㅑㄱ체원형 로드한다. */	

	/* For.Prototype_GameObject_BackGround */ 
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;	

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */

	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));	
	/* 모델를 로드한다. */


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	/* 개ㅑㄱ체원형 로드한다. */
	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), 
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	///* For.Prototype_GameObject_UI*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
	//	CUI::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RasSamrah */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasSamrah"),
		CRas_Samrah::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hammer */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hammer"),
		CHammer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Chaudron */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chaudron"),
		CChaudron::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ChaudronChain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChaudronChain"),
		CChaudronChain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ChaudronChain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndChain"),
		CEndChain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ras_Hands */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ras_Hands"),
		CRas_Hands::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Ras_Hands2 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ras_Hands2"),
		CRas_Hands2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Ras_Hands3 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ras_Hands3"),
		CRas_Hands3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_StaticObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticObject"),
		CStaticObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Homunculus */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Homunculus"),
		CHomonculus::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Totem */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Totem"),
		CTotem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Totem */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Projectile"),
		CProjectile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cylinder"),
		CCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Yantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yantari"),
		CYantari::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_YantariWeapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_YantariWeapon"),
		CYantariWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Aspiration */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aspiration"),
		CAspiration::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ProgressBar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ProgressBar"),
		CProgressBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hit_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hit_Effect"),
		CHit_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Explosion*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Explosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */
	/* For.Prototype_Component_Texture_Terrain */
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Noise */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Dissolve/T_NoisyClouds.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BurnNoise */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BurnNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Dissolve/T_Burning_Noise.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HealthBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HealthBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/HealthBar.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UIBackGround */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UIBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/UIBackGround.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Inventory */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Inventory"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/InventoryBackGround.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_StaminaBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_StaminaBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/StaminaBar.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ScreenBlood */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScreenBlood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/ScreenBlood.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ProgressBackGround */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ProgressBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/ProgressBackGround.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ProgressBackGround */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LIFEBAR_BOSS"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ShatterdUIResources/LIFEBAR_BOSS.png"), 1))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 2, 2))))
		return E_FAIL;

	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Homunculus */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Homonculus")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Homonculus"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Homonculus/Homonculus.dat", PivotMatrix))))
			return E_FAIL;
	}

	/* For.Prototype_Component_Model_Fiona */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fiona")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fiona"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", PivotMatrix))))
			return E_FAIL;
	}

	/* For.Prototype_Component_Model_Sword */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Weapon/DurgaSword.txt", PivotMatrix))))
			return E_FAIL;
	}

	//RasSamrah
	/* For.Prototype_Component_Model_RasSamrah */
	PivotMatrix = XMMatrixScaling(0.45f, 0.45f, 0.45f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasSamrah")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasSamrah"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Ras_Samrah.txt", PivotMatrix))))
			return E_FAIL;
	}

	//Hand1
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand1")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand1"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Hands/Hand1.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Hand2
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand2")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand2"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Hands/Hand2.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Hand3
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand3")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand3"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Hands/Hand3.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Hammer
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hammer")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hammer"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Hammer/Hammer.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//ChaudronChain
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChaudronChain")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChaudronChain"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/Chaudron.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//Chaudron
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chaudron")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chaudron"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/ChaudronTeapot.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//ChaudronEndChain
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_EndChain")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_EndChain"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/EndChanis.dat", PivotMatrix))))
			return E_FAIL;
	}

	//ChaudronBreakedEndChain
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BreakedEndChain")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BreakedEndChain"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/BreakedEndChanis.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Cercle_int
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cercle_int")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cercle_int"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Castle/Cercle_int.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Grille_Chaudron
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Grille_Chaudron")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Grille_Chaudron"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Castle/Grille_Chaudron.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Piller
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Piller")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Piller"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Castle/Piller.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Plat
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Plat")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Plat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Castle/Plat.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Prototype_Component_Model_Sky
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sky")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sky"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Sky/SkyDome.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Prototype_Component_Model_Totem
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Totem")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Totem"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Totem.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Prototype_Component_Model_Cylinder
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cylinder")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cylinder"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Effect/Cylinder.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Prototype_Component_Model_Door
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Castle/Door.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//Prototype_Component_Model_Aspiration01
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Aspiration01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Aspiration01"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Effect/Aspiration01.dat", PivotMatrix))))
			return E_FAIL;
	}

	//Explosion
	/* For.Prototype_Component_Model_YantariWeapon */
	if (!pGameInstance->IsOverlapComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Explosion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Explosion"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Effect/Explosion.dat", PivotMatrix, true))))
			return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGameYantariLevel()
{
	lstrcpy(m_szLoadingText, TEXT("Yantari Level"));

	_matrix		PivotMatrix = XMMatrixIdentity();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region Terrain
	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 2, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;
#pragma endregion

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Yantari */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yantari"),
		CYantari::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_YantariWeapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_YantariWeapon"),
		CYantariWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_StaticObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticObject"),
		CStaticObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Player
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fiona")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fiona"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", PivotMatrix))))
			return E_FAIL;
	}

	//PlayerWeapon
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Weapon/DurgaSword.txt", PivotMatrix))))
			return E_FAIL;
	}

	//Yantari
	PivotMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_YANTARI, TEXT("Prototype_Component_Model_Yantari")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_YANTARI, TEXT("Prototype_Component_Model_Yantari"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Yantari/Yantari.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//YantariWeapon
	if (!pGameInstance->IsOverlapComponent(LEVEL_YANTARI, TEXT("Prototype_Component_Model_YantariWeapon")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_YANTARI, TEXT("Prototype_Component_Model_YantariWeapon"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Yantari/YantariWeapon.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//YantariMap
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_YANTARI, TEXT("Prototype_Component_Model_Cube")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_YANTARI, TEXT("Prototype_Component_Model_Cube"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Yantari/Map/Cube.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	//SkyDomeLevel6
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (!pGameInstance->IsOverlapComponent(LEVEL_YANTARI, TEXT("Prototype_Component_Model_SkyDomeLevel6")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_YANTARI, TEXT("Prototype_Component_Model_SkyDomeLevel6"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Sky/SkyDomeLevel6.dat", PivotMatrix, true))))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);


}
