#pragma once

#include "UI.h"

BEGIN(Client)

class CProgressBar final : public CUI
{
private:
	CProgressBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CProgressBar(const CProgressBar& Prototype);
	virtual ~CProgressBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components();

public:
	static CProgressBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END