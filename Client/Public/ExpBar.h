#pragma once

#include "UI.h"

BEGIN(Client)

class CExpBar final : public CUI
{
private:
	CExpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CExpBar(const CExpBar& Prototype);
	virtual ~CExpBar() = default;

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
	_uint m_iExp = {};
	_uint m_iMaxExp = {};

public:
	static CExpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END