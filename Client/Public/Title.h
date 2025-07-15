#pragma once

#include "UI.h"

BEGIN(Client)

class CTitle final : public CUI
{
private:
	CTitle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTitle(const CTitle& Prototype);
	virtual ~CTitle() = default;

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
	static CTitle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END