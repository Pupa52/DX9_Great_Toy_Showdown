#pragma once

#include "Item_Possesion.h"

BEGIN(Client)
class CItem_Passive final : public CItem_Possesion
{
private:
	CItem_Passive(class CPlayer* pPlayer, _uint iItemType);
	virtual ~CItem_Passive() = default;

public:
	virtual HRESULT Initialize() override;

private:
	virtual HRESULT Ready_Component();

private:
	_uint m_iItemType = { 0 };

	_uint m_iAttack = { 0 };
	_float m_fSpeed = { 0.f };
	_int m_iBullet = { 0 };

public:
	static CItem_Passive* Create(CPlayer* pPlayer, _uint iItemType);
	virtual void Free() override;
};
END
