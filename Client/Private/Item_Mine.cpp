#include "stdafx.h"
#include "Item_Mine.h"

#include "GameInstance.h"
#include "Player.h"
#include "Bullet.h"

CItem_Mine::CItem_Mine(CPlayer* pPlayer)
	: CItem_Possesion(pPlayer)
{
}

HRESULT CItem_Mine::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fCoolTime = 5.f;

	return S_OK;
}

void CItem_Mine::Priority_Update(_float fTimeDelta)
{
}

void CItem_Mine::Update(_float fTimeDelta)
{
	if (m_fCheckTime > m_fCoolTime)
	{
		// 여기서 지뢰 까는 거.
		CBullet::TAG_BULLET_DESC desc = {};
		desc.m_Pos = m_pPlayer->Get_Pos();

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_ItemBullet"),
			TEXT("Prototype_GameObject_MineBullet"), &desc);

		m_fCheckTime = 0.f;
	}
}

void CItem_Mine::Late_Update(_float fTimeDelta)
{
	m_fCheckTime += fTimeDelta;
}

HRESULT CItem_Mine::Ready_Component()
{
	m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Meteor")));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

CItem_Mine* CItem_Mine::Create(CPlayer* pPlayer)
{
	CItem_Mine* pInstance = new CItem_Mine(pPlayer);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CItem_Mine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Mine::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
}
