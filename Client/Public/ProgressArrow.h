#pragma once

#include "UI.h"

BEGIN(Client)

class CProgressArrow final : public CUI
{
private:
	CProgressArrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CProgressArrow(const CProgressArrow& Prototype);
	virtual ~CProgressArrow() = default;

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
	_float m_fMaxRange = {};
	_float m_fMinRange = {};

public:
	static CProgressArrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END