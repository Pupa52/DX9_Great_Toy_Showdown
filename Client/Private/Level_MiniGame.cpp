#include "stdafx.h"

#include "Level_MiniGame.h"
#include "Level_Loading.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Player.h"
#include "MiniGame_Bot.h"
#include "TileMgr.h"
#include "TileMap.h"
#include "Sky.h"
#include "Layer.h"
#include "UI.h"

CLevel_MiniGame::CLevel_MiniGame(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID)
	: CLevel(pGraphic_Device)
	, m_ePlayerID(ePlayerID)
{
}

HRESULT CLevel_MiniGame::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	
	m_fOverTime = 60.f;

	return S_OK;
}

void CLevel_MiniGame::Update(_float fTimeDelta)
{
	if (m_fOverTime < 0.f && m_bResult == false)
	{
		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_MINIGAME, TEXT("Layer_TileMgr"));
		CTileMgr* pTileMgr = dynamic_cast<CTileMgr*>(pLayer->Get_Objects().front());

		_uint iCatTile = pTileMgr->Tile_Count(CTile::TILE_MUSEUM_0);
		_uint iDogTile = pTileMgr->Tile_Count(CTile::TILE_PHONE_0);
		_uint iPenguinTile = pTileMgr->Tile_Count(CTile::TILE_MAGIC_0);

		_uint iMaxScore = max(iCatTile, iDogTile);
		iMaxScore = max(iMaxScore, iPenguinTile);

		PLAYER_CHARACTER ePlayer{ PLAYER_CHARACTER::PLAYER_END };

		if (iMaxScore == iCatTile)
			ePlayer = PLAYER_CHARACTER::PLAYER_CAT;
		if (iMaxScore == iDogTile)
			ePlayer = PLAYER_CHARACTER::PLAYER_DOG;
		if (iMaxScore == iPenguinTile)
			ePlayer = PLAYER_CHARACTER::PLAYER_PENGUIN;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_Result_BG"))))
			return;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_Result_Line"))))
			return;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_Result_Char"), (void*)&ePlayer)))
			return;

		m_bResult = true;
	}

	m_fOverTime -= fTimeDelta;
}

HRESULT CLevel_MiniGame::Render()
{
	_float fTime = m_fOverTime <= 0 ? 0 : m_fOverTime;

	_tchar buffer[32];
	swprintf_s(buffer, L"Time %2d", (_uint)fTime);

	m_pGameInstance->Draw_Text(FT_L_LARGE, wstring(buffer), (_uint)(g_iWinSizeX >> 1), 30, D3DCOLOR_ARGB(255, 0, 0, 0));
	m_pGameInstance->Draw_Text(FT_LARGE, wstring(buffer), (_uint)(g_iWinSizeX >> 1), 30, D3DCOLOR_ARGB(255, 255, 255, 255));

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 10.f, -10.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);
	Desc.eLevelID = LEVEL_MINIGAME;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_BackGround()
{
	CTileMap::TILEMAP_DESC tTileMapDesc = {};
	tTileMapDesc.eLevelID = LEVEL_MINIGAME;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_TileMap"), &tTileMapDesc)))
		return E_FAIL;

	_uint TileMapLength = 20;
	CTileMgr::TILEMGR_DESC tTileMgrDesc{ TileMapLength, TileMapLength, LEVEL_MINIGAME };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
		TEXT("Layer_TileMgr"), TEXT("Prototype_GameObject_TileMgr"), (void*)&tTileMgrDesc)))
		return E_FAIL;

	CSky::SKY_DESC tSkyDesc = {};
	tSkyDesc.eLevelID = LEVEL_MINIGAME;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), &tSkyDesc)))
		return E_FAIL;

	_float fOffset = -0.5f;
	for (_uint i = 0; i < TileMapLength + 2; i++)
	{
		_float3 _vPos = { fOffset + i, 0.5f, fOffset };
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
			TEXT("Layer_Wall"), TEXT("Prototype_GameObject_CanMoveCube"), (void*)&_vPos)))
			return E_FAIL;
	}

	for (_uint i = 0; i < TileMapLength + 2; i++)
	{
		_float3 _vPos = { fOffset + i, 0.5f, fOffset + TileMapLength + 1 };
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
			TEXT("Layer_Wall"), TEXT("Prototype_GameObject_CanMoveCube"), (void*)&_vPos)))
			return E_FAIL;
	}

	for (_uint i = 1; i < TileMapLength + 1; i++)
	{
		_float3 _vPos = { fOffset, 0.5f, fOffset + i };
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
			TEXT("Layer_Wall"), TEXT("Prototype_GameObject_CanMoveCube"), (void*)&_vPos)))
			return E_FAIL;
	}

	for (_uint i = 1; i < TileMapLength + 1; i++)
	{
		_float3 _vPos = { fOffset + TileMapLength + 1 , 0.5f, fOffset + i };
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
			TEXT("Layer_Wall"), TEXT("Prototype_GameObject_CanMoveCube"), (void*)&_vPos)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	Desc = {};

	Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_MINIGAME, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_MINIGAME, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	Ready_Player(Desc, _float3(3.f, 1.f, 3.f));
	
	PLAYER_CHARACTER eBot_1 = { PLAYER_CHARACTER::PLAYER_END };
	PLAYER_CHARACTER eBot_2 = { PLAYER_CHARACTER::PLAYER_END };
	
	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		eBot_1 = PLAYER_CHARACTER::PLAYER_DOG;
		eBot_2 = PLAYER_CHARACTER::PLAYER_PENGUIN;
		break;

	case PLAYER_CHARACTER::PLAYER_DOG:
		eBot_1 = PLAYER_CHARACTER::PLAYER_PENGUIN;
		eBot_2 = PLAYER_CHARACTER::PLAYER_CAT;
		break;

	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		eBot_1 = PLAYER_CHARACTER::PLAYER_CAT;
		eBot_2 = PLAYER_CHARACTER::PLAYER_DOG;
		break;
	}
	
	Ready_Bot(Desc, _float3(1.5f, 1.f, 18.5f), FIRST_BOT, eBot_1);
	Ready_Bot(Desc, _float3(18.5f, 1.f, 18.5f), SECOND_BOT, eBot_2);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Player(CLandObject::LANDOBJECT_DESC	Desc, _float3 vSpawnPos)
{
	CPlayer::PLAYER_DESC PlayerDesc = {};
	PlayerDesc.pTerrainTransform = Desc.pTerrainTransform;
	PlayerDesc.pTerrainVIBuffer = Desc.pTerrainVIBuffer;
	PlayerDesc.ePlayerID = m_ePlayerID;
	PlayerDesc.vStartPos = vSpawnPos;
	PlayerDesc.eLevel = LEVEL_MINIGAME;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
		TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &PlayerDesc)))
		return E_FAIL;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_MINIGAME, TEXT("Layer_Player"));
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pPlayer = pLayer->Get_Objects().back();
	CUI::UI_DESC tDesc = { 0.f, 0.f, 0.f, 0.f, pPlayer, LEVEL_MINIGAME, nullptr, 0};
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_BotHpBar"), (void*)&tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Bot(CLandObject::LANDOBJECT_DESC	Desc, _float3 vSpawnPos, Bullet_ID eBulletID, PLAYER_CHARACTER eCharacterID)
{
	CMiniGame_Bot::MINIGAME_BOT_DESC BotDesc = {};
	BotDesc.pTerrainTransform = Desc.pTerrainTransform;
	BotDesc.pTerrainVIBuffer = Desc.pTerrainVIBuffer;
	BotDesc.eBulletID = eBulletID;
	BotDesc.ePlayerID = eCharacterID;
	BotDesc.vStartPos = vSpawnPos;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME,
		TEXT("Layer_MiniGame_Bot"), TEXT("Prototype_GameObject_MiniGame_Bot"), &BotDesc)))
		return E_FAIL;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_MINIGAME, TEXT("Layer_MiniGame_Bot"));
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pBot = pLayer->Get_Objects().back();
	CUI::UI_DESC tDesc = { 0.f, 0.f, 0.f, 0.f, pBot, LEVEL_MINIGAME, nullptr, 0 };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_BotHpBar"), (void*)&tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Lights()
{
	return S_OK;
}

CLevel_MiniGame* CLevel_MiniGame::Create(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID)
{
	CLevel_MiniGame* pInstance = new CLevel_MiniGame(pGraphic_Device, ePlayerID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CLevel_MiniGame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MiniGame::Free()
{
	__super::Free();
}
