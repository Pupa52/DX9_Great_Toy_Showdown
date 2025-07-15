#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Manager final : public CGameObject
{
public:
	typedef struct tagUIMgrDesc {
		_int iLevelIndex;
		PLAYER_CHARACTER eType;
	}UIMGR_DESC;

private:
	CUI_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Manager(const CUI_Manager& Prototype);
	virtual ~CUI_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Play_Result();

private:
	HRESULT Get_Player();
	HRESULT Print_Font();

	HRESULT Set_HpBar();

private:
	HRESULT Init_UI();
	HRESULT Init_Icon();
	HRESULT Init_GamePlay_UI();
	HRESULT Init_GamePlay_Icon();

private:
	UIMGR_DESC m_tUIMgrDesc = {};

	// 공용 ui
	class CGameObject* m_pObject = { nullptr };
	// player 전용 ui
	class CPlayer* m_pPlayer = { nullptr };
public:
	static CUI_Manager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END