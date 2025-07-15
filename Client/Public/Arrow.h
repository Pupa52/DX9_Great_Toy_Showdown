#pragma once

#include "UI.h"

BEGIN(Client)

class CArrow final : public CUI
{
public:
	typedef struct tagArrowDesc : public UI_DESC
	{
		_bool bLeft;
	}ARROW_DESC;

private:
	CArrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CArrow(const CArrow& Prototype);
	virtual ~CArrow() = default;

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
	_bool m_bLeft = { false };
	_bool m_bNoRender = { false };
	_bool m_bPicking = { false };

	PLAYER_CHARACTER m_eCurrent_Character = { PLAYER_CHARACTER::PLAYER_END };

	_float m_fKeyDownDelay = { 0.f };

public:
	static CArrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END