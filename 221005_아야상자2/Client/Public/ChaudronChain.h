#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)

class CChaudronChain final : public CGameObject
{
	enum PARTTYPE { CHAUDRON, CHAIN01, CHAIN02, CHAIN03, CHAIN04, PART_END };
private:
	CChaudronChain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChaudronChain(const CChaudronChain& rhs);
	virtual ~CChaudronChain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta) override;

public:
	void GetDamaged(_float fDamage);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	_bool								m_bAnimEnd = false;
	vector<class CHierarchyNode*>		m_Sockets;
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();
	HRESULT Update_Weapon();

public:
	static CChaudronChain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END