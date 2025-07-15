#pragma once

#include "UI.h"

BEGIN(Client)

class CMonsterHpBar final : public CUI
{
private:
	CMonsterHpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonsterHpBar(const CMonsterHpBar& Prototype);
	virtual ~CMonsterHpBar() = default;

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

public:
	static CMonsterHpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END