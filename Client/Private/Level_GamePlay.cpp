#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "Level_Loading.h"

#include "GameInstance.h"
#include "TileMap.h"
#include "Camera.h"
#include "Layer.h"
#include "Sky.h"
#include "Storm.h"

#include "TileMgr.h"
#include "UI_Manager.h"
#include "GameManager.h"

#include "Item_Object.h"

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID)
	: CLevel { pGraphic_Device }
	, m_ePlayerID(ePlayerID)
{
}

// HI
HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	CGameManager::GM_DESC tGMDesc{ m_ePlayerID, LEVEL_GAMEPLAY };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_GameMgr"), TEXT("Prototype_GameObject_GameManager"), (void*)&tGMDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CUI_Manager::UIMGR_DESC tUIMgrDesc{ LEVEL_GAMEPLAY };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UIMgr"), TEXT("Prototype_GameObject_UI_Manager"), (void*)&tUIMgrDesc)))
		return E_FAIL;

	m_pGameInstance->PlayBGM(L"BGM_Ingame_Battle_Phase2 2.wav", (_uint)CHANNELID::SOUND_BGM, 0.4f);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (true == m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->Get_Objects().empty())
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_LOBBY,
			PLAYER_CHARACTER::PLAYER_END, true))))
			return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 10.f, -10.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround()
{
	CTileMap::TILEMAP_DESC tTileMapDesc = {};
	tTileMapDesc.eLevelID = LEVEL_GAMEPLAY;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_TileMap"), &tTileMapDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, 
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	CSky::SKY_DESC tSkyDesc = {};
	tSkyDesc.eLevelID = LEVEL_GAMEPLAY;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), &tSkyDesc)))
		return E_FAIL;

	_uint TileMapLength = 118;
	CTileMgr::TILEMGR_DESC tTileMgrDesc{ TileMapLength, TileMapLength, LEVEL_GAMEPLAY };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, 
		TEXT("Layer_TileMgr"), TEXT("Prototype_GameObject_TileMgr"), (void*)&tTileMgrDesc)))
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
		_float3 _vPos = { fOffset + i, 0.5f, fOffset + TileMapLength + 1};
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
		_float3 _vPos = { fOffset + TileMapLength + 1 , 0.5f, fOffset + i};
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
			TEXT("Layer_Wall"), TEXT("Prototype_GameObject_CanMoveCube"), (void*)&_vPos)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Tile_Smoke"), nullptr)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Storm"), TEXT("Prototype_GameObject_Storm"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	Desc = {};

	Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	//if (FAILED(Ready_Layer_Item(Desc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Item(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
{
	CItem_Object::ITEM_DESC desc = {};
	
	desc.pTerrainTransform = LandObjectDesc.pTerrainTransform;
	desc.pTerrainVIBuffer = LandObjectDesc.pTerrainVIBuffer;
	
	desc.eItemType = CItem_Object::ITEM_SWORD;
	desc.vPos = { 10.f, 1.f, 10.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;
	
	desc.eItemType = CItem_Object::ITEM_SKATE;
	desc.vPos = { 15.f, 1.f, 10.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;

	desc.eItemType = CItem_Object::ITEM_MAGAZINE;
	desc.vPos = { 20.f, 1.f, 10.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;

	desc.eItemType = CItem_Object::ITEM_METEOR;
	desc.vPos = { 25.f, 1.f, 10.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;

	desc.eItemType = CItem_Object::ITEM_MINE;
	desc.vPos = { 30.f, 1.f, 10.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;

	desc.eItemType = CItem_Object::ITEM_MISSILE;
	desc.vPos = { 35.f, 1.f, 10.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;

	desc.eItemType = CItem_Object::ITEM_TELEPORT;
	desc.vPos = { 53.f, 1.f, 12.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	D3DLIGHT9			LightDesc{};
	/* 빛의 종류. */
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	/* L.Diffuse * M.Diffuse * (Shade + (L.Ambient * M.Ambient))*/
	/* 빛의 색 */
	// LightDesc.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.6f, 0.f, 1.f, 1.f);
	/* 빛의 범위.*/
	/* 명, 암. */
	LightDesc.Direction = _float3(1.f, -1.f, 1.f);
	/* iIndex번째 광원의 정보를 장치에 저장해놓는다. */
	m_pGraphic_Device->SetLight(5, &LightDesc);
	m_pGraphic_Device->LightEnable(5, TRUE);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device, PLAYER_CHARACTER ePlayerID)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pGraphic_Device, ePlayerID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
	m_pGraphic_Device->LightEnable(5, FALSE);
}
