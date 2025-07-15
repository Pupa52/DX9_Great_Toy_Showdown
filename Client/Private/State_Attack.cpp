#include "stdafx.h"
#include "State_Attack.h"
#include "FSM.h"
#include "Part.h"
#include "Transform.h"

#include "State_Idle.h"
#include "State_Move.h"


CState_Attack::CState_Attack(class CPart** pParts, _uint iNumParts, CTransform* pTransform,
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill,
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
    : CPlayer_State{ pParts, iNumParts, pTransform, fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill,
    pHp, iBulletCount, pLevel, pAttack, iMaxHp, iPlayer, eLevelID }
{

}

HRESULT CState_Attack::StartState()
{
    __super::StartState();

    for (size_t i = 0; i < m_iNumParts; ++i)
        m_pParts[i]->Set_Animation_Attack();

    return S_OK;
}

void CState_Attack::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        if(m_pParts[i]->Dir_Changed())
            m_pParts[i]->Set_Animation_Attack();
    }
}

void CState_Attack::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    Fire_Bullet(fTimeDelta);
}

void CState_Attack::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CState_Attack::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CState_Attack::ExitState()
{
    
    return S_OK;
}

CState_Attack* CState_Attack::Create(CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
{
    CState_Attack* pInstance = new CState_Attack(pParts, iNumParts, pTransform, 
        fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill, pHp, 
        iBulletCount, pLevel, pAttack, iMaxHp, iPlayer, eLevelID);

    if (FAILED(pInstance->StartState()))
    {
        Safe_Release(pInstance);
        pInstance = nullptr;
    }

    return pInstance;
}

void CState_Attack::Free()
{
    __super::Free();

}
