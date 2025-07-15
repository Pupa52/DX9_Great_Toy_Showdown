#pragma once

#include "UI.h"

BEGIN(Client)

class CPlayerDetail final : public CUI
{
private:
	CPlayerDetail(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayerDetail(const CPlayerDetail& Prototype);
	virtual ~CPlayerDetail() = default;

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
	static CPlayerDetail* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END