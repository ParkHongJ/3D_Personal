#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentIndex = -1;
	}NAVIGATIONDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool isMove(_fvector vPosition, _float3* vCurrentPosition);
	_float GetHeight(_fvector vTargetPos);
	_vector GetCellPos(_uint iCellIndex);

	void SetCellType(_uint iCellIndex, _uint eCellType);
	void SetCurrentIndex(_uint iIndex)
	{
		//ÇöÀç ¸ðµç¼¿ÀÇ ÀÎµ¦½ºº¸´Ù Å©¸é¾ÈµÊ
		if (m_Cells.size() < iIndex)
		{
			MSG_BOX(L"WrongIndex");
			return;
		}
		m_NavigationDesc.iCurrentIndex = iIndex;
	}
#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

	NAVIGATIONDESC					m_NavigationDesc;

private:
	HRESULT Ready_Neighbor();
	
public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END