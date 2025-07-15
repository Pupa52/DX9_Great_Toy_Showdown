#pragma once

#include "UI.h"

BEGIN(Client)

class CNew final : public CUI
{
public:
	typedef struct taNewDesc : public UI_DESC
	{
	}NEW_DESC;

private:
	CNew(LPDIRECT3DDEVICE9 pGraphic_Device);
	CNew(const CNew& Prototype);
	virtual ~CNew() = default;

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
	static CNew* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END