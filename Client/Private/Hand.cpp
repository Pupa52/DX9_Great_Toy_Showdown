#include "stdafx.h"
#include "../Public/Hand.h"

CHand::CHand(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPart(pGraphic_Device)
{
}

HRESULT CHand::Initialize(_uint iLevelIndex, void* pArg)
{
	__super::Initialize(iLevelIndex, pArg);

	if (FAILED(Ready_Components(iLevelIndex)))
		return E_FAIL;
	
	// юс╫ц
	m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Hand"));

	return S_OK;
}

void CHand::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CHand::Update(_float fTimeDelta)
{
}

void CHand::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CHand::Render()
{


	return S_OK;
}


HRESULT CHand::Ready_Components(_uint iLevelIndex)
{
	__super::Ready_Components(iLevelIndex);

	CTexture* Cat_Front_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iLevelIndex,
		TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Body"), nullptr));
	CTexture* Cat_Back_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iLevelIndex,
		TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Body"), nullptr));

	CTexture* Cat_Front_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iLevelIndex,
		TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Body"), nullptr));
	CTexture* Cat_Back_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iLevelIndex,
		TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Body"), nullptr));

	CTexture* Cat_Front_Walk_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iLevelIndex,
		TEXT("Prototype_Component_Texture2D_Cat_Front_Walk_Body"), nullptr));
	CTexture* Cat_Back_Walk_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(iLevelIndex,
		TEXT("Prototype_Component_Texture2D_Cat_Back_Walk_Body"), nullptr));

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Body"), Cat_Front_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Body"), Cat_Back_Attack_Body);

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Body"), Cat_Front_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Body"), Cat_Back_Idle_Body);

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Cat_Front_Walk_Body"), Cat_Front_Walk_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Cat_Back_Walk_Body"), Cat_Back_Walk_Body);

	return S_OK;
}

CHand* CHand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return new CHand(pGraphic_Device);
}

void CHand::Free()
{
	__super::Free();
}

HRESULT CHand::Set_Animation_Attack()
{
	return E_NOTIMPL;
}

HRESULT CHand::Set_Animation_Idle()
{
	return E_NOTIMPL;
}

HRESULT CHand::Set_Animation_Walk()
{
	return E_NOTIMPL;
}
