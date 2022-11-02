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
	typedef struct CreateUIInfo
	{
		//생성할때 필요한 자료.
		//이름
		//프로토타입 태그
		//레이어 태그
		//레벨
		//모델명
		char szName[260] = "";
		wchar_t pPrototypeTag[260] = L"";
		unsigned int iNumLevel = LEVEL_END;
		wchar_t pTextureTag[260] = L"";

		XMFLOAT4X4 WorldMatrix;
		_uint iPass;
		_float fAlpha;
	}CREATE_UI_INFO;

public:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	void Initialize(const _tchar* pPath = nullptr);
	void SetValue(const _char* pUITag, _float fValue);
	void Tick(_float fTimeDelta);
	void Render();
	HRESULT Load();
private:
	map<_char*, CProgressBar*> m_ProgressBar;

public:
	virtual void Free() override;
};
END
