#include "stdafx.h"
#include "Item_Missile.h"

#include "GameInstance.h"
#include "Player.h"
#include "Bullet.h"

CItem_Missile::CItem_Missile(CPlayer* pPlayer)
	: CItem_Possesion(pPlayer)
{
}

HRESULT CItem_Missile::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	m_iBulletCount = m_pPlayer->Get_BulletCount();

	return S_OK;
}

void CItem_Missile::Priority_Update(_float fTimeDelta)
{
}

void CItem_Missile::Update(_float fTimeDelta)
{
	if (m_iBulletCount - m_pPlayer->Get_BulletCount() > 1 && false == m_pPlayer->Get_Reloading())
	{
		// 여기서 발사
		m_pPlayer->Get_Pos();	// 여기서

		CBullet::TAG_BULLET_DESC desc = {};
		desc.m_Pos = m_pPlayer->Get_Pos();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_ItemBullet"),
			TEXT("Prototype_GameObject_MissileBullet"), &desc)))
		{
			int stop = 0;
			return;
		}

		m_iBulletCount = m_pPlayer->Get_BulletCount();
	}


	if (true == m_pPlayer->Get_Reloading() && m_iBulletCount < m_pPlayer->Get_TotalBulletCount())
	{
		m_iBulletCount += m_pPlayer->Get_TotalBulletCount();
	}
}

void CItem_Missile::Late_Update(_float fTimeDelta)
{

}

HRESULT CItem_Missile::Ready_Component()
{
	m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Meteor")));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

CItem_Missile* CItem_Missile::Create(CPlayer* pPlayer)
{
	CItem_Missile* pInstance = new CItem_Missile(pPlayer);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CItem_Missile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Missile::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
}
