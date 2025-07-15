#pragma once

#include "UI.h"

BEGIN(Client)

class CBossHpBar final : public CUI
{
private:
	CBossHpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossHpBar(const CBossHpBar& Prototype);
	virtual ~CBossHpBar() = default;

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
	class CPierrot* m_pPierrot = { nullptr };
	_float m_fCurHpPer = {};

public:
	static CBossHpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END