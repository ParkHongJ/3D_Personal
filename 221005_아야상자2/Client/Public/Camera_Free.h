#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	_float3 GetNormalizeDir(_uint eState);

	class CTransform* GetTargetTransform();
	HRESULT Set_Player(class CTransform* pPlayerTransform);
	HRESULT Set_Target(class CTransform* pTargetTransform);

	void ReleaseTarget();
	void Shake(_float fTimeDelta);
	void ShakeStart(_float fShakeTime = 0.15f, _float fShakeStrength = 0.25f);
	void SetZoom(_bool bZoom) {
		if (m_bZoom) //�̹� ���̶�� return;
			return;
		m_fOldFov = m_CameraDesc.fFovy;
		m_bZoom = bZoom;
	}
private:
	void Zoom(_float fTimeDelta);

private:
	_bool				m_bDebug = false;
	class CTransform* m_pTargetTransform = nullptr;
	class CTransform* m_pPlayerTransform = nullptr;
	_float3 m_vCamPosition;

	_float3	m_vPivot;

	//Shake
	_float  m_fCurrentShakeTime = 0.0f;
	_float  m_fMaxShakeTime = 0.15f;

	_float  m_fShakeStrength = 0.25f;

	//���� �ֱ�
	_float	m_fAmplitude = 0.f;
	_float  m_fAmplitudeMaxTime = 0.005f;
	_bool	m_bShake = false;

	//Ȯ��
	_bool	m_bZoom = false;
	_float	m_fZoomTime = 0.0f;
	_float  m_fZoomTimeMax = 0.4f;
	_float  m_fOldFov;

	_bool	m_bZoomDelay = false;
	_float	m_fZoomDelayMax = 2.f;
	_int	m_iSign = 1;//������� ��ȣ
public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END