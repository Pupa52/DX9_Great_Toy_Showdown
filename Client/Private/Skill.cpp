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
	// �õ尪�� ��� ���� ��¥ ���� ����.
	random_device rd;

	// �޸��� Ʈ������ ���� ���� ���� �˰���(��ü�� ŭ(2kb �̻�), �޸� �����ϸ� minstd_rand()�� ��������...?)
	// mt19937
	minstd_rand gen(rd());

	// (a, b) ������ ������ ���� ���� ����
	uniform_int_distribution<int> dis(_first, _second);

	return dis(gen);
}

void CSkill::Free()
{
    __super::Free();
}
