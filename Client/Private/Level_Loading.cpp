#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Level_Logo.h"
#include "Level_Loddy.h"
#include "Level_GamePlay.h"
#include "Level_MiniGame.h"

#include "GameInstance.h"
#include "MainTitle.h"
#include "Title.h"
#include "Terrain.h"
#include "Obstacle.h"
#include "Player.h"
#include "Camera.h"
#include "Sky.h"
#include "Tile.h"
#include "ExpBar.h"
#include "Minimap.h"
#include "PlayerHpBar.h"
#include "TileMap.h"
#include "ShowDown.h"
#include "Room.h"
#include "HpBar.h"
#include "PlayerDetail.h"
#include "BossHpBar.h"
#include "Loading_Pattern.h"
#include "Icon.h"
#include "MonsterHpBar.h"
#include "Wall.h"
#include "Arrow.h"
#include "Ready.h"
#include "..\Public\New.h"
#include "Range.h"
#include "Storm.h"
#include "SkillGauge.h"
#include "Scanner.h"
#include "Damage.h"
#include "HealKit.h"

#include "UI_Manager.h"
#include "GameManager.h"
#include "WinResult.h"
#include "WinResult_BG.h"
#include "WinResult_Char.h"
#include "ProgressBar.h"
#include "ProgressArrow.h"
#include "Item_Object.h"
#include "Meteor_Bullet.h"
#include "Missile_Bullet.h"
#include "Mine_Bullet.h"

#include "Exp.h"

#include "Effect_Monster_Spawn.h"
#include "Effect_Feather.h"
#include "Effect_Smoke.h"
#include "Effect_LandMine_Count.h"
#include "Effect_LandMine_Max.h"
#include "Effect_Bird_Teleport.h"
#include "Effect_Bird_Light.h"
#include "Effect_Telephone_Count.h"
#include "Effect_Telephone_Max.h"
#include "Effect_Telephone_Explosion_Light.h"
#include "Effect_LandMine_Explosion.h"
#include "Effect_Die_Dust.h"
#include "Effect_Boss_Hit.h"
#include "Effect_Shadow.h"
#include "Effect_Spark_Light.h"
#include "Effect_Impact.h"
#include "Effect_Shield_End.h"
#include "Effect_Burn_Smoke.h"
#include "Effect_Som_Die.h"
#include "Effect_Red_Apple_Die.h"
#include "Effect_Exp_Trail.h"
#include "Effect_Police_Die.h"
#include "Effect_Tile_Smoke.h"
#include "Effect_Scanner_Laser.h"
#include "Effect_Rip.h"
#include "Effect_Level_Up.h"
#include "Effect_Boss_Arrow.h"
#include "Effect_Big_Shadow.h"

#include "Effect_Telephone_Count.h"
#include "Effect_Telephone_Max.h"
#include "Effect_Hit_Effect.h"
#include "Effect_Pierrot_intro.h"
#include "Effect_Meteor_Explosion.h"
#include "Effect_Missile_Boom.h"
#include "Effect_Meteor_Ground.h"
#include "Effect_PlayerMine_Max.h"
#include "Effect_PlayerMine_Count.h"
#include "Effect_Bullet_Fire.h"
#include "Effect_Gum_Skul.h"
#include "Effect_Gum_Big_Splash.h"
#include "Effect_Gum_Small_Splash.h"
#include "Effect_MiniGame_Hit.h"

#include "Effect_Tile_Rect.h"
#include "Effect_Blast.h"
#include "Effect_Vertical_Smoke.h"

#include "Bird_Bullet.h"
#include "Boss_Ball.h"
#include "Boss_Bullet.h"
#include "Boss_PaperPlane.h"
#include "Bullet_Police_Laser.h"

#include "Lego_LandMine.h"
#include "Player_Bullet.h"
#include "Telephone_Meteor_Bullet.h"
#include "CarrotBoom.h"
#include "Shield.h"
#include "GumBoom.h"
#include "Electric_Bolt.h"

#include "Bot.h"
#include "Friend_Bot.h"

#include "LobbyPlayer.h"
#include "LobbyCamera.h"

#include "Pierrot.h"
#include "Monster_Som.h"
#include "Monster_Telephone.h"
#include "Monster_Bird.h"
#include "Monster_Magic_Mirror.h"
#include "Monster_Red_Apple.h"
#include "Monster_Purple_Apple.h"
#include "Monster_Heart.h"
#include "Monster_Police.h"

#include "Monster_Police_Gun.h"
#include "MiniGame_Bot.h"

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID, _bool bReLoading)
	: CLevel { pGraphic_Device }
	, m_ePlayerID(ePlayerID)
	, m_bReLoading(bReLoading)
{
}

/* unsigned (__stdcall* _beginthreadex_proc_type)(void*); */
unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLevel_Loading*		pLoading = static_cast<CLevel_Loading*>(pArg);

	if (FAILED(pLoading->Loading()))
		return 1;

	return 0;
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	m_pGameInstance->PlayBGM(L"BGM_Ingame_Battle_Phase1.wav", (_uint)CHANNELID::SOUND_BGM, 1.f);

	InitializeCriticalSection(&m_CriticalSection);

	/* 추가로 생성한 스레드가 다음레벨에대한 자우너을 로드하낟. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_isFinished)
	{
		HRESULT			hr = {};

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			hr = m_pGameInstance->Change_Level(LEVEL_LOGO, CLevel_Logo::Create(m_pGraphic_Device));
			break;
		case LEVEL_LOBBY:
			hr = m_pGameInstance->Change_Level(LEVEL_LOBBY, CLevel_Loddy::Create(m_pGraphic_Device, m_bReLoading));
			break;
		case LEVEL_GAMEPLAY:
			hr = m_pGameInstance->Change_Level(LEVEL_GAMEPLAY, CLevel_GamePlay::Create(m_pGraphic_Device, m_ePlayerID));
			break;
		case LEVEL_MINIGAME:
			hr = m_pGameInstance->Change_Level(LEVEL_MINIGAME, CLevel_MiniGame::Create(m_pGraphic_Device, m_ePlayerID));
			break;
		}

		if (FAILED(hr))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
	SetWindowText(g_hWnd, m_szLoadingText);

	return S_OK;
}

HRESULT CLevel_Loading::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_Resource_For_LogoLevel();
		break;
	case LEVEL_LOBBY:
		hr = Loading_Resource_For_LobbyLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_Resource_For_GamePlayLevel();
		break;
	case LEVEL_MINIGAME:
		hr = Loading_Resource_For_MiniGameLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_MainTitle */
  	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_MainTitle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Title/main0%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Title"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Title/Title%d.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	/* For. Prototype_GameObject_MainTitle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainTitle"),
		CMainTitle::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Title"),
		CTitle::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_LobbyLevel()
{
	if (false == m_bReLoading)
	{

#pragma region 텍스처
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_Arrow */
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Arrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/ui_arrow%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Ready */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Ready"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/ui_showdown%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Ready */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_New"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/label_new.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Textures/Skybox/Sky_%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Tile/Ground0%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Tile */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Tile"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Tile/More_Tile/Tile_%d.png"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Magic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Magic"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Magic/room_magic_object_%d.png"), 8))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Museum */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Museum"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Museum/room_museum_object_%d.png"), 14))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Phon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Phone"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Phone/room_phone_object_%d.png"), 13))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Pierrot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Pierrot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Pierrot/room_pierrot_object_%d.png"), 9))))
		return E_FAIL;

	// ForRobby
	/* For. Prototype_Component_Texture_Room_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Back/map_back_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Door"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Door/map_door_%d.png"), 11))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Ground */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Ground"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Ground/map_ground_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Robby */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Robby"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Robby/robby_object_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Room_Toy */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Room_Toy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Toy/map_toy_%d.png"), 8))))
		return E_FAIL;

#pragma region 로비 플레이어
	// 고양이--------------------------------------------------------------------
	/* For. Prototype_Component_Texture_LobbyPlayer_Cat_Change_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Change_Character"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Cat/Change/Character/change_%d.png"), 71))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Cat_Change_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Change_Gun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Cat/Change/Gun/change_%d.png"), 71))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Cat_Idle_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Idle_Character"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Cat/Idle/Character/idle_weapon_3_%d.png"), 58))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Cat_Idle_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Idle_Gun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Cat/Idle/Gun/idle_weapon_3_%d.png"), 58))))
		return E_FAIL;

	// 강아지--------------------------------------------------------------------
		/* For. Prototype_Component_Texture_LobbyPlayer_Dog_Change_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Change_Character"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Dog/Change/Character/change_%d.png"), 70))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Dog_Change_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Change_Gun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Dog/Change/Gun/change_%d.png"), 70))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Dog_Idle_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Idle_Character"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Dog/Idle/Character/idle_weapon_3_%d.png"), 59))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Dog_Idle_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Idle_Gun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Dog/Idle/Gun/idle_weapon_3_%d.png"), 59))))
		return E_FAIL;


	// 펭귄--------------------------------------------------------------------
	/* For. Prototype_Component_Texture_LobbyPlayer_Penguin_Change_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Change_Character"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Penguin/Change/Character/change_%d.png"), 72))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Penguin_Change_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Change_Gun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Penguin/Change/Gun/change_%d.png"), 72))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Penguin_Idle_Character */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Idle_Character"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Penguin/Idle/Character/idle_weapon_3_%d.png"), 59))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LobbyPlayer_Penguin_Idle_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Idle_Gun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/LobbyPlayer/Penguin/Idle/Gun/idle_weapon_3_%d.png"), 59))))
		return E_FAIL;
#pragma endregion


#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 40, 40))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


		lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
		/* For. Prototype_GameObject_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Room */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Room"),
			CRoom::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Wall */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall"),
			CWall::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_LobbyPlayer */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LobbyPlayer"),
			CLobbyPlayer::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_TileMap */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TileMap"),
			CTileMap::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Sky */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
			CSky::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_LobbyCamera */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LobbyCamera"),
			CLobbyCamera::Create(m_pGraphic_Device))))
			return E_FAIL;
	

	/* For. Prototype_GameObject_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Arrow"),
		CArrow::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Ready */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ready"),
		CReady::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_New */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_New"),
		CNew::Create(m_pGraphic_Device))))
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_GamePlayLevel()
{
	if (false == m_bReLoading)
	{
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

		if (FAILED(Texture_UI()))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Room_Back */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Room_Back"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Back/map_back_%d.png"), 30))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Room_Magic */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Room_Magic"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Magic/room_magic_object_%d.png"), 8))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Room_Museum */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Room_Museum"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Museum/room_museum_object_%d.png"), 14))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Room_Phon */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Room_Phone"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Phone/room_phone_object_%d.png"), 13))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Room_Pierrot */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Room_Pierrot"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Room/Pierrot/room_pierrot_object_%d.png"), 9))))
			return E_FAIL;


#pragma region Map

		/* For. Prototype_Component_Texture_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Tile/Ground0%d.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Tile */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Tile/More_Tile/Tile_%d.png"), 9))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Obstacle */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Obstacle"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Obstacle/fence_toy_S_%d.png"), 28))))
			return E_FAIL;


#pragma endregion Map

		/* For. Prototype_Component_Texture_Sky */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Textures/Skybox/Sky_%d.dds"), 4))))
			return E_FAIL;

#pragma region 몬스터
		if (FAILED(Texture_Pierrot()))
			return E_FAIL;

		if (FAILED(Texture_Som()))
			return E_FAIL;

		if (FAILED(Texture_Telephone()))
			return E_FAIL;

		if (FAILED(Texture_Bird()))
			return E_FAIL;
		
		if (FAILED(Texture_Magic_Mirror()))
				return E_FAIL;

		if (FAILED(Texture_Police()))
			return E_FAIL;

		if (FAILED(Texture_Apple()))
			return E_FAIL;

		if (FAILED(Texture_Heart()))
			return E_FAIL;
#pragma endregion 몬스터

#pragma region 스캐너
		if (FAILED(Texture_Scanner()))
			return E_FAIL;
#pragma endregion 스캐너

#pragma region 이펙트
#pragma region 몬스터 죽으면 먼지 나오는 이펙트
		/* For. Prototype_GameObject_Die_Dust_Effect*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Die_Dust_Effect"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Die_Dust_Effect/Die_Dust_Effect_%d.png"), 15))))
			return E_FAIL;
#pragma endregion 몬스터 죽으면 먼지 나오는 이펙트


#pragma region 보스맞는 이펙트
		/* For. Prototype_GameObject_Bullet_Boss_Hit_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet_Boss_Hit_Effect"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Boss_Hit/Boss_Hit_%d.png"), 14))))
			return E_FAIL;

#pragma endregion 보스맞는 이펙트

#pragma region 그림자
		/* For. Prototype_Component_Texture_Shadow*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shadow"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Shadow/Shadow%d.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Big_Shadow*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Big_Shadow"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Shadow/Big_Shadow%d.png"), 1))))
			return E_FAIL;

		if (FAILED(Texture_Effect()))
			return E_FAIL;
#pragma endregion 그림자

#pragma region 메테오 폭발
		/* For. Prototype_Component_Texture_Meteor_Explosion */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Meteor_Explosion"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Meteor_Explosion/Meteor_Explosion_%d.png"), 16))))
			return E_FAIL;
#pragma endregion

#pragma region 메테오 바닥
		/* For. Prototype_Component_Texture_Meteor_Ground */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Meteor_Ground"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Meteor_Ground/Meteor_Ground_%d.png"), 16))))
			return E_FAIL;
#pragma endregion

#pragma region 미사일 폭발
		/* For. Prototype_Component_Texture_Missile_Boom */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Missile_Boom"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Missile_Boom/Missile_Boom_%d.png"), 12))))
			return E_FAIL;
#pragma endregion

#pragma endregion 이펙트

#pragma region 아이템
		/* For. Prototype_GameObject_Item_Healkit */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_Healkit"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Healkit/item_box_0_%d.png"), 2))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Magazine */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Magazine"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Magazine.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Meteor */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Meteor"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Meteor.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Mine */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Mine"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Mine.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Missile */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Missile"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Missile.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Skate */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Skate"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Skate.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Sword */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Sword"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Sword.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Item_Teleport */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Teleport"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Teleport.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Missile_Bullet */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Missile_Bullet"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Missile_Bullet/Item_Missile.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Meteor_Bullet */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Meteor_Bullet"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Meteor_Bullet/Item_Meteor.png"), 1))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Exp */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Exp"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/item/Item_Exp/exp_%d.png"), 3))))
			return E_FAIL;
		
		/* For. Prototype_Component_Texture_Storm */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Storm"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Storm/Storm%d.png"), 1))))
			return E_FAIL;

#pragma endregion 아이템
		/* For. Prototype_Component_Boss_Balls */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Boss_Balls "),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Boss_Ball/Boss_Ball_%d.png"), 3))))
			return E_FAIL;

		/* For. Prototype_GameObject_Card_Bullet */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Card_Bullet"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Card_Bullet/Card_Bullet_Y%d.png"), 5))))
			return E_FAIL;

		/* For. Prototype_GameObject_Bird_Bullet */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bird_Bullet"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Bird_Bullet/Bird_Bullet%d.png"), 1))))
			return E_FAIL;
	
		/* For. Prototype_GameObject_PaperPlane */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PaperPlane"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/PaperPlane/PaperPlane_Temp%d.png"), 3))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Laser */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Laser"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Laser/Laser%d.png"), 1))))
			return E_FAIL;

		/* For. Prototype_GameObject_Lego_LandMine */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Lego_LandMine"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Lego/lego%d.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Drop_Telephone"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Drop_Telephone/Tex_phone%d.png"), 1))))
			return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
		/* For. Prototype_Component_VIBuffer_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
			CVIBuffer_Terrain::Create(m_pGraphic_Device, 144, 144))))
			return E_FAIL;

		lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

		lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

		/* For. Prototype_GameObject_Bot */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bot"),
			CBot::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Friend_Bot */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Friend_Bot"),
			CFriend_Bot::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Pierrot */
  		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Pierrot"),
			CPierrot::Create(m_pGraphic_Device))))
			return E_FAIL; 

		/* For. Prototype_GameObject_Monster_Apple_Red */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Heart"),
			CMonster_Heart::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Apple_Red */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Apple_Red"),
			CMonster_Red_Apple::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Apple_Purple */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Apple_Purple"),
			CMonster_Purple_Apple::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Police */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Police"),
			CMonster_Police::Create(m_pGraphic_Device))))
			return E_FAIL;

		///* For. Prototype_GameObject_Monster_Police_First_Spine */
		//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Police_First_Spine"),
		//	CMonster_Police_First_Spine::Create(m_pGraphic_Device))))
		//	return E_FAIL;

		/* For. Prototype_GameObject_Monster_Police_Gun */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Police_Gun"),
			CMonster_Police_Gun::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Som */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Som"),
			CMonster_Som::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Telephone */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Telephone"),
			CMonster_Telephone::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Bird */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Bird"),
			CMonster_Bird::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Magic_Mirror */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Magic_Mirror"),
			CMonster_Magic_mirror::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Boss_Bullet */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Bullet"),
			CBoss_Bullet::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Boss_Ball */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Ball"),
			CBoss_Ball::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Boss_PaperPlane */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_PaperPlane"),
			CBoss_PaperPlane::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_LandMine */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LandMine"),
			CLego_LandMine::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Telephone_Meteor */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Telephone_Meteor"),
			CTelephone_Meteor_Bullet::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Bird_Bullet */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bird_Bullet"),
			CBird_Bullet::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Boss_Arrow */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Arrow"),
			CEffect_Boss_Arrow::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Police_Laser */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Police_Laser"),
			CBullet_Police_Laser::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Police_Laser */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rip"),
			CEffect_Rip::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Monster_Spawn_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Spawn_Effect"),
			CEFFECT_Monster_Spawn::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Scanner_Laser */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Scanner_Laser"),
			CEffect_Scanner_Laser::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Feather_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Feather_Effect"),
			CEffect_Feather::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Die_Dust_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Die_Dust_Effect"),
			CEffect_Die_Dust::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Som_Die */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Som_Die"),
			CEffect_Som_Die::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Red_Apple_Die */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Red_Apple_Die"),
			CEffect_Red_Apple_Die::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Exp_Trail */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Exp_Trail"),
			CEffect_Exp_Trail::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Smoke_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Smoke_Effect"),
			CEffect_Smoke::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Police_Die */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Police_Die"),
			CEffect_Police_Die::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Burn_Smoke */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Burn_Smoke"),
			CEffect_Burn_Smoke::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Scanner */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Scanner"),
			CScanner::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Tile_Smoke*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tile_Smoke"),
			CEffect_Tile_Smoke::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Shield_End*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Shield_End"),
			CEffect_Shield_End::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Shadow */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shadow"),
			CEffect_Shadow::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Spark_Light */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spark_Light"),
			CEffect_Spark_Light::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Impact */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Impact"),
			CEffect_Impact::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Big_Shadow*/ 
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Big_Shadow"),
			CEffect_Big_Shadow::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_LandMine_Count_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LandMine_Count_Effect"),
			CEFFECT_LandMine_Count::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_LandMine_Count_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Level_Up_Paint"),
			CEffect_Level_Up::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_LandMine_Max_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LandMine_Max_Effect"),
			CEFFECT_LandMine_Max::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Bird_Teleport_Effect2 */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bird_Teleport_Effect2"),
			CEffect_Bird_Teleport::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Bird_Teleport_Light_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bird_Teleport_Light_Effect"),
			CEffect_Bird_Light::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Telephone_Count_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Telephone_Count_Effect"),
			CEffect_Telephone_Count::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Telephone_Max_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Telephone_Max_Effect"),
			CEffect_Telephone_Max::Create(m_pGraphic_Device))))
			return E_FAIL;
		
		/* For. Prototype_GameObject_Player_Bullet_Hit_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"),
			CEffect_Hit_Effect::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Blast */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Blast"),
			CEffect_Blast::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Vertical_Smoke */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Vertical_Smoke"),
			CEffect_Vertical_Smoke::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Pierrot_intro_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pierrot_intro_Effect"),
			CEffect_Pierrot_intro::Create(m_pGraphic_Device))))
			return E_FAIL;



		/* For. Prototype_GameObject_PlayerMine_Max*/
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerMine_Max"),
			CEFFECT_PlayerMine_Max::Create(m_pGraphic_Device))))
			return E_FAIL;
		
		/* For. Prototype_GameObject_PlayerMine_Count */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerMine_Count"),
			CEFFECT_PlayerMine_Count::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Boss_Hit_Effect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Hit_Effect"),
			CEffect_Boss_Hit::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Meteor_Explosion */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Meteor_Explosion"),
			CEffect_Meteor_Explosion::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Missile_Boom */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Missile_Boom"),
			CEffect_Missile_Boom::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Effect_Meteor_Ground */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Meteor_Ground"),
			CEffect_Meteor_Ground::Create(m_pGraphic_Device))))
			return E_FAIL;


		/* For. Prototype_GameObject_Obstacle */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Obstacle"),
			CObstacle::Create(m_pGraphic_Device))))
			return E_FAIL;

		if (FAILED(Prototype_UI()))
			return E_FAIL;

#pragma region 아이템 (+ 아이템 총알)
		
		/* For. Prototype_GameObject_ItemObject */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemObject"),
			CItem_Object::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_MeteorBullet */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MeteorBullet"),
			CMeteor_Bullet::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_MineBullet */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MineBullet"),
			CMine_Bullet::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_MissileBullet */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissileBullet"),
			CMissile_Bullet::Create(m_pGraphic_Device))))
			return E_FAIL;
#pragma region 아이템

		/* For. Prototype_GameObject_Exp */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Exp"),
			CExp::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Healkit */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Healkit"),
			CHealkit::Create(m_pGraphic_Device))))
			return E_FAIL;

		/* For. Prototype_GameObject_Storm */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Storm"),
			CStorm::Create(m_pGraphic_Device))))
			return E_FAIL;

#pragma endregion
		/* For. Prototype_GameObject_Effect_Tile_Rect */
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Tile_Rect"),
			CEffect_Tile_Rect::Create(m_pGraphic_Device))))
			return E_FAIL;

		if (FAILED(Prototype_Manager()))
			return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_MiniGameLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스처을(를) 로딩중입니다."));
	/* For. Prototype_Component_Texture_Tile */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Tile"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Tile/More_Tile/Tile_%d.png"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Textures/Skybox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Effect_Blast*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Effect_Blast"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Blast/Effect_Blast_%d.png"), 16))))
		return E_FAIL;

#pragma region Result
	/* For. Prototype_Component_Texture_Result_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Result_BackGround"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/ui_result_win_bg%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Result_Line"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/ui_result_win_line%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Dog */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Result_Dog"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/WinIdle/Dog/win_idle_%d.png"), 112))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Cat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Result_Cat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/WinIdle/Cat/win_idle_%d.png"), 111))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Penguin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Result_Penguin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/WinIdle/Penguin/win_idle_%d.png"), 112))))
		return E_FAIL;
#pragma endregion Result


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 20, 20))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체 원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_MiniGame_Bot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGame_Bot"),
		CMiniGame_Bot::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MiniGame_Hit */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGame_Hit"),
		CEffect_MiniGame_Hit::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING,
		TEXT("Layer_Loading"), TEXT("Prototype_GameObject_Loading_Background"))))
		return E_FAIL;

	for (_uint i = 0; i < 8; i++)
	{
		for (_uint j = 0; j < 8; j++)
		{
			CLoading_Pattern::PATTERN_DESC tDesc{};
			tDesc.fX = j * 200.f + 25.f;
			tDesc.fY = i * 200.f + 65.f;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING,
				TEXT("Layer_Loading"), TEXT("Prototype_GameObject_Loading_Pattern"),(void*)&tDesc)))
				return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING,
		TEXT("Layer_Loading"), TEXT("Prototype_GameObject_Loading_Icon"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING,
		TEXT("Layer_Cursor"), TEXT("Prototype_GameObject_Cursor"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Loading::Texture_Pierrot()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Ghost_idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/ghost_idle/boss_pierrot_ghost_idle_%d.png"), 30)))) // 줄임
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Transform"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/transform/boss_pierrot_transform_%d.png"), 23))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/idle/boss_pierrot_boss_idle_%d.png"), 30)))) // 줄임
		return E_FAIL;

	//cardMagic

	/* For. Prototype_Component_Texture_Pierrot_End */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/cardMagic/end/boss_pierrot_cardMagic_end_%d.png"), 29))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_Pierrot_Ready */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Ready"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/cardMagic/ready/boss_pierrot_cardMagic_ready_%d.png"), 38))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_Pierrot_Shuffle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Shuffle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/cardMagic/shuffle/boss_pierrot_cardMagic_shuffle_%d.png"), 23)))) // 줄임
		return E_FAIL;
	/* For. Prototype_Component_Texture_Pierrot_Throw */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Throw"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/cardMagic/throw/boss_pierrot_cardMagic_throw_%d.png"), 27)))) // 줄임
		return E_FAIL;
	// 비행기

	/* For. Prototype_Component_Texture_Pierrot_paperPlaneMagic_end */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_end"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/paperPlaneMagic/end/boss_pierrot_paperPlaneMagic_end_%d.png"), 39))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_Pierrot_paperPlaneMagic_ready */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_ready"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/paperPlaneMagic/ready/boss_pierrot_paperPlaneMagic_ready_%d.png"), 43))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_Pierrot_paperPlaneMagic_rest */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_rest"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/paperPlaneMagic/rest/boss_pierrot_paperPlaneMagic_rest_%d.png"), 29)))) // 줄임
		return E_FAIL;
	/* For. Prototype_Component_Texture_Pierrot_paperPlaneMagic_summon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_summon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/paperPlaneMagic/summon/boss_pierrot_paperPlaneMagic_summon_%d.png"), 39))))
		return E_FAIL;

	// 저글링

	/* For. Prototype_Component_Texture_Pierrot_paperPlaneMagic_ready */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Juggling_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/juggling/end/boss_pierrot_juggling_end_%d.png"), 26))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Pierrot_Juggling_Juggling */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Juggling_Juggling"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/juggling/juggling/boss_pierrot_juggling_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Pierrot_Juggling_Ready */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Juggling_Ready"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/juggling/ready/boss_pierrot_juggling_ready_%d.png"), 38))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_PaperPlane */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PaperPlane"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/PaperPlane/PaperPlane_Temp%d.png"), 3))))
		return E_FAIL;

	// 점프, 랜딩

	/* For. Prototype_Component_Texture_Pierrot_Jump_Start */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Jump_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/teleport/move/boss_pierrot_teleport_move_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Pierrot_Jump_End */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Jump_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Pierrot/teleport/smash/boss_pierrot_teleport_smash_%d.png"), 19))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Som()
{
	/* For. Prototype_Component_Texture_Monster_Som_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Som_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_som/monster_dirtysom_Move/monster_dirtysom_move_%d.png"), 16))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Som_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Som_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_som/monster_dirtysom_Die/monster_dirtysom_die_%d.png"), 19))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_CupTrio()
{
	/* For. Prototype_Component_Texture_Monster_Cuptrio_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Cuptrio_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_CupTrio/monster_cuptrio_die_Sequence/monster_cuptrio_die_%d.png"), 55))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_Monster_Cuptrio_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Cuptrio_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_CupTrio/monster_cuptrio_move_Sequence/monster_cuptrio_move_%d.png"), 38))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Telephone()
{
	/* For. Prototype_Component_Texture_Monster_Telephone_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Telephone/bg_map_object_artifact_1_1idle_Sequence/bg_map_object_artifact_1_1idle_%d.png"), 20))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Telephone_Active */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_Active"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Telephone/bg_map_object_artifact_1_2active_Sequence/bg_map_object_artifact_1_2active_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Telephone_End */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Telephone/bg_map_object_artifact_1_3end_Sequence/bg_map_object_artifact_1_3end_%d.png"), 44))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Telephone_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Telephone/bg_map_object_artifact_1_4die_Sequence/bg_map_object_artifact_1_4die_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Lego_LandMine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Lego_LandMine"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Lego/lego%d.png"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Police()
{

		/* For. Prototype_Component_Texture_Monster_Police_Die*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Die"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Body/monster_museum_0_die_Sequence/monster_museum_0_die_%d.png"), 42))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Idle */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Idle"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Body/monster_museum_0_idle_Sequence/monster_museum_0_idle_%d.png"), 38))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Move*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Move"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Body/monster_museum_0_move_Sequence/monster_museum_0_move_%d.png"), 37))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Shot_Back*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Shot_Back"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Body/monster_museum_0_shot_back_Sequence/monster_museum_0_shot_back_%d.png"), 57))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Shot */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Shot"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Body/monster_museum_0_shot_Sequence/monster_museum_0_shot_%d.png"), 79))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Shot */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Aim"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Body/monster_museum_0_aim_Sequence/monster_museum_0_aim_%d.png"), 1))))
			return E_FAIL;


		/* For. Prototype_Component_Texture_Monster_Police_First_Spine_Die*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Die"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/First_Spine/monster_museum_0_die_Sequence/monster_museum_0_die_%d.png"), 42))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_First_Spine_Idle*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Idle"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/First_Spine/monster_museum_0_idle_Sequence/monster_museum_0_idle_%d.png"), 38))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_First_Spine_Move*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Move"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/First_Spine/monster_museum_0_move_Sequence/monster_museum_0_move_%d.png"), 37))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_First_Spine_Shot_Back*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Shot_Back"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/First_Spine/monster_museum_0_shot_back_Sequence/monster_museum_0_shot_back_%d.png"), 57))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_First_Spine_Shot*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Shot"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/First_Spine/monster_museum_0_shot_Sequence/monster_museum_0_shot_%d.png"), 79))))
			return E_FAIL;


		/* For. Prototype_Component_Texture_Monster_Police_Gun_Die*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Die"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Gun/monster_museum_0_die_Sequence/monster_museum_0_die_%d.png"), 42))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Gun_Idle*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Idle"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Gun/monster_museum_0_idle_Sequence/monster_museum_0_idle_%d.png"), 38))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Gun_Move*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Move"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Gun/monster_museum_0_move_Sequence/monster_museum_0_move_%d.png"), 37))))
			return E_FAIL;

		/* For. Prototoype_Component_Texture_Monster_Police_Gun_Shot_Back*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Shot_Back"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Gun/monster_museum_0_shot_back_Sequence/monster_museum_0_shot_back_%d.png"), 57))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Gun_Shot_Ready*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Shot_Ready"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Gun/monster_museum_0_shot_Sequence/monster_museum_0_shot_%d.png"), 79))))
			return E_FAIL;

		/* For. Prototype_Component_Texture_Monster_Police_Gun_Aim*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Aim"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Monster_Police/Police_Gun/monster_museum_0_aim_Sequence/monster_museum_0_aim_%d.png"), 1))))
			return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Bird()
{/* For. Prototype_Component_Texture_Monster_Bird_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Bird/bg_map_object_artifact_0_1idle_Sequence/bg_map_object_artifact_0_1idle_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Bird_Active_Start */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_Active_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Bird/bg_map_object_artifact_0_2active_start_Sequence/bg_map_object_artifact_0_2active start_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Bird_Active */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_Active"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Bird/bg_map_object_artifact_0_3active_Sequence/bg_map_object_artifact_0_3active_%d.png"), 20))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Bird_End */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Bird/bg_map_object_artifact_0_4end_Sequence/bg_map_object_artifact_0_4end_%d.png"), 42))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Loading::Texture_Magic_Mirror()
{
	/* For. Prototype_Component_Texture_Monster_Mirror_Back_to_Front */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Back_to_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Magic_Mirror/monster_magic_mirror_change_back_to_front_Sequence/monster_magic_mirror_change_back_to_front_%d.png"), 26))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Mirror_Front_to_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Front_to_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Magic_Mirror/monster_magic_mirror_change_front_to back_Sequence/monster_magic_mirror_change_front_to back_%d.png"), 26))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Mirror_Die_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Die_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Magic_Mirror/monster_magic_mirror_die_back_Sequence/monster_magic_mirror_die_back_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Mirror_Front_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Front_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Magic_Mirror/monster_magic_mirror_die_front_Sequence/monster_magic_mirror_die_front_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Idle_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Idle_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Magic_Mirror/monster_magic_mirror_idle_back_Sequence/monster_magic_mirror_idle_back_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Idle_Front */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Idle_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/Magic_Mirror/monster_magic_mirror_idle_Sequence/monster_magic_mirror_idle_%d.png"), 30))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Apple()
{
	/* For. Prototype_Component_Texture_Monster_Red_Apple_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Red_Apple_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/apple/red/die/apple_die_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Red_Apple_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Red_Apple_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/apple/red/move/apple_move_%d.png"), 32))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Apple_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Purple_Apple_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/apple/purple/die/apple_die_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Purple_Apple_move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Purple_Apple_move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/apple/purple/move/apple_move_%d.png"), 31))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Heart()
{
	/* For. Prototype_Component_Texture_Monster_Heart_Move */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Heart_Move"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/monster_heart/move/monster_heart_move_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Monster_Heart_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Heart_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Monster/monster_heart/die/monster_heart_die_%d.png"), 30))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Scanner()
{
#pragma region 스캐너

	/* For. Prototype_Component_Texture_Tile_Scanner_End_Start */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_End_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Scanner/tile_scanner_0_end_start_Sequence/tile_scanner_0_end_start_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Tile_Scanner_Flying_End */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Flying_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Scanner/tile_scanner_0_flying_1&end_Sequence/tile_scanner_0_flying_1&end_%d.png"), 14))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Tile_Scanner_Flying_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Flying_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Scanner/tile_scanner_0_flying_2_idle_Sequence/tile_scanner_0_flying_2_idle_%d.png"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Tile_Scanner_Flying_Start */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Flying_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Scanner/tile_scanner_0_flying_2_start_Sequence/tile_scanner_0_flying_2_start_%d.png"), 46))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Tile_Scanner_Start */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Scanner/tile_scanner_0_start_Sequence/tile_scanner_0_start_%d.png"), 27))))
		return E_FAIL;

#pragma endregion 스캐너

	return S_OK;
}

HRESULT CLevel_Loading::Texture_Effect()
{

#pragma region 스캐너 탐지 이펙트
	/* For. Prototype_Component_Texture_Scanner_Laser */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Scanner_Laser"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Scanner_Light/Scanner_Laser_%d.png"), 3))))
		return E_FAIL;
#pragma endregion 스캐너 탐지 이펙트

#pragma region 묘비 텍스쳐..
	/* For. Prototype_Component_Texture_Rip */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rip"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Grave/Effect_Rip_%d.png"), 2))))
		return E_FAIL;
#pragma endregion 묘비 텍스쳐..

#pragma region 플레이어 지뢰 이펙트
	/* For. Prototype_Component_Texture_Player_LandMine_Max */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_LandMine_Max"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Mine_Timer/Player_Timer_Max_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 플레이어 지뢰 이펙트

#pragma region 플레이어 지뢰 카운트 이펙트
	/* For. Prototype_Component_Texture_Player_LandMine_Count*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_LandMine_Count"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Mine_Timer/Player_Timer_Count_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 플레이어 지뢰 카운트 이펙트

#pragma region 몬스터 스폰 이펙트
	/* For. Prototype_GameObject_Spawn_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Spawn_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Tile_Effect/Monster_spawn%d.png"), 1))))
		return E_FAIL;
#pragma endregion 몬스터 스폰 이펙트
#pragma region 깃털 이펙트
	/* For. Prototype_GameObject_Feather*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Feather"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Feather/Bird_Feather%d.png"), 1))))
		return E_FAIL;
#pragma endregion 깃털  이펙트
#pragma region 연기 이펙트
	/* For. Prototype_GameObject_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Smoke"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Smoke/Effect_1_Smoke_%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_GameObject_2_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_2_Smoke"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Smoke/Effect_2_Smoke_%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Textrue_Smoke_Vertical*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Textrue_Smoke_Vertical"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Vertical_Smoke/tex-vfx-smoke-vertical%d.png"), 16))))
		return E_FAIL;
#pragma endregion 연기 이펙트
#pragma region 지뢰 이펙트
	/* For. Prototype_GameObject_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MineTimer_Max"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Mine_Timer/Mine_Timer_Max_%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_GameObject_2_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MineTimer_Count"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Mine_Timer/Mine_Timer_Count_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 지뢰 이펙트

#pragma region 새 마법진 이펙트
	/* For. Prototype_GameObject_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bird_Teleport_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Tile_Effect/Bird_Teleport/Bird_Teleport_Tile%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_GameObject_2_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bird_Spawn_Light_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Tile_Effect/Bird_Teleport/Spawn_Light_Circle%d.png"), 1))))
		return E_FAIL;
#pragma endregion 새 마법진 이펙트

#pragma region 전화기 떨어지는거 이펙트
	/* For. Prototype_GameObject_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Telephone_Timer_Count_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Telephone_Drop_Timer/Mine_Timer_Count_%d.png"), 1))))
		return E_FAIL;

	/* For. Effect_Telephone_Drop_Timer*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Telephone_Timer_Max_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Telephone_Drop_Timer/Mine_Timer_Max_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 전화기 떨어지는거 이펙트

#pragma region 총알맞는 이펙트
	/* For. Prototype_GameObject_Bullet_Hit_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_Hit_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Hit_Effect/Hit_Effect_%d.png"), 2))))
		return E_FAIL;
#pragma endregion 총알맞는 이펙트

#pragma region 노이즈 텍스쳐
	/* For. Prototype_Component_Texture_Noise_Texture*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise_Texture"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Noise/Tex_Noise_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 노이즈 텍스쳐
	
#pragma region 피에로 등장 이펙트
	/* For. Prototype_GameObject_Pierrot_Spawn_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Pierrot_Spawn_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Pierrot_intro/Effect_Pierrot_intro_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 피에로 등장 이펙트

#pragma region 텍스쳐 빛 이펙트
	/* For. Prototype_GameObject_Pierrot_Spawn_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Spark_Light"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Spark_Light/Spark_Light%d.png"), 1))))
		return E_FAIL;
#pragma endregion 텍스쳐 빛 이펙트

#pragma region 임펙트 이펙트
	/* For. Prototype_Component_Effect_Impact*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Impact"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Impact/Effect_Impact_%d.png"), 9))))
		return E_FAIL;
#pragma endregion 임펙트 이펙트

#pragma region 레벨업 이펙트
	/* For. Prototype_Component_Effect_Level_UP_Paint*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Level_UP_Paint"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_LevelUp/Ground/level_up_box_ground_%d.png"), 4))))
		return E_FAIL;
#pragma endregion 레벨업 이펙트

#pragma region 보스 화살표 이펙트
	/* For. Prototype_Component_Effect_Level_UP_Paint*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Boss_Arrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Boss_Arrow/Boss_Arrow_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 보스 화살표 이펙트

#pragma region 레벨업2 이펙트
	/* For. Prototype_Component_Effect_Level_Up_Box*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Level_Up_Box"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_LevelUp/level_up_box_%d.png"), 6))))
		return E_FAIL;
#pragma endregion 레벨업2 이펙트

#pragma region 블라스트 이펙트
	/* For. Prototype_Component_Effect_Blast*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Blast"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Blast/Effect_Blast_%d.png"), 16))))
		return E_FAIL;
#pragma endregion 블라스트 이펙트

#pragma region 경찰 죽는 이펙트
	/* For. Prototype_Component_Effect_Police_Die*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Police_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Police_Die/Effect_Police_Die_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 경찰 죽는 이펙트

#pragma region 쉴드 끝나고 이펙트
	/* For. Prototype_Component_Effect_Shield_End*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Shield_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Shield_End/Effect_Shield_End_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 쉴드 끝나고 이펙트

#pragma region 연기 이펙트
	/* For. Prototype_Component_Effect_Burn_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Burn_Smoke"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Burn_Smoke/Effect_Burn_Smoke_%d.png"), 14))))
		return E_FAIL;
#pragma endregion 연기 이펙트

#pragma region 솜 죽는 이펙트
	/* For.
	*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Som_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Som_Die_Effect/Som_Die_Effect_%d.png"), 4))))
		return E_FAIL;
#pragma endregion 솜 죽는 이펙트

#pragma region 경험치 트레일
	/* For.
	*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Exp_Trail"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Trail/Effect_Trail_%d.png"), 1))))
		return E_FAIL;
#pragma endregion 경험치 트레일

#pragma region 사과 죽는 이펙트
	/* For. Prototype_Component_Effect_Red_Apple_Die*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Red_Apple_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_AppleDie/Red/Dead_Red_Effect_%d.png"), 4))))
		return E_FAIL;
#pragma endregion 사과 죽는 이펙트

	// 0625 심현우 : 타일
	// 0626 이재진 : 1장 더 추가함
	/* For. 
	*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Tile_Rect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Tile_Rect/Tile_Rect%d.png"), 15))))
		return E_FAIL;

#pragma region 타일 안개 이펙트
	/* For. Prototype_Component_Effect_Tile_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Tile_Smoke"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Tile_Smoke/Tile_Smoke%d.png"), 1))))
		return E_FAIL;
#pragma endregion 타일 안개 이펙트

	return S_OK;
}


HRESULT CLevel_Loading::Texture_UI()
{
	/* For. Prototype_Component_Texture_Minimap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Minimap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/MiniMap/ui_minimap%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ShowDown */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShowDown"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/ui_showdown%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_PlayerDetail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerDetail"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/ui_playerDetail%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/HpBar/ui_detail_hp%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BossHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Boss/ui_boss_energy%d.png"), 3))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Monster/ui_monsterHpBar%d.png"), 3))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Icon/ui_icon%d.png"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Skill/ui_skill%d.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_ProgressBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ProgressBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/ingame_progressBar%d.png"), 1))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_UI_ProgressArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ProgressArrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/ingame_arrow%d.png"), 1))))
		return E_FAIL;

#pragma region Result
	/* For. Prototype_Component_Texture_Result_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_BackGround"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/ui_result_win_bg%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Line"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/ui_result_win_line%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Dog */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Dog"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/WinIdle/Dog/win_idle_%d.png"), 112))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_Result_Cat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Cat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/WinIdle/Cat/win_idle_%d.png"), 111))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Result_Penguin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Penguin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Win/WinIdle/Penguin/win_idle_%d.png"), 112))))
		return E_FAIL;
#pragma endregion Result

	return S_OK;
}

HRESULT CLevel_Loading::Prototype_UI()
{
	/* For. Prototype_GameObject_UI_Manager */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Manager"),
		CUI_Manager::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Minimap */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minimap"),
		CMinimap::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHpBar"),
		CPlayerHpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_ShowDown */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShowDown"),
		CShowDown::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_ShowDown */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerDetail"),
		CPlayerDetail::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_HpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpBar"),
		CHpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_UI_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Icon"),
		CIcon::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Skill"),
		CSkillGauge::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MonsterHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHpBar"),
		CMonsterHpBar::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Result_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Result_Line"),
		CWinResult::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Result_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Result_BG"),
		CWinResult_BG::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Result_Char */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Result_Char"),
		CWinResult_Char::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_ProgressBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ProgressBar"),
		CProgressBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_ProgressArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ProgressArrow"),
		CProgressArrow::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Prototype_Manager()
{
	/* For. Prototype_GameObject_GameManager */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GameManager"),
		CGameManager::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVELID eNextLevelID, PLAYER_CHARACTER ePlayerID, _bool bReLoading)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pGraphic_Device, ePlayerID, bReLoading);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{	
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	__super::Free();

}
