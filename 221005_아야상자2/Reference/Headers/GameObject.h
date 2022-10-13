#pragma	 once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}	
	_bool IsActive() { return m_bActive; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta) {};
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta) {};
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta) {};
	bool CompareTag(const _tchar* _tag) {
		if (m_Tag == _tag)
			return TRUE;
		else return FALSE;
	}

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* 객체에게 추가된 컴포넌트들을 키로 분류하여 보관한다. */
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

protected:
	_float				m_fCamDistance = 0.f;
	_bool				m_bActive = true;
	_bool				m_bDestroy = false;
	char				m_szName[MAX_PATH] = "";
	const _tchar*		m_Tag = nullptr;
protected:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);


public:
	char* GetName() { return m_szName; }
private:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END