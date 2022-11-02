#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "ProgressBar.h"
BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
public:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	void Initialize(const _tchar* pPath = nullptr);
	void SetValue(const _char* pUITag, _float fValue);
	void Tick(_float fTimeDelta);
	void Render();
private:
	map<_char*, CProgressBar*> m_ProgressBar;

public:
	virtual void Free() override;
};
END
