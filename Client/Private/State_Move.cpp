#include "stdafx.h"
#include "State_Move.h"
#include "FSM.h"
#include "Part.h"
#include "Transform.h"

#include "State_Idle.h"
#include "State_Attack.h"

CState_Move::CState_Move(class CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
    : CPlayer_State{ pParts, iNumParts, pTransform, fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill, 
    pHp, iBulletCount, pLevel, pAttack, iMaxHp,  iPlayer, eLevelID }
{

}

HRESULT CState_Move::StartState()
{

    for (size_t i = 0; i < m_iNumParts; ++i)
        m_pParts[i]->Set_Animation_Move();

    return S_OK;
}

void CState_Move::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        if (m_pParts[i]->Dir_Changed())
            m_pParts[i]->Set_Animation_Move();
    }

}

void CState_Move::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CState_Move::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CState_Move::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CState_Move::ExitState()
{

    return S_OK;
}

CState_Move* CState_Move::Create(CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, _int* pHp, _int* iBulletCount, 
    _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
{
    CState_Move* pInstance = new CState_Move(pParts, iNumParts, pTransform, 
        fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill, pHp, 
        iBulletCount, pLevel, pAttack, iMaxHp,  iPlayer, eLevelID);

    if (FAILED(pInstance->StartState()))
    {
        Safe_Release(pInstance);
        pInstance = nullptr;
    }

    return pInstance;
}

void CState_Move::Free()
{
    __super::Free();

}
