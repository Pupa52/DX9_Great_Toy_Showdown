#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MiniGame final : public CLevel
{
private:
	CLevel_MiniGame(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID);
	virtual ~CLevel_MiniGame() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_LandObjects();

	HRESULT Ready_Player(CLandObject::LANDOBJECT_DESC Desc, _float3 vSpawnPos);
	HRESULT Ready_Bot(CLandObject::LANDOBJECT_DESC	Desc, _float3 vSpawnPos, Bullet_ID eBulletID, PLAYER_CHARACTER eCharacterID);

	HRESULT Ready_Lights();

private:
	PLAYER_CHARACTER m_ePlayerID = { PLAYER_CHARACTER::PLAYER_END };

	_float m_fOverTime = { 0.f };
	_bool m_bResult = { false };

public:
	static CLevel_MiniGame* Create(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID);
	virtual void Free() override;
};

END
