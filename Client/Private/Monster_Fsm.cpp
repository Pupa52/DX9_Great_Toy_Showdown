#include "stdafx.h"
#include "Monster_Fsm.h"

CMonster_Fsm::CMonster_Fsm(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{pGraphic_Device}
{
}

CMonster_Fsm::CMonster_Fsm(const CMonster_Fsm& Prototype)
	:CGameObject{Prototype}
{
}

HRESULT CMonster_Fsm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Fsm::Initialize(void* pArg)
{
	return S_OK;
}

void CMonster_Fsm::Priority_Update(_float fTimeDelta)
{
}

void CMonster_Fsm::Update(_float fTimeDelta)
{
}

void CMonster_Fsm::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster_Fsm::Render()
{
	return S_OK;
}

void CMonster_Fsm::Set_Monster_Fsm(MONSTER_STATE* _State)
{
}
