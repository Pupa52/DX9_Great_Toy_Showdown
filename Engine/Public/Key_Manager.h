#pragma once

#include "Base.h"

BEGIN(Engine)

class CKey_Manager final : public CBase
{
public:
	enum KEYBUFFER { KB_ONE, KB_TWO, KB_THREE, KB_END };

private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	HRESULT Initialize();

public:
	_bool	Key_Pressing(int _iKey, KEYBUFFER keyBuffer = KB_ONE);
	_bool	Key_Down(int _iKey, KEYBUFFER keyBuffer = KB_ONE);
	_bool	Key_Up(int _iKey, KEYBUFFER keyBuffer = KB_ONE);

private:
	_bool	m_bKeyState[KB_END][VK_MAX];

public:
	static CKey_Manager* Create();
	virtual void Free();
};

END