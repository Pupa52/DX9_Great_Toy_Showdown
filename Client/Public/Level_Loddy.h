#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CLevel_Loddy final : public CLevel
{
private:
	CLevel_Loddy(LPDIRECT3DDEVICE9 pGraphic_Device, _bool bReLoading);
	virtual ~CLevel_Loddy() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_PlayerCharacter(PLAYER_CHARACTER ePlayerCharacter) { m_ePlayerCharacter = ePlayerCharacter; }
	PLAYER_CHARACTER Get_PlayerCharacter() { return m_ePlayerCharacter; }
	void Set_NextLevel() { m_bNextLevel = true; }
	void Set_MiniGame() { m_bMiniGame = true; }

private:
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Lights();


private:
	PLAYER_CHARACTER m_ePlayerCharacter = { PLAYER_CHARACTER::PLAYER_END };
	_bool m_bReLoading = { false };
	_bool m_bNextLevel = { false };
	_bool m_bMiniGame = { false };

public:
	static CLevel_Loddy* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool bReLoading = {false });
	virtual void Free() override;
};
END
