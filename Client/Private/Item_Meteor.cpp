#include "stdafx.h"
#include "Item_Meteor.h"

#include "GameInstance.h"
#include "Player.h"
#include "Bullet.h"

CItem_Meteor::CItem_Meteor(CPlayer* pPlayer)
	: CItem_Possesion(pPlayer)
{
}

HRESULT CItem_Meteor::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	
	m_fCoolTime = 3.f;

	return S_OK;
}

void CItem_Meteor::Priority_Update(_float fTimeDelta)
{

}

void CItem_Meteor::Update(_float fTimeDelta)
{
	if (m_fCheckTime > m_fCoolTime)
	{
		// ¿©±â¼­ ¶³±¸´Â °Å.
		_int iMoveX = GetRandomNum(1, 3);
		_int iMoveZ = GetRandomNum(1, 3);
		if (GetRandomNum(0, 1))
			iMoveX *= -1;
		if (GetRandomNum(0, 1))
			iMoveZ *= -1;

		_float3 vDropPos = m_pPlayer->Get_Pos();
		vDropPos.x += (_float)iMoveX;
		vDropPos.z += (_float)iMoveZ;

		// ¿©±â´Ù ¶³±¸¸é µÊ.
		CBullet::TAG_BULLET_DESC desc = {};
		desc.m_targetPos = vDropPos;

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_ItemBullet"), 
			TEXT("Prototype_GameObject_MeteorBullet"), (void*) & desc);


		m_fCheckTime = 0.f;
	}
}

void CItem_Meteor::Late_Update(_float fTimeDelta)
{
	m_fCheckTime += fTimeDelta;
}

HRESULT CItem_Meteor::Ready_Component()
{
	m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Meteor")));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

CItem_Meteor* CItem_Meteor::Create(CPlayer* pPlayer)
{
	CItem_Meteor* pInstance = new CItem_Meteor(pPlayer);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CItem_Meteor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Meteor::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
}
