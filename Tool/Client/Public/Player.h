#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
public:
	enum PARTTYPE { PART_WEAPON, PART_END };
	enum STATE_PLAYER { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_ATTACK, STATE_JUMP, STATE_END };
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;
private:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_float					m_fTime = 0.0f;
	_bool					m_bChangeAnm = false;
	STATE_PLAYER			m_eCurrentState = STATE_END;
	STATE_PLAYER			m_ePrevState = STATE_END;
	_bool m_bKeyState[256] = { false };
	_uint i = 0;
private:
	HRESULT Ready_Components();

	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();

	HRESULT Update_Weapon();
public:
	_bool Key_Down(_uchar KeyInput);
	_bool Key_Pressing(_uchar KeyInput);
	void SetState(STATE_PLAYER eState);
	void StateMachine(STATE_PLAYER eState);

public:
	//State
	void Idle_State();
	void Walk_State();
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END