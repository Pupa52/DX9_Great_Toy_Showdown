#include "stdafx.h"
#include "State_Idle.h"
#include "FSM.h"
#include "Part.h"
#include "Transform.h"

#include "State_Attack.h"
#include "State_Move.h"

CState_Idle::CState_Idle(class CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
    : CPlayer_State{ pParts, iNumParts, pTransform, fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill,
    pHp, iBulletCount, pLevel, pAttack, iMaxHp, iPlayer, eLevelID }
{
}

HRESULT CState_Idle::StartState()
{
    for (size_t i = 0; i < m_iNumParts; ++i)
        m_pParts[i]->Set_Animation_Idle();


    return S_OK;
}

void CState_Idle::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    for (size_t i = 0; i < m_iNumParts; ++i)
    {
        if (m_pParts[i]->Dir_Changed())
            m_pParts[i]->Set_Animation_Idle();
    }
}

void CState_Idle::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CState_Idle::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);


}

HRESULT CState_Idle::Render()
{
    __super::Render();

    return S_OK;
}


HRESULT CState_Idle::ExitState()
{
    return S_OK;
}

CState_Idle* CState_Idle::Create(CPart** pParts, _uint iNumParts, CTransform* pTransform, 
    _float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
    _int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID)
{
    CState_Idle* pInstance = new CState_Idle(pParts, iNumParts, pTransform, 
        fFireDelay, pTargetPos, iPlayerCharacter, pUseSkill, pHp, 
        iBulletCount, pLevel, pAttack, iMaxHp, iPlayer, eLevelID);

    if (FAILED(pInstance->StartState()))
    {
        Safe_Release(pInstance);
        pInstance = nullptr;
    }

    return pInstance;
}

void CState_Idle::Free()
{
    __super::Free();

}
