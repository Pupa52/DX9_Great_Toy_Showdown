#include "stdafx.h"
#include "Item_Passive.h"

#include "GameInstance.h"
#include "Item.h"
#include "Player.h"

CItem_Passive::CItem_Passive(CPlayer* pPlayer, _uint iItemType)
	: CItem_Possesion(pPlayer)
	, m_iItemType(iItemType)
{
}

HRESULT CItem_Passive::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_iAttack = 10;
	m_fSpeed = 1.f;
	m_iBullet = 2;

	switch (m_iItemType)
	{
	case (_uint)CItem::ITEM_SWORD:
		m_pPlayer->Set_Attack(m_pPlayer->Get_Attack() + m_iAttack);
		break;

	case (_uint)CItem::ITEM_SKATE:
		m_pPlayer->Set_Speed(m_pPlayer->Get_Speed() + m_fSpeed);
		break;

	case (_uint)CItem::ITEM_MAGAZINE:
		m_pPlayer->Set_TotalBulletCount(m_pPlayer->Get_TotalBulletCount() + m_iBullet);
		break;

	default:
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CItem_Passive::Ready_Component()
{
	wstring strPrototypeTag = TEXT("");

	switch (m_iItemType)
	{
	case (_uint)CItem::ITEM_SWORD:
		strPrototypeTag = TEXT("Prototype_Component_Texture_Item_Sword");
		break;

	case (_uint)CItem::ITEM_SKATE:
		strPrototypeTag = TEXT("Prototype_Component_Texture_Item_Skate");
		break;

	case (_uint)CItem::ITEM_MAGAZINE:
		strPrototypeTag = TEXT("Prototype_Component_Texture_Item_Magazine");
		break;

	default:
		return E_FAIL;
	}

	m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, strPrototypeTag));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

CItem_Passive* CItem_Passive::Create(CPlayer* pPlayer, _uint iItemType)
{
	CItem_Passive* pInstance = new CItem_Passive(pPlayer, iItemType);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create_Failed : CItem_Passive"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CItem_Passive::Free()
{
	__super::Free();

	switch (m_iItemType)
	{
	case (_uint)CItem::ITEM_SWORD:
		m_pPlayer->Set_Attack(m_pPlayer->Get_Attack() - m_iAttack);
		break;

	case (_uint)CItem::ITEM_SKATE:
		m_pPlayer->Set_Speed(m_pPlayer->Get_Speed() - m_fSpeed);
		break;

	case (_uint)CItem::ITEM_MAGAZINE:
		m_pPlayer->Set_TotalBulletCount(m_pPlayer->Get_TotalBulletCount() - m_iBullet);
		break;

	default:
		return;
	}

	Safe_Release(m_pTextureCom);
}
