#include "stdafx.h"
#include "Player_State.h"
#include "FSM.h"
#include "Part.h"
#include "Transform.h"
#include "GameInstance.h"

#include "Player_Bullet.h"

#include "GumBoom.h"
#include "CarrotBoom.h"
#include "Shield.h"
#include "Effect.h"
#include "Bot.h"

CPlayer_State::CPlayer_State(class CPart** pParts, _uint iNumParts, CTransform* pTransform,  _float& fFireDelay, 
    _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
    : m_pParts(pParts)
    , m_iNumParts(iNumParts)
    , m_pTransformCom(pTransform)
    , m_fFireDelay(fFireDelay)
    , m_pTargetPos(pTargetPos)
    , m_iPlayerCharacter(iPlayerCharacter)
    , m_pUseSkill(pUseSkill)
    , m_pHp(pHp)
    , m_iBulletCount(iBulletCount)
    , m_pLevel(pLevel)
    , m_pAttack(pAttack)
    , m_iMaxHp(iMaxHp)
    , m_iPlayer(iPlayer)
    , m_eLevelID(eLevelID)
    , m_pGameInstance(CGameInstance::Get_Instance())
{
    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        Safe_AddRef(m_pParts[i]);
    }
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pGameInstance);
    
}

HRESULT CPlayer_State::StartState()
{
    switch(m_iPlayerCharacter)
    {
    case (_uint)PLAYER_CHARACTER::PLAYER_CAT:
        m_fFireLimit = 0.5f;    // 발사 간격
        m_fFireDistance = 0.3f; // 총구 위치
        break;

    case (_uint)PLAYER_CHARACTER::PLAYER_DOG:
        m_fFireLimit = 0.2f;
        m_fFireDistance = 0.4f;
        break;

    case (_uint)PLAYER_CHARACTER::PLAYER_PENGUIN:
        m_fFireLimit = 0.4f;
        m_fFireDistance = 0.5f;
        break;

    default:
        return E_FAIL;
    }
    return S_OK;
}

void CPlayer_State::Priority_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        m_pParts[i]->Priority_Update(fTimeDelta);
    }

    if (*m_pLevel > 10)
        m_iRank = 2;
    else if (*m_pLevel > 5)
        m_iRank = 1;
    else
        m_iRank = 0;
}

void CPlayer_State::Update(_float fTimeDelta)
{
    _wstring strSkillLayer = TEXT("");

    if (m_iPlayer == ID_PLAYER)
        strSkillLayer = TEXT("Layer_Player_Skill");
    else if(m_iPlayer == FIRST_BOT)
        strSkillLayer = TEXT("Layer_Bot_Skill");
    else if (m_iPlayer == SECOND_BOT)
        strSkillLayer = TEXT("Layer_SECOND_Bot_Skill");
    else if (m_iPlayer == THIRD_BOT)
        strSkillLayer = TEXT("Layer_THIRD_Bot_Skill");

    if (true == *m_pUseSkill)
    {
        switch (m_iPlayerCharacter)
        {
        case (_uint)PLAYER_CHARACTER::PLAYER_CAT:
            if (FAILED(Use_Skill_GumBoom(strSkillLayer)))
                return;
            break;

        case (_uint)PLAYER_CHARACTER::PLAYER_DOG:
            if (FAILED(Use_Skill_Shield(strSkillLayer)))
                return;
            break;

        case (_uint)PLAYER_CHARACTER::PLAYER_PENGUIN:
            if (FAILED(Use_Skill_CarrotBoom(strSkillLayer)))
                return;
            break;

        default:
            break;
        }
        *m_pUseSkill = false;
    }

    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        m_pParts[i]->Update(fTimeDelta);
    }
}

void CPlayer_State::Late_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        m_pParts[i]->Late_Update(fTimeDelta);
    }

}

HRESULT CPlayer_State::Render()
{
    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        m_pParts[i]->Render();
    }
    return S_OK;
}

HRESULT CPlayer_State::ExitState()
{
    return S_OK;
}

HRESULT CPlayer_State::Fire_Bullet(_float fTimeDelta)
{
    if (m_fFireDelay > m_fFireLimit)
    {
        CPlayer_Bullet::PLAYER_BULLET_DESC desc = {};
        
        desc.iAttack = *m_pAttack;
        desc.m_targetPos = *m_pTargetPos;
        desc.fAngle = D3DXToDegree(m_pParts[2]->Get_Angle());
        desc.eLevelID = m_eLevelID;
        desc.eBulletID = (Bullet_ID)m_eBullet_Id;
        _float3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _float3 vTargetDir = desc.m_targetPos - vPlayerPos;
        D3DXVec3Normalize(&vTargetDir, &vTargetDir);

        desc.m_Pos = vPlayerPos + vTargetDir * m_fFireDistance;
        desc.iPlayerCharacter = m_iPlayerCharacter;

        _wstring strBulletLayer = TEXT("");
        //재진 0627 수정

        if (ID_PLAYER == m_iPlayer)
            strBulletLayer = TEXT("Layer_Player_Bullet");
        else if (m_iPlayer == FIRST_BOT)
            strBulletLayer = TEXT("Layer_Bot_Bullet");
        else if (m_iPlayer == SECOND_BOT)
            strBulletLayer = TEXT("Layer_SECOND_Bot_Bullet");
        else if (m_iPlayer == THIRD_BOT)
            strBulletLayer = TEXT("Layer_THIRD_Bot_Bullet");

        switch (m_iPlayerCharacter)
        {
        case (_uint)PLAYER_CHARACTER::PLAYER_CAT:
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, strBulletLayer, TEXT("Prototype_GameObject_Player_Bullet"), &desc)))
                return E_FAIL;
            if (m_iPlayer == ID_PLAYER)
            {
                m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_FIRE));
                m_pGameInstance->SoundPlay(L"GunShot_Candy.wav", _uint(CHANNELID::SOUND_PLAYER_FIRE), .5f);
            }

            break;
        case (_uint)PLAYER_CHARACTER::PLAYER_DOG:
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, strBulletLayer, TEXT("Prototype_GameObject_Player_Bullet"), &desc)))
                return E_FAIL;
            if (m_iPlayer == ID_PLAYER)
            {
                m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_FIRE));
                m_pGameInstance->SoundPlay(L"GunShot_Cola.wav", _uint(CHANNELID::SOUND_PLAYER_FIRE), .5f);
            }

            break;

        case (_uint)PLAYER_CHARACTER::PLAYER_PENGUIN: 
        {
            _float fSpray = { -15.f };
            _float fAngle = desc.fAngle;
            for (size_t i = 0; i < 4; ++i)
            {
                desc.fAngle = fAngle;
                desc.fAngle += fSpray;

                if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, strBulletLayer, TEXT("Prototype_GameObject_Player_Bullet"), &desc)))
                    return E_FAIL;
                if (m_iPlayer == ID_PLAYER)
                {
                    m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_FIRE));
                    m_pGameInstance->SoundPlay(L"GunShot_Carrot.wav", _uint(CHANNELID::SOUND_PLAYER_FIRE), .5f);
                }
                fSpray += 10.f;
            }
        }
            break;

        default:
            return E_FAIL;
        }
        m_fFireDelay = 0.f;
        *m_iBulletCount -= 1;

        CEffect::EFFECT_DESC tEffectDesc = {};
        tEffectDesc.m_Pos = desc.m_Pos;
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Bullet_Fire"), &tEffectDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer_State::Use_Skill_GumBoom(_wstring strSkillLayerTag)
{
    CGumBoom::SKILL_GUM_DESC desc = {};

    desc.iRank = m_iRank;
    desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    desc.vTargetPos = *m_pTargetPos;
    desc.eLevelID = m_eLevelID;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, strSkillLayerTag, TEXT("Prototype_GameObject_PlayerSkill_GumBoom"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_State::Use_Skill_Shield(_wstring strSkillLayerTag)
{
    CShield::SKILL_SHIELD_DESC desc = {};

    desc.iRank = m_iRank;
    desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    desc.vTargetPos = *m_pTargetPos;
    desc.pHp = m_pHp;
    desc.pMaxHp = m_iMaxHp;
    desc.pPlayerTransform = m_pTransformCom;
    desc.eLevelID = m_eLevelID;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, strSkillLayerTag, TEXT("Prototype_GameObject_PlayerSkill_Shield"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_State::Use_Skill_CarrotBoom(_wstring strSkillLayerTag)
{
    CGumBoom::SKILL_GUM_DESC desc = {};

    desc.iRank = m_iRank;
    desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    desc.vTargetPos = *m_pTargetPos;
    desc.eLevelID = m_eLevelID;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_eLevelID, strSkillLayerTag, TEXT("Prototype_GameObject_PlayerSkill_CarrotBoom"), &desc)))
        return E_FAIL;

    return S_OK;
}



void CPlayer_State::Free()
{
    __super::Free();

    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        Safe_Release(m_pParts[i]);
    }
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pGameInstance);
}
