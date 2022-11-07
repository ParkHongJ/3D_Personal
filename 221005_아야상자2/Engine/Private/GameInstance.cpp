#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())	
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pCollider_Manager(CCollider_Manager::Get_Instance())
	, m_pKey_Manager(CKey_Manager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pCollider_Manager);
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pPicking);
}


HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 엔진을 구동하기위한 초기화작업을 거치낟. */
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 그래픽디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 입력 초기화. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 초기화. */
	if (FAILED(m_pPicking->Initialize(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, *ppDevice, *ppContext)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Update();

	m_pFrustum->Tick();

	m_pPicking->Tick();
	
	m_pObject_Manager->LateTick(fTimeDelta);

	m_pCollider_Manager->Collision(CCollider_Manager::PLAYER, CCollider_Manager::MONSTER, fTimeDelta);
	m_pCollider_Manager->Clear_CollisionGroup();
	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

map<const _tchar*, class CLayer*>* CGameInstance::GetLayers(_uint iLevelIndex)
{
	return m_pObject_Manager->GetLayers(iLevelIndex);
}

list<class CGameObject*>* CGameInstance::GetLayers(const _tchar * pLayerTag, _uint iLevelIndex)
{
	//m_pObject_Manager->/
	return m_pObject_Manager->GetLayers(pLayerTag, iLevelIndex);
	
}

map<const _tchar*, class CComponent*>* CGameInstance::GetPrototypeComponent(_uint iLevelIndex)
{
	return m_pComponent_Manager->GetPrototypeComponent(iLevelIndex);
}

map<const _tchar*, class CGameObject*>* CGameInstance::GetPrototypeGameObject()
{
	return m_pObject_Manager->GetPrototypeGameObjects();
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();	
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iLayerIndex);	
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);	
}

_bool CGameInstance::IsOverlapComponent(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	//찾는 컴포넌트가 이미 있다면
	if (nullptr != m_pComponent_Manager->Find_Component(iLevelIndex, pPrototypeTag))
		return true;
	
	//없다면 false를 리턴
	return false;
}

HRESULT CGameInstance::Add_CollisionGroup(CCollider_Manager::COLLISIONGROUP eCollisionGroup, CCollider * pGameObject)
{
	m_pCollider_Manager->Add_CollisionGroup(eCollisionGroup, pGameObject);
	return S_OK;
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Update_Timer(pTimerTag);
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);	
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

_bool CGameInstance::Key_Down(_uchar KeyInput)
{
	return m_pKey_Manager->Key_Down(KeyInput);
}

_bool CGameInstance::Key_Pressing(_uchar KeyInput)
{
	return m_pKey_Manager->Key_Pressing(KeyInput);
}

_bool CGameInstance::Key_Up(_uchar KeyInput)
{
	return m_pKey_Manager->Key_Up(KeyInput);
}

_bool CGameInstance::Mouse_Down(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->MouseDown(eMouseKeyID);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);	
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
	
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eTransformState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();	
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);	
}

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRadius);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CKey_Manager::Get_Instance()->Destroy_Instance();

	CCollider_Manager::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();

	CFrustum::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();	
	
	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device); 
	
}
