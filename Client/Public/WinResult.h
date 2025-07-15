#pragma once

#include "UI.h"

BEGIN(Client)

class CWinResult final : public CUI
{
private:
	CWinResult(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWinResult(const CWinResult& Prototype);
	virtual ~CWinResult() = default;

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
	_float m_fDeadTime = { 0.f };
	_float m_fSpeed = { 0.f };

public:
	static CWinResult* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END