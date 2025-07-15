#pragma once

#include "Item_Possesion.h"

BEGIN(Client)
class CItem_Missile final : public CItem_Possesion
{
private:
	CItem_Missile(CPlayer* pPlayer);
	virtual ~CItem_Missile() = default;

public:
	virtual HRESULT Initialize();
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:
	virtual HRESULT Ready_Component();

private:
	_int m_iBulletCount = { 0 };

public:
	static CItem_Missile* Create(CPlayer* pPlayer);
	virtual void Free() override;

};
END