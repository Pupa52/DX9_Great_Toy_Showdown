#pragma once

#include "UI.h"

BEGIN(Client)

class CMainTitle final : public CUI
{
private:
	CMainTitle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMainTitle(const CMainTitle& Prototype);
	virtual ~CMainTitle() = default;

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
	static CMainTitle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END