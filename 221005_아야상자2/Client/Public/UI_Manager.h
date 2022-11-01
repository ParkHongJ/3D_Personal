#pragma once
#include "Client_Defines.h"
#include "Base.h"

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
	void SetValue(const _char* pUITag, _float fValue);

private:
	map<const _char*, class CProgressBar*> m_ProgressBar;

public:
	virtual void Free() override;
};
END
