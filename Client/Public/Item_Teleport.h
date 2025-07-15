#pragma once

#include "Item_Possesion.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CItem_Teleport final : public CItem_Possesion
{
private:
	CItem_Teleport(CPlayer* pPlayer);
	virtual ~CItem_Teleport() = default;

public:
	virtual HRESULT Initialize();
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:
	virtual HRESULT Ready_Component();

private:
	CTransform* m_pPlayerTransformCom = { nullptr };

	_float m_fCheckTime = { 0.f };
	_float m_fCoolTime = { 0.f };

public:
	static CItem_Teleport* Create(CPlayer* pPlayer);
	virtual void Free() override;

};
END