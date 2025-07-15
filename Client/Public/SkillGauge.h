#pragma once

#include "UI.h"

BEGIN(Client)

class CSkillGauge final : public CUI
{
public:
	enum class SKILLTYPE { GUM = 3, BUBBLE, BOOM, ST_END };

private:
	CSkillGauge(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkillGauge(const CSkillGauge& Prototype);
	virtual ~CSkillGauge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT SetUp_RenderState();

private:
	CPlayer* m_pPlayer = { nullptr };
	_int m_iCharacter = { 0 };
	_float m_fCurPer = { 0.f };

public:
	static CSkillGauge* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END