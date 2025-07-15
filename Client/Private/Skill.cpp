#include "stdafx.h"
#include "Skill.h"
#include "random"


CSkill::CSkill(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CBlendObject(pGraphic_Device)
{
}

CSkill::CSkill(const CSkill& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CSkill::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
    return S_OK;
}

_uint CSkill::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CSkill::Update(_float fTimeDelta)
{
}

void CSkill::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkill::Render()
{
    return S_OK;
}

int CSkill::GetRandomNum(int _first, int _second)
{
	// 시드값을 얻기 위한 진짜 난수 생성.
	random_device rd;

	// 메르센 트위스터 유사 난수 생성 알고리즘(객체가 큼(2kb 이상), 메모리 부족하면 minstd_rand()가 나을수도...?)
	// mt19937
	minstd_rand gen(rd());

	// (a, b) 사이의 균일한 정수 분포 생성
	uniform_int_distribution<int> dis(_first, _second);

	return dis(gen);
}

void CSkill::Free()
{
    __super::Free();
}
