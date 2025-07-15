#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID, _bool bReLoading);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Loading();
	HRESULT Loading_Resource_For_LogoLevel();
	HRESULT Loading_Resource_For_LobbyLevel();
	HRESULT Loading_Resource_For_GamePlayLevel();
	HRESULT Loading_Resource_For_MiniGameLevel();

private:
	HRESULT Ready_Layer_BackGround();

private:
	HRESULT Texture_Pierrot();
	HRESULT Texture_Som();
	HRESULT Texture_CupTrio();
	HRESULT Texture_Telephone();
	HRESULT Texture_Police();
	HRESULT Texture_Bird();
	HRESULT Texture_Magic_Mirror();
	HRESULT Texture_Apple();
	HRESULT Texture_Heart();
private:
	HRESULT Texture_Scanner();

private:
	HRESULT Texture_Effect();

private:
	HRESULT Texture_UI();
	HRESULT Prototype_UI();

private:
	HRESULT Prototype_Manager();

private:
	LEVELID				m_eNextLevelID = { LEVEL_END };
	HANDLE				m_hThread = {};
	CRITICAL_SECTION	m_CriticalSection = {};

	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = { false };

	PLAYER_CHARACTER	m_ePlayerID = { PLAYER_CHARACTER::PLAYER_END };

	_bool				m_bReLoading = { false };

public:
	static CLevel_Loading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVELID eNextLevelID, PLAYER_CHARACTER ePlayerID, _bool bReLoading = false);
	virtual void Free() override;
};

END
