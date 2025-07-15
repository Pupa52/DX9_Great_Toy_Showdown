#include "stdafx.h"
#include "GameManager.h"

#include "Player.h"
#include "Bot.h"
#include "Friend_Bot.h"
#include "Pierrot.h"
#include "Monster_Bird.h"
#include "Monster_Police.h"
#include "Monster_Telephone.h"
#include "UI_Manager.h"
#include "Layer.h"

CGameManager::CGameManager(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject(pGraphic_Device)
{
}

CGameManager::CGameManager(const CGameManager& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CGameManager::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameManager::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    m_tGMDesc = *static_cast<GM_DESC*>(pArg);

    CLandObject::LANDOBJECT_DESC	Desc = {};

    Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
    Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

    // player
    if (FAILED(Spawn_Player(Desc, _float3{ 53.f, 1.f, 12.5f })))
        return E_FAIL;

    if (FAILED(Spawn_Bot(Desc, _float3{ 53.f, 1.f, 12.5f })))
        return E_FAIL;

    // 테스트 용
    //if (FAILED(Spawn_Enemy(Desc, _float3{ 45.5f, 1.f, 12.5f })))
    //    return E_FAIL;
    
    //if (FAILED(Spawn_Enemy(Desc, _float3{ 48.f, 1.f, 12.5f })))
    //    return E_FAIL;
    
    //if (FAILED(Spawn_Enemy(Desc, _float3{ 50, 1.f, 12.5f })))
    //    return E_FAIL;

    // 실전 용
    if (FAILED(Spawn_Enemy(Desc, _float3{ 15.5f, 1.f, 105.5f })))
        return E_FAIL;
    
    if (FAILED(Spawn_Enemy(Desc, _float3{ 108.5f, 1.f, 56.5f })))
        return E_FAIL;
    
    if (FAILED(Spawn_Enemy(Desc, _float3{ 97.5f, 1.f, 9.5f })))
        return E_FAIL;


    // monster
    if (FAILED(Spawn_Pierrot(Desc, _float3{59.5f, 0.f, 59.5f})))
        return E_FAIL;
    // 새 왼쪽 아래
    if (FAILED(Spawn_Bird(Desc, _float3(14.f, 4.f, 20.f))))
        return E_FAIL;
    // 새 오른쪽
    if (FAILED(Spawn_Bird(Desc, _float3(91.f, 4.f, 83.f))))
        return E_FAIL;

    // Police 왼쪽 위
    if (FAILED(Spawn_Police(Desc, _float3(34.f + rand() % 5, 1.f, 75.f + rand() % 5))))
        return E_FAIL;
    if (FAILED(Spawn_Police(Desc, _float3(34.f + rand() % 5, 1.f, 75.f + rand() % 5))))
        return E_FAIL;
    if (FAILED(Spawn_Police(Desc, _float3(34.f + rand() % 5, 1.f, 75.f + rand() % 5))))
        return E_FAIL;
    if (FAILED(Spawn_Police(Desc, _float3(34.f + rand() % 5, 1.f, 75.f + rand() % 5))))
        return E_FAIL;

    // Police 왼쪽
    if (FAILED(Spawn_Police(Desc, _float3(11.f + rand() % 5, 1.f, 50.f + rand() % 5))))
        return E_FAIL;
    if (FAILED(Spawn_Police(Desc, _float3(11.f + rand() % 5, 1.f, 50.f + rand() % 5))))
        return E_FAIL;
    if (FAILED(Spawn_Police(Desc, _float3(11.f + rand() % 5, 1.f, 50.f + rand() % 5))))
        return E_FAIL;
    if (FAILED(Spawn_Police(Desc, _float3(11.f + rand() % 5, 1.f, 50.f + rand() % 5))))
        return E_FAIL;

    // 플레이어 바로 오른쪽
    if (FAILED(Spawn_Telephone(Desc, _float3(84.f, 10.f, 36.f))))
        return E_FAIL;

    if (FAILED(Spawn_Telephone(Desc, _float3(59.f, 10.f, 102.f))))
        return E_FAIL;

    m_pEnemyLayer = m_pGameInstance->Find_Layer(m_tGMDesc.eLevelIndex, TEXT("Layer_Bot"));
    if (nullptr == m_pEnemyLayer)
        return E_FAIL;

    return S_OK;
}

_uint CGameManager::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CGameManager::Update(_float fTimeDelta)
{
    if (nullptr == m_pEnemyLayer)
        return;

    size_t CurCount = m_pEnemyLayer->Get_Objects().size();
    if (CurCount <= 0 && m_bResult == false)
    {
        CLayer* pUIMgr = m_pGameInstance->Find_Layer(m_tGMDesc.eLevelIndex, TEXT("Layer_UIMgr"));
        if (nullptr == pUIMgr)
            return;
        CUI_Manager* pMgr = static_cast<CUI_Manager*>(pUIMgr->Get_Objects().front());
        pMgr->Play_Result();
        m_bResult = true;
    }
}

void CGameManager::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameManager::Render()
{
    return S_OK;
}

HRESULT CGameManager::Spawn_Player(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CPlayer::PLAYER_DESC PlayerDesc = {};
    PlayerDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    PlayerDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;
    PlayerDesc.ePlayerID = m_tGMDesc.ePlayerID;
    PlayerDesc.vStartPos = vSpawnPos;
    PlayerDesc.eLevel = LEVEL_GAMEPLAY;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &PlayerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameManager::Spawn_Bot(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CBot::PLAYER_DESC BotDesc = {};
    BotDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    BotDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;
    BotDesc.ePlayerID = PLAYER_CHARACTER::PLAYER_DOG;
    BotDesc.vStartPos = vSpawnPos;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Friend_Bot"), TEXT("Prototype_GameObject_Friend_Bot"), &BotDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CGameManager::Spawn_Enemy(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CBot::PLAYER_DESC BotDesc = {};
    BotDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    BotDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;

    if (rand() % 2 == 0)
    {
        BotDesc.ePlayerID = PLAYER_CHARACTER::PLAYER_CAT;
    }
    else
    {
        BotDesc.ePlayerID = PLAYER_CHARACTER::PLAYER_PENGUIN;
    }
    BotDesc.vStartPos = vSpawnPos;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Bot"), TEXT("Prototype_GameObject_Bot"), &BotDesc)))
        return E_FAIL;

    CLayer* BotLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Bot")));

    if (nullptr == BotLayer)
        return E_FAIL;

    list<CGameObject*> Bot_Obj = BotLayer->Get_Objects();

    for (auto& pDst : Bot_Obj)
    {
        CBot* pBotObject = static_cast<CBot*>(pDst);
        if (pBotObject->Get_ID() == ID_END)
        {
            pBotObject->Set_ID((Bullet_ID)(m_iBotCount));
            m_iBotCount++;
        }
    }

    return S_OK;
}

HRESULT CGameManager::Spawn_Pierrot(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CMonster::MONSTER_DESC tDesc = {};
    tDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    tDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;
    tDesc.m_Pos = vSpawnPos;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Pierrot"), &tDesc)))
        return E_FAIL;

    Set_MonsterUI();

    return S_OK;
}

HRESULT CGameManager::Spawn_Telephone(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CMonster::MONSTER_DESC tDesc = {};
    tDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    tDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;
    tDesc.m_Pos = vSpawnPos;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Telephone"), &tDesc)))
        return E_FAIL;

    Set_MonsterUI();

    return S_OK;
}

HRESULT CGameManager::Spawn_Bird(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CMonster::MONSTER_DESC tDesc = {};
    tDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    tDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;
    tDesc.m_Pos = vSpawnPos;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Bird"), &tDesc)))
        return E_FAIL;

    Set_MonsterUI();

    return S_OK;
}

HRESULT CGameManager::Spawn_Police(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos)
{
    CMonster::MONSTER_DESC tDesc = {};
    tDesc.pTerrainTransform = tLandObjectDesc.pTerrainTransform;
    tDesc.pTerrainVIBuffer = tLandObjectDesc.pTerrainVIBuffer;
    tDesc.m_Pos = vSpawnPos;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
        TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Police"), &tDesc)))
        return E_FAIL;

    Set_MonsterUI();

    return S_OK;
}

HRESULT CGameManager::Set_MonsterUI()
{
    CLayer* pLayer = m_pGameInstance->Find_Layer(m_tGMDesc.eLevelIndex, TEXT("Layer_Monster"));
    CGameObject* pObject = pLayer->Get_Objects().back();

    CUI::UI_DESC tDesc = {0.f, 0.f, 0.f, 0.f, pObject, 0 };
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tGMDesc.eLevelIndex,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHpBar"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

CGameManager* CGameManager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameManager* pInstance = new CGameManager(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGameManager"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameManager* CGameManager::Clone(void* pArg)
{
    CGameManager* pInstance = new CGameManager(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGameManager"));
        Safe_Release(pInstance);
    }
    return pInstance;
}
void CGameManager::Free()
{
    __super::Free();
}
