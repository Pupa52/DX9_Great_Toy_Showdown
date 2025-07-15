#pragma once

#include "UI.h"

BEGIN(Client)

class CMiniGameStart final : public CUI
{
private:
	CMiniGameStart(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMiniGameStart(const CMiniGameStart& Prototype);
	virtual ~CMiniGameStart() = default;

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

	PLAYER_CHARACTER m_eCurrent_Character = { PLAYER_CHARACTER::PLAYER_END };

	_float m_fKeyDownDelay = { 0.f };

public:
	static CMiniGameStart* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END