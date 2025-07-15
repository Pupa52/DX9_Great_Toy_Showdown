#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();

	HRESULT Ready_LandObjects();

	HRESULT Ready_Layer_Item(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

private:
	HRESULT Ready_Lights();

private:
	PLAYER_CHARACTER m_ePlayerID = { PLAYER_CHARACTER::PLAYER_END };

public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID);
	virtual void Free() override;
};

END
