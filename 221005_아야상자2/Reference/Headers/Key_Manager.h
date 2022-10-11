#pragma once
#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CKey_Manager final : public CBase
{
	DECLARE_SINGLETON(CKey_Manager)
private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	_bool Key_Down(_uchar KeyInput);
	_bool Key_Pressing(_uchar KeyInput); // 누르는 동안 한번만 실행
	_bool Key_Up(_uchar KeyInput);
private:
	_bool m_bKeyState[256] = { false };
public:
	virtual void Free() override;
};

END