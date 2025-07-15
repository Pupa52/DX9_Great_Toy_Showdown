#pragma once
#include "stdafx.h"
#include "..\Public\Level_Loddy.h"

#include "Level_Loading.h"

#include "GameInstance.h"

#include "TileMgr.h"
#include "TileMap.h"
#include "Sky.h"

#include "LobbyPlayer.h"
#include "LobbyCamera.h"

#include "Arrow.h"
#include "Ready.h"
#include "../Public/New.h"

CLevel_Loddy::CLevel_Loddy(LPDIRECT3DDEVICE9 pGraphic_Device, _bool bReLoading)
	: CLevel{ pGraphic_Device }
	, m_bReLoading(bReLoading)
{
}

HRESULT CLevel_Loddy::Initialize()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	m_ePlayerCharacter = PLAYER_CHARACTER::PLAYER_CAT;

	m_pGameInstance->PlayBGM(L"BGM_Ingame_Toy Machine_Loop.wav", (_uint)CHANNELID::SOUND_BGM, 1.f);

	return S_OK;
}

void CLevel_Loddy::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000 || true == m_bNextLevel)
	{
		if(m_ePlayerCharacter == PLAYER_CHARACTER::PLAYER_CAT 
			|| m_ePlayerCharacter == PLAYER_CHARACTER::PLAYER_DOG
			|| m_ePlayerCharacter == PLAYER_CHARACTER::PLAYER_PENGUIN)
		{
			if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, 
				CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY,
				m_ePlayerCharacter, m_bReLoading))))
				return;
		}
	}

	if (true == m_bMiniGame)
	{
		if (m_ePlayerCharacter == PLAYER_CHARACTER::PLAYER_CAT
			|| m_ePlayerCharacter == PLAYER_CHARACTER::PLAYER_DOG
			|| m_ePlayerCharacter == PLAYER_CHARACTER::PLAYER_PENGUIN)
		{
			if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING,
				CLevel_Loading::Create(m_pGraphic_Device, LEVEL_MINIGAME,
					m_ePlayerCharacter, m_bReLoading))))
				return;
		}

	}

}

HRESULT CLevel_Loddy::Render()
{
	SetWindowText(g_hWnd, TEXT("로비레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Loddy::Ready_Layer_Camera()
{
	CLobbyCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 10.f, -10.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(35.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_LobbyCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loddy::Ready_Layer_BackGround()
{
	_uint TileMapLength = 40;
	CTileMgr::TILEMGR_DESC tTileMgrDesc{ TileMapLength, TileMapLength, LEVEL_LOBBY };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_TileMgr"), TEXT("Prototype_GameObject_TileMgr"), (void*)&tTileMgrDesc)))
		return E_FAIL;

	CTileMap::TILEMAP_DESC tTileMapDesc = {};
	tTileMapDesc.eLevelID = LEVEL_LOBBY;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_TileMap"), &tTileMapDesc)))
		return E_FAIL;

	CSky::SKY_DESC tSkyDesc = {};
	tSkyDesc.eLevelID = LEVEL_LOBBY;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), &tSkyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loddy::Ready_Layer_Player()
{
	CLobbyPlayer::LOBBYPlAYER_DESC desc = {};

	desc.ePlayerCharacter = PLAYER_CHARACTER::PLAYER_CAT;
	desc.m_Pos = _float3(9.5f, 2.f, 7.5f);
	if(FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY, TEXT("Layer_LobbyPlayer"),
		TEXT("Prototype_GameObject_LobbyPlayer"), &desc)))
		return E_FAIL;

	desc.ePlayerCharacter = PLAYER_CHARACTER::PLAYER_DOG;
	desc.m_Pos = _float3(19.5f, 2.f, 7.5f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY, TEXT("Layer_LobbyPlayer"),
		TEXT("Prototype_GameObject_LobbyPlayer"), &desc)))
		return E_FAIL;

	desc.ePlayerCharacter = PLAYER_CHARACTER::PLAYER_PENGUIN;
	desc.m_Pos = _float3(29.5f, 2.f, 7.5f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY, TEXT("Layer_LobbyPlayer"),
		TEXT("Prototype_GameObject_LobbyPlayer"), &desc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Loddy::Ready_Layer_UI()
{
	CReady::READY_DESC tRDesc{};
	tRDesc.fX = 640;
	tRDesc.fY = 600;
	tRDesc.fSizeX = 514;
	tRDesc.fSizeY = 100;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_Ready"), (void*)&tRDesc)))
		return E_FAIL;

	CNew::NEW_DESC tNDesc{};
	tNDesc.fX = 1000;
	tNDesc.fY = 600;
	tNDesc.fSizeX = 100;
	tNDesc.fSizeY = 80;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_New"), (void*)&tNDesc)))
		return E_FAIL;

	CArrow::ARROW_DESC tADesc{};
	tADesc.fX = 100;
	tADesc.fY = 360;
	tADesc.fSizeX = 100;
	tADesc.fSizeY = 100;
	tADesc.bLeft = true;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_Arrow"), (void*)&tADesc)))
		return E_FAIL;

	tADesc.fX = 1180;
	tADesc.fY = 360;
	tADesc.fSizeX = 100;
	tADesc.fSizeY = 100;
	tADesc.bLeft = false;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_Arrow"), (void*)&tADesc)))
		return E_FAIL;


	// 0629 심현우 커서
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOBBY,
		TEXT("Layer_Cursor"), TEXT("Prototype_GameObject_Cursor"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loddy::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	D3DLIGHT9			LightDesc{};

	/* 빛의 종류. */
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;

	/* L.Diffuse * M.Diffuse * (Shade + (L.Ambient * M.Ambient))*/

	/* 빛의 색 */
	LightDesc.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);

	/* 빛의 범위.*/

	/* 명, 암. */
	LightDesc.Direction = _float3(1.f, -1.f, 1.f);

	/* iIndex번째 광원의 정보를 장치에 저장해놓는다. */
	m_pGraphic_Device->SetLight(0, &LightDesc);

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Position = _float3(19.5f, 7.f, 31.5f);
	LightDesc.Range = 10.f;
	LightDesc.Attenuation1 = .01f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.3f, 0.1f, 0.1f, 1.f);

	m_pGraphic_Device->SetLight(1, &LightDesc);

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Position = _float3(19.5f, 2.f, 7.5f);
	LightDesc.Range = 3.f;
	LightDesc.Attenuation1 = .1f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);

	m_pGraphic_Device->SetLight(2, &LightDesc);

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Position = _float3(9.5f, 2.f, 7.5f);
	LightDesc.Range = 3.f;
	LightDesc.Attenuation1 = .1f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);

	m_pGraphic_Device->SetLight(3, &LightDesc);

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Position = _float3(29.5f, 2.f, 7.5f);
	LightDesc.Range = 3.f;
	LightDesc.Attenuation1 = .1f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);

	m_pGraphic_Device->SetLight(4, &LightDesc);

	/* iIndex번째 광원을 활성화한다. */
	m_pGraphic_Device->LightEnable(0, TRUE);
	m_pGraphic_Device->LightEnable(1, TRUE);
	m_pGraphic_Device->LightEnable(2, TRUE);
	m_pGraphic_Device->LightEnable(3, TRUE);
	m_pGraphic_Device->LightEnable(4, TRUE);

	D3DMATERIAL9		Materials{};
	Materials.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Materials.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_pGraphic_Device->SetMaterial(&Materials);

	return S_OK;
}


CLevel_Loddy* CLevel_Loddy::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool bReLoading)
{
	CLevel_Loddy* pInstance = new CLevel_Loddy(pGraphic_Device, bReLoading);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loddy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loddy::Free()
{
	__super::Free();
	m_pGraphic_Device->LightEnable(0, FALSE);
	m_pGraphic_Device->LightEnable(1, FALSE);
	m_pGraphic_Device->LightEnable(2, FALSE);
	m_pGraphic_Device->LightEnable(3, FALSE);
	m_pGraphic_Device->LightEnable(4, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
}
