#pragma once

#include "Item_Possesion.h"

BEGIN(Client)
class CItem_Mine final : public CItem_Possesion
{
private:
	CItem_Mine(CPlayer* pPlayer);
	virtual ~CItem_Mine() = default;

public:
	virtual HRESULT Initialize();
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:
	virtual HRESULT Ready_Component();

private:
	_float m_fCheckTime = { 0.f };
	_float m_fCoolTime = { 0.f };

public:
	static CItem_Mine* Create(CPlayer* pPlayer);
	virtual void Free() override;

};
END