#include "stdafx.h"
#include "State_MoveAttack.h"
#include "FSM.h"
#include "Part.h"
#include "Transform.h"

#include "State_Idle.h"
#include "State_Attack.h"

CState_MoveAttack::CState_MoveAttack(class CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
    : CPlayer_State{ pParts, iNumParts, pTransform, fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill, 
    pHp, iBulletCount, pLevel, pAttack, iMaxHp, iPlayer, eLevelID }
{

}

HRESULT CState_MoveAttack::StartState()
{
    __super::StartState();

    for (size_t i = 0; i < m_iNumParts; ++i)
        m_pParts[i]->Set_Animation_Move();

    return S_OK;
}

void CState_MoveAttack::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        if (m_pParts[i]->Dir_Changed())
            m_pParts[i]->Set_Animation_Move();
    }

}

void CState_MoveAttack::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    Fire_Bullet(fTimeDelta);
}

void CState_MoveAttack::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CState_MoveAttack::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CState_MoveAttack::ExitState()
{

    return S_OK;
}

CState_MoveAttack* CState_MoveAttack::Create(CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
{
    CState_MoveAttack* pInstance = new CState_MoveAttack(pParts, iNumParts, pTransform, fFireDelay, 
        pTargetPos, iPlayerCharacter, pUseSkill, pHp, iBulletCount, pLevel, pAttack, iMaxHp, iPlayer, eLevelID);

    if (FAILED(pInstance->StartState()))
    {
        Safe_Release(pInstance);
        pInstance = nullptr;
    }

    return pInstance;
}

void CState_MoveAttack::Free()
{
    __super::Free();

}
