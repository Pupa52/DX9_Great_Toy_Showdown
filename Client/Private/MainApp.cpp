#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "..\Public\Level_Loading.h"

#include "TileMgr.h"
#include "BackGround.h"
#include "Loading_Pattern.h"
#include "DodoHead.h"
#include "LoadingBar.h"
#include "Cursor.h"
#include "BotHpBar.h"

#include "CanMoveCube.h"
#include "Camera.h"

#include "Player_Bullet.h"
#include "GumBoom.h"
#include "CarrotBoom.h"
#include "Shield.h"
#include "Electric_Bolt.h"

#include "Effect_Gum_Big_Splash.h"
#include "Effect_Gum_Skul.h"
#include "Effect_Gum_Small_Splash.h"
#include "Effect_LandMine_Explosion.h"
#include "Effect_Bullet_Fire.h"

#include "Damage.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	/* 내가 레퍼런스 카운트를 증가시키는 시점 : */
	/* 원래 보관하기위한 포인터에 저장을 할때는 증가시키지 않는다. */
	/* 원래 보관된 위치에 있던 주소를 다른 포인터형변수에 저장을 하게되면 이때는 증가시킨다. */
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iMaxSoundChannel = (_uint)CHANNELID::SOUND_END;
	EngineDesc.isWindowsed = true;

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

#ifdef _DEBUG

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

#endif // _DEBUG

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{	
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/* 이후 모든 렌더링과정에서 이 셋팅을 먹이고 싶어서. */
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	/* For. Prototype_Component_VIBuffer_Cylinder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cylinder"),
		CVIBuffer_Cylinder::Create(m_pGraphic_Device, .5f, .5f, 1.f, 100))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Animator */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		CAnimator::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"),
		CCollider_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		CCollider_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_TileMgr */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TileMgr"),
		CTileMgr::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Tile */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tile"),
		CTile::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cursor"),
		CCursor::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		CFSM::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"),
		CCamera::Create(m_pGraphic_Device))))
		return E_FAIL;

	Texture_Cat();
	Texture_Dog();
	Texture_Penguin();
	Character_Skill();
	Character_Bullet();
	Effect();
	Object();

#pragma region LEVEL_LOADING

	/* For. Prototype_Component_Texture_Loading_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Background"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Loading/loading_background%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Loading_Pattern */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Pattern"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Loading/Pattern_twinkle%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Loading_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Loading/loading_icon%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Loading_Bar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Loading/loading_bar%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_GameObject_Loading_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Background"),
		CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Loading_Pattern */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Pattern"),
		CLoading_Pattern::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Loading_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Icon"),
		CDodoHead::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Loading_Bar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Bar"),
		CLoadingBar::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion LEVEL_LOADING

	/* For. Prototype_Component_Texture_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/Cursor/ui_cursor%d.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Player_Hp_Bar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Hp_Bar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/UI/HpBar/ui_player_hp_bar%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_GameObject_BotHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BotHpBar"),
		CBotHpBar::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_CanMoveCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CanMoveCube"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURECUBE, TEXT("../Resources/Texture2D/Obstacle/canmove%d.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_GameObject_CanMoveCube */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CanMoveCube"),
		CCanMoveCube::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eStartLevelID, PLAYER_CHARACTER::PLAYER_END))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Texture_Cat()
{
#pragma region 고양이
#pragma region 고양이 공격 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Attack/Head/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Attack/Body/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Attack/Weapon_Pistol/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;
#pragma endregion

#pragma region 고양이 공격 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Attack/Head/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Attack/Body/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Attack/Weapon_Pistol/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;
#pragma endregion

#pragma region 고양이 움직임 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Move_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Move/Head/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Move_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Move/Body/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Move_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Move/Weapon_Pistol/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

#pragma endregion

#pragma region 고양이 움직임 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Move_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Move/Head/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Move_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Move/Body/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Move_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Move/Weapon_Pistol/move_weapon_3_%d.png"), 31))))
		return E_FAIL;
#pragma endregion

#pragma region 고양이 기본 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Idle/Head/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Idle/Body/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Front/Idle/Weapon_Pistol/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;
#pragma endregion

#pragma region 고양이 기본 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Idle/Head/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Idle/Body/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Cat/Back/Idle/Weapon_Pistol/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;
#pragma endregion
#pragma endregion
	return S_OK;
}

HRESULT CMainApp::Texture_Dog()
{
#pragma region 강아지
#pragma region 강아지 공격 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Attack_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Attack/Head/attack_weapon_3_%d.png"), 29))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Attack_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Attack/Body/attack_weapon_3_%d.png"), 29))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Attack_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Attack/Weapon_Rifle/attack_weapon_3_%d.png"), 29))))
		return E_FAIL;
#pragma endregion

#pragma region 강아지 공격 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Attack_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Attack/Head/attack_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Attack_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Attack/Body/attack_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Attack_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Attack/Weapon_Rifle/attack_weapon_3_%d.png"), 30))))
		return E_FAIL;
#pragma endregion


#pragma region 강아지 움직임 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Move_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Move/Head/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Move_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Move/Body/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Move_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Move/Weapon_Rifle/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

#pragma endregion

#pragma region 강아지 움직임 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Move_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Move/Head/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Move_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Move/Body/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Move_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Move/Weapon_Rifle/move_weapon_3_%d.png"), 31))))
		return E_FAIL;
#pragma endregion


#pragma region 강아지 기본 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Idle_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Idle/Head/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Idle_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Idle/Body/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Front_Idle_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Front/Idle/Weapon_Rifle/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;
#pragma endregion

#pragma region 강아지 기본 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Idle_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Idle/Head/idle_weapon_3_%d.png"), 29))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Idle_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Idle/Body/idle_weapon_3_%d.png"), 29))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Dog_Back_Idle_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Dog/Back/Idle/Weapon_Rifle/idle_weapon_3_%d.png"), 29))))
		return E_FAIL;
#pragma endregion
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Texture_Penguin()
{
#pragma region 펭귄
#pragma region 펭귄 공격 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Attack_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Attack/Head/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Attack_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Attack/Body/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Attack_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Attack_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Attack/Weapon_Shotgun/attack_weapon_3_%d.png"), 28))))
		return E_FAIL;
#pragma endregion

#pragma region 펭귄 공격 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Attack_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Attack/Head/attack_weapon_3_%d.png"), 27))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Attack_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Attack/Body/attack_weapon_3_%d.png"), 27))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Attack_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Attack_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Attack/Weapon_Shotgun/attack_weapon_3_%d.png"), 27))))
		return E_FAIL;
#pragma endregion


#pragma region 펭귄 움직임 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Move_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Move/Head/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Move_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Move/Body/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Move_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Move/Weapon_Shotgun/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

#pragma endregion

#pragma region 펭귄 움직임 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Move_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Move/Head/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Move_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Move/Body/move_weapon_3_%d.png"), 31))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Back_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Move_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Move/Weapon_Shotgun/move_weapon_3_%d.png"), 31))))
		return E_FAIL;
#pragma endregion


#pragma region 펭귄 기본 앞모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Idle_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Idle/Head/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Body */

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Idle_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Idle/Body/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Idle_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Front_Idle_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Front/Idle/Weapon_Shotgun/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;
#pragma endregion

#pragma region 펭귄 기본 뒷모습
	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Idle_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Idle/Head/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Idle_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Idle/Body/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;

	/* For. Prototype_Component_Texture2D_Cat_Front_Move_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture2D_Penguin_Back_Idle_Weapon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D,
			TEXT("../Resources/Texture2D/Player/Penguin/Back/Idle/Weapon_Shotgun/idle_weapon_3_%d.png"), 30))))
		return E_FAIL;
#pragma endregion
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Character_Skill()
{
	/* For. Prototype_Component_Texture_Player_Skill_Gum */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Skill_Gum"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Player_Bullet/Gum_Skill_Red.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Player_Skill_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Skill_Shield"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Player_Bullet/Bubble.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Player_Skill_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Skill_Boom"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Player_Bullet/Bullet_Fire.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Electric_Bolt */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Electric_Bolt"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Electric_Bolt/Electric_Bolt_%d.png"), 2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Character_Bullet()
{
	/* For. Prototype_Component_Texture_Player_Bullet_Pistol */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Bullet_Pistol"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Player_Bullet/Bullet_Pistol_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Player_Bullet_Rifle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Bullet_Rifle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Player_Bullet/Bullet_Rifle.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Player_Bullet_Shotgun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Bullet_Shotgun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Bullet/Player_Bullet/Bullet_Shotgun.png"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Effect()
{
#pragma region 총구 화염
	/* For. Prototype_Component_Texture_Missile_Boom */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_Fire"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Bullet_Fire/Bullet_Fire_%d.png"), 4))))
		return E_FAIL;
#pragma endregion

#pragma region 껌 해골
	/* For. Prototype_Component_Texture_Gum_Skul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gum_Skul"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Gum_Skul/Gum_Skul_%d.png"), 14))))
		return E_FAIL;
#pragma endregion

#pragma region 껌 작은 물방울 튐
	/* For. Prototype_Component_Texture_Gum_Small_Splash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gum_Small_Splash"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Gum_Small_Splash/Gum_Small_Splash_%d.png"), 16))))
		return E_FAIL;
#pragma endregion

#pragma region 껌 큰 물방울 튐
	/* For. Prototype_Component_Texture_Gum_Big_Splash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gum_Big_Splash"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Gum_Big_Splash/Gum_Big_Splash_%d.png"), 12))))
		return E_FAIL;
#pragma endregion

#pragma region 고추폭탄 폭발
	/* For. Prototype_GameObject_Texture_Mine_Explosive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Texture_Mine_Explosive"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Resources/Texture2D/Effect/Effect_Explosive/Effect_Explosive_%d.png"), 17))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CMainApp::Object()
{
	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Bullet"),
		CPlayer_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	// 여기서부터 스킬
/* For. Prototype_GameObject_PlayerSkill_GumBoom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerSkill_GumBoom"),
		CGumBoom::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerSkill_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerSkill_Shield"),
		CShield::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerSkill_CarrotBoom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerSkill_CarrotBoom"),
		CCarrotBoom::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerSkill_ElectricBolt */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerSkill_ElectricBolt"),
		CElectric_Bolt::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 이펙트
	/* For. Prototype_GameObject_Effect_Gum_Skul */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Gum_Skul"),
		CEffect_Gum_Skul::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Gum_Small_Splash */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Gum_Small_Splash"),
		CEffect_Gum_Small_Splash::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Gum_Big_Splash */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Gum_Big_Splash"),
		CEffect_Gum_Big_Splash::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Mine_Explosive */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mine_Explosive"),
		CEffect_LandMine_Explosion::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Meteor_Ground */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Bullet_Fire"),
		CEffect_Bullet_Fire::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	// 데미지 폰트
	/* For. Prototype_GameObject_Damage */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Damage"),
		CDamage::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}


CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	
}

