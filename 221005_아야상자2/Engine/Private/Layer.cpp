#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_ComponentPtr(const _tchar * pComponentTag, _uint iLayerIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iLayerIndex; ++i)
		++iter;

	return (*iter)->Get_ComponentPtr(pComponentTag);	
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Initialize()
{

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	list<CGameObject*>::iterator iter = m_GameObjects.begin();

	for (iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if ((*iter)->IsActive())
		{
			_bool bDestroy = (*iter)->Tick(fTimeDelta);
			if (bDestroy) {
				Safe_Release(*iter);
				iter = m_GameObjects.erase(iter);
			}
			else
				iter++;
		}
		else
			iter++;
	}
	//for (auto& pGameObject : m_GameObjects)
	//{
	//	if (nullptr != pGameObject && pGameObject->IsActive())
	//	{
	//		_bool bDestroy = pGameObject->Tick(fTimeDelta);
	//		if (bDestroy) { 
	//			Safe_Release(pGameObject);
	//			iter = m_GameObjects.erase(iter);
	//			//m_GameObjects.remove(iIndex);
	//		}
	//		else
	//			iter++;
	//	}
	//}
}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->LateTick(fTimeDelta);
	}
}

CLayer * CLayer::Create()
{
	CLayer*			pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}


