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
//#include "Effect.h"
//#include "Sky.h"
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

	//CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	//Safe_AddRef(pGameInstance);
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

	///* For.Prototype_GameObject_Hammer */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
	//	CCamera_Free::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

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

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
	//	return E_FAIL;

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
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 25, 25))))
		return E_FAIL;

	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_ForkLift */
	/*PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/ForkLift/ForkLift.txt", PivotMatrix))))
		return E_FAIL;*/

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Homunculus/Enemy.txt", PivotMatrix))))
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sword */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Weapon/DurgaSword.txt", PivotMatrix))))
		return E_FAIL;

	//RasSamrah
	/* For.Prototype_Component_Model_RasSamrah */
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.45f, 0.45f, 0.45f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RasSamrah"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Ras_Samrah.txt", PivotMatrix))))
		return E_FAIL;

	//Hand1
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Hands/Hand1.dat", PivotMatrix))))
		return E_FAIL;

	//Hand2
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Hands/Hand2.dat", PivotMatrix))))
		return E_FAIL;

	//Hand3
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Hands/Hand3.dat", PivotMatrix))))
		return E_FAIL; 

	//Hammer
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Hammer/Hammer.txt", PivotMatrix))))
		return E_FAIL;

	//ChaudronChain
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChaudronChain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/Chaudron.dat", PivotMatrix))))
		return E_FAIL;

	//Chaudron
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chaudron"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/ChaudronTeapot.dat", PivotMatrix))))
		return E_FAIL;

	//ChaudronEndChain
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_EndChain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/EndChanis.dat", PivotMatrix))))
		return E_FAIL;

	//ChaudronBreakedEndChain
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BreakedEndChain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, L"../Bin/Resources/Meshes/Boss/Chaudron/BreakedEndChanis.dat", PivotMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_ModelInstance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ModelInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODELINSTANCE_DECLARATION::Elements, VTXMODELINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl")))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("충돌체를 로딩중입니다. "));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CAABB::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		COBB::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CSphere::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/NavigationDataTest.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));

	Safe_Release(pGameInstance);

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
