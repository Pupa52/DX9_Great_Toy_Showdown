#include "stdafx.h"
#include "Item_Teleport.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect.h"

CItem_Teleport::CItem_Teleport(CPlayer* pPlayer)
	: CItem_Possesion(pPlayer)
{
}

HRESULT CItem_Teleport::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pPlayerTransformCom = static_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransformCom);

	m_fCoolTime = 5.f;
	m_fCheckTime = 5.f;

	return S_OK;
}

void CItem_Teleport::Priority_Update(_float fTimeDelta)
{
}

void CItem_Teleport::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('1') && m_fCheckTime > m_fCoolTime)
	{
		CEffect::EFFECT_DESC desc = {};
		desc.m_Pos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
		desc.m_Pos.y += .5f;
		for(size_t i = 0; i < 16; ++i)
		{
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Feather_Effect"), &desc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Smoke_Effect"), &desc);
		}

		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_SKILL));
		m_pGameInstance->SoundPlay(L"sfx_MagicianMonster_Teleport.wav", _uint(CHANNELID::SOUND_PLAYER_SKILL), .5f);
		
		m_pPlayerTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayer->Get_TargetPos());

		desc.m_Pos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
		desc.m_Pos.y += .5f;
		for(size_t i = 0; i < 16; ++i)
		{
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Feather_Effect"), &desc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Smoke_Effect"), &desc);
		}

		m_fCheckTime = 0.f;
	}
}

void CItem_Teleport::Late_Update(_float fTimeDelta)
{
	m_fCheckTime += fTimeDelta;
}

HRESULT CItem_Teleport::Ready_Component()
{
	m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Meteor")));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

CItem_Teleport* CItem_Teleport::Create(CPlayer* pPlayer)
{
	CItem_Teleport* pInstance = new CItem_Teleport(pPlayer);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CItem_Teleport"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Teleport::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pPlayerTransformCom);
}
