#pragma once

#include "UI.h"

BEGIN(Client)

class CDodoHead final : public CUI
{
private:
	CDodoHead(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDodoHead(const CDodoHead& Prototype);
	virtual ~CDodoHead() = default;

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
	_float m_fAngle = { 0.f };

public:
	static CDodoHead* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END