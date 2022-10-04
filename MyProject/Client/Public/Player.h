#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	enum STATE_PLAYER { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_ATTACK, STATE_JUMP, STATE_END };
	enum PLAYER_DIR { DIR_FORWARD, DIR_LEFT, DIR_RIGHT, DIR_BACK, DIR_END};

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
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_float					m_fTime = 0.0f;
	_bool					m_bChangeAnm = false;
	STATE_PLAYER			m_eCurrentState = STATE_END;
	_bool m_bKeyState[256] = { false };
	_bool m_bAnimEnd = false;

	PLAYER_DIR m_eDir = DIR_END;
private:
	HRESULT Ready_Components();
public:
	_bool Key_Down(_uchar KeyInput);
	_bool Key_Pressing(_uchar KeyInput);
	void SetState(STATE_PLAYER eState, _float fTimeDelta);

public:
	//State
	void Idle_State(_float fTimeDelta);
	void Walk_State(_float fTimeDelta);
	void Attack_State(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END