#include "stdafx.h"
#include "../Public/UI_Manager.h"

#include "GameInstance.h"

#include "Layer.h"
#include "Icon.h"
#include "WinResult.h"
#include "WinResult_BG.h"
#include "WinResult_Char.h"

CUI_Manager::CUI_Manager(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CUI_Manager::CUI_Manager(const CUI_Manager& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUI_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Manager::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_tUIMgrDesc = *static_cast<UIMGR_DESC*>(pArg);

	if (FAILED(Get_Player()))
		return E_FAIL;

	if (FAILED(Init_UI()))
		return E_FAIL;

	if (FAILED(Init_Icon()))
		return E_FAIL;

	if (FAILED(Set_HpBar()))
		return E_FAIL;

	//if (FAILED(Play_Result()))
	//	return E_FAIL;

	return S_OK;
}

_uint CUI_Manager::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CUI_Manager::Update(_float fTimeDelta)
{
}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Manager::Render()
{
	Print_Font();

	return S_OK;
}

HRESULT CUI_Manager::Get_Player()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(m_tUIMgrDesc.iLevelIndex, TEXT("Layer_Player"));
	
	m_pObject = pLayer->Get_Objects().front();
	if (nullptr == m_pObject)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pObject);
	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Print_Font()
{
	CActor::ACTOR_DESC tDesc = m_pPlayer->Get_ActorDesc();
	_tchar buffer[32];
	_int iCurBullet = m_pPlayer->Get_BulletCount();
	_int iMaxBullet = m_pPlayer->Get_TotalBulletCount();

	swprintf_s(buffer, L"%2d / %2d", tDesc.iHp, tDesc.iMaxHp);
	m_pGameInstance->Draw_Text(FT_L_LARGE, wstring(buffer), 60, (_uint)g_iWinSizeY - 60, D3DCOLOR_ARGB(255, 0, 0, 0));
	m_pGameInstance->Draw_Text(FT_LARGE, wstring(buffer), 60, (_uint)g_iWinSizeY - 60, D3DCOLOR_ARGB(255, 255, 255, 255));

	swprintf_s(buffer, L"%2d", tDesc.iAttack);
	m_pGameInstance->Draw_Text(FT_LARGE, wstring(buffer), 55, (_uint)g_iWinSizeY - 85, D3DCOLOR_ARGB(255, 0, 0, 0));
	m_pGameInstance->Draw_Text(FT_MIDDLE, wstring(buffer), 55, (_uint)g_iWinSizeY - 85, D3DCOLOR_ARGB(255, 255, 255, 255));

	swprintf_s(buffer, L"%2d / %2d", iCurBullet, iMaxBullet);
	m_pGameInstance->Draw_Text(FT_LARGE, wstring(buffer), 325, (_uint)g_iWinSizeY - 45, D3DCOLOR_ARGB(255, 0, 0, 0));
	m_pGameInstance->Draw_Text(FT_MIDDLE, wstring(buffer), 325, (_uint)g_iWinSizeY - 45, D3DCOLOR_ARGB(255, 255, 255, 255));

	return S_OK;
}

HRESULT CUI_Manager::Set_HpBar()
{
	CUI::UI_DESC tPlayerDesc = { 0.f, 0.f, 0.f, 0.f, m_pObject, 0 };

	// 플레이어 하단 체력바
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_HpBar"), (void*)&tPlayerDesc)))
		return E_FAIL;
	
	// 플레이어 상단 체력바
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerHpBar"), (void*)&tPlayerDesc)))
		return E_FAIL;

	// 아군봇 상단 체력바
	CLayer* pLayer = m_pGameInstance->Find_Layer(m_tUIMgrDesc.iLevelIndex, TEXT("Layer_Friend_Bot"));
	if (nullptr == pLayer)
		return E_FAIL;
	CGameObject* pFriendBot = pLayer->Get_Objects().back();

	CUI::UI_DESC tFriendDesc = { 0.f, 0.f, 0.f, 0.f, pFriendBot, 0, nullptr, 1 };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_BotHpBar"), (void*)&tFriendDesc)))
		return E_FAIL;
	
	// 적군봇 상단 체력바
	pLayer = m_pGameInstance->Find_Layer(m_tUIMgrDesc.iLevelIndex, TEXT("Layer_Bot"));
	if (nullptr == pLayer)
		return E_FAIL;
	list<CGameObject*> Enemys = pLayer->Get_Objects();
	for (auto& iter : Enemys)
	{
		CUI::UI_DESC tBotDesc = { 0.f, 0.f, 0.f, 0.f, iter, 0 };
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_BotHpBar"), (void*)&tBotDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Manager::Play_Result()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_Result_BG"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_Result_Line"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_Result_Char"), (void*)&m_tUIMgrDesc.eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Init_UI()
{
	HRESULT hr = E_FAIL;

	switch (m_tUIMgrDesc.iLevelIndex)
	{
	case LEVEL_GAMEPLAY:
		hr = Init_GamePlay_UI();
		break;
	}

	return hr;
}

HRESULT CUI_Manager::Init_Icon()
{
	HRESULT hr = E_FAIL;

	switch (m_tUIMgrDesc.iLevelIndex)
	{
	case LEVEL_GAMEPLAY:
		hr = Init_GamePlay_Icon();
		break;
	}

	return hr;
}

HRESULT CUI_Manager::Init_GamePlay_UI()
{
	CUI::UI_DESC tDesc = { 0.f, 0.f, 0.f, 0.f, m_pObject, 0 };

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
	//	TEXT("Layer_UI"), TEXT("Prototype_GameObject_Minimap"), (void*)&tDesc)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_ShowDown"), (void*)&tDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_PlayerDetail"), (void*)&tDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Skill"), (void*)&tDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_ProgressBar"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_ProgressArrow"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_Cursor"), TEXT("Prototype_GameObject_Cursor"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Init_GamePlay_Icon()
{
	CUI::UI_DESC tDesc = { 0.f, 0.f, 0.f, 0.f, nullptr, 0 };

	tDesc.fSizeX = 40.f;
	tDesc.fSizeY = 40.f;
	tDesc.fX = 25.f;
	tDesc.fY = g_iWinSizeY - 30.f;
	tDesc.iType = (_int)CIcon::ICON_TYPE::IT_HEALTH;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Icon"), (void*)&tDesc)))
		return E_FAIL;

	tDesc.fSizeX = 30.f;
	tDesc.fSizeY = 30.f;
	tDesc.fX = 25.f;
	tDesc.fY = g_iWinSizeY - 70.f;
	tDesc.iType = (_int)CIcon::ICON_TYPE::IT_POWER;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Icon"), (void*)&tDesc)))
		return E_FAIL;

	tDesc.fSizeX = 35.f;
	tDesc.fSizeY = 35.f;
	tDesc.fX = 300.f;
	tDesc.fY = g_iWinSizeY - 30.f;
	tDesc.iType = (_int)CIcon::ICON_TYPE::IT_BULLET;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Icon"), (void*)&tDesc)))
		return E_FAIL;

	//tDesc.fSizeX = 40.f;
	//tDesc.fSizeY = 40.f;
	//tDesc.fX = (g_iWinSizeX >> 1) - 70.f;
	//tDesc.fY = g_iWinSizeY - 35;
	//tDesc.iType = (_int)CIcon::ICON_TYPE::IT_MOUSE;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
	//	TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Icon"), (void*)&tDesc)))
	//	return E_FAIL;

	//tDesc.fSizeX = 30.f;
	//tDesc.fSizeY = 30.f;
	//tDesc.fX = 800.f;
	//tDesc.fY = 20.f;
	//tDesc.iType = (_int)CIcon::ICON_TYPE::IT_MARK;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
	//	TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Icon"), (void*)&tDesc)))
	//	return E_FAIL;
	return S_OK;
}

CUI_Manager* CUI_Manager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Manager* pInstance = new CUI_Manager(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Created to Failed : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Manager::Clone(void* pArg)
{
	CUI_Manager* pInstance = new CUI_Manager(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Created to Cloned : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();
}
