#pragma once

#include "UI.h"

BEGIN(Client)

class CReady final : public CUI
{
public:
	typedef struct tagReadyDesc : public UI_DESC
	{
	}READY_DESC;

private:
	CReady(LPDIRECT3DDEVICE9 pGraphic_Device);
	CReady(const CReady& Prototype);
	virtual ~CReady() = default;

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
	virtual HRESULT Reset_RenderState();

private:
	_bool m_bPicking = { false };
	_bool m_bNextLevel = { false };

public:
	static CReady* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END