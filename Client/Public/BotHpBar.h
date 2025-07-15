#pragma once

#include "UI.h"

BEGIN(Client)

class CBotHpBar final : public CUI
{
public:
	enum class BOT_TYPE { FRIEND = 1, ENEMY = 2, BT_END };

private:
	CBotHpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBotHpBar(const CBotHpBar& Prototype);
	virtual ~CBotHpBar() = default;

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
	class CActor* m_pActor = { nullptr };
	_float m_fCurHpPer = {};
	BOT_TYPE m_eType = { BOT_TYPE::BT_END };

public:
	static CBotHpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
