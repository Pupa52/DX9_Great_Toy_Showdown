#pragma once

#include "UI.h"

BEGIN(Client)

class CHpBar final : public CUI
{
private:
	CHpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHpBar(const CHpBar& Prototype);
	virtual ~CHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components();

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fCurHpPer = {};

public:
	static CHpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END