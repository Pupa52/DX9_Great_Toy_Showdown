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
	// �õ尪�� ��� ���� ��¥ ���� ����.
	random_device rd;

	// �޸��� Ʈ������ ���� ���� ���� �˰���(��ü�� ŭ(2kb �̻�), �޸� �����ϸ� minstd_rand()�� ��������...?)
	// mt19937
	minstd_rand gen(rd());

	// (a, b) ������ ������ ���� ���� ����
	uniform_int_distribution<int> dis(_first, _second);

	return dis(gen);
}


void CItem_Possesion::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
