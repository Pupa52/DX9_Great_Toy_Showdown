#include "stdafx.h"
#include "Item_Possesion.h"

#include "GameInstance.h"
#include "Player.h"
#include "random"

CItem_Possesion::CItem_Possesion(CPlayer* pPlayer)
	: m_pPlayer(pPlayer)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CItem_Possesion::Initialize()
{
	return S_OK;
}

void CItem_Possesion::Priority_Update(_float fTimeDelta)
{
}

void CItem_Possesion::Update(_float fTimeDelta)
{
}

void CItem_Possesion::Late_Update(_float fTimeDelta)
{
}

int CItem_Possesion::GetRandomNum(int _first, int _second)
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


void CItem_Possesion::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
