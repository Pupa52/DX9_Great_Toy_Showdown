#include "stdafx.h"
#include "Weapon.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPart(pGraphic_Device)
{
}

HRESULT CWeapon::Initialize(_uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	__super::Initialize(iLevelIndex, pArg, ePlayerID);

	if (FAILED(Ready_Components(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	Inverse_Vertex();
}

void CWeapon::Update(_float fTimeDelta)
{
	_float fAngle = m_fAngle;
	if (m_bInverse)
	{
		if (m_eCurDir == PLAYER_DIR::LEFTDOWN)
			fAngle += D3DX_PI;
		else if (m_eCurDir == PLAYER_DIR::LEFTTOP)
			fAngle -= D3DX_PI;

		m_pTransformPart->Rotation(_float3{ 0.f, 0.f, 1.f }, fAngle);
		m_pTransformPart->Scaling(-1.4f, 1.4f, 1.4f);
	}
	else
		m_pTransformPart->Rotation(_float3{ 0.f, 0.f, 1.f }, fAngle);
}

void CWeapon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CWeapon::Render()
{
	if (FAILED(m_pTransformPart->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(__super::SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(__super::Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}
HRESULT CWeapon::Set_Animation_Attack()
{
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Attack_Weapon"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.03f };
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Attack_Weapon"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.01f };
	}

	return S_OK;
}

HRESULT CWeapon::Set_Animation_Idle()
{
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Idle_Weapon"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.03f };
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Idle_Weapon"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.01f };
	}

	return S_OK;
}

HRESULT CWeapon::Set_Animation_Move()
{
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Move_Weapon"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.03f };
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Move_Weapon"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.01f };
	}

	return S_OK;
}
HRESULT CWeapon::Ready_Components(_uint iLevelIndex)
{
	__super::Ready_Components(iLevelIndex);
	wstring strTagCharacterWeapon[6] = {};

	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		strTagCharacterWeapon[0] = TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Weapon");
		strTagCharacterWeapon[1] = TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Weapon");
		strTagCharacterWeapon[2] = TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Weapon");
		strTagCharacterWeapon[3] = TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Weapon");
		strTagCharacterWeapon[4] = TEXT("Prototype_Component_Texture2D_Cat_Front_Move_Weapon");
		strTagCharacterWeapon[5] = TEXT("Prototype_Component_Texture2D_Cat_Back_Move_Weapon");
		break;
	case PLAYER_CHARACTER::PLAYER_DOG:
		strTagCharacterWeapon[0] = TEXT("Prototype_Component_Texture2D_Dog_Front_Attack_Weapon");
		strTagCharacterWeapon[1] = TEXT("Prototype_Component_Texture2D_Dog_Back_Attack_Weapon");
		strTagCharacterWeapon[2] = TEXT("Prototype_Component_Texture2D_Dog_Front_Idle_Weapon");
		strTagCharacterWeapon[3] = TEXT("Prototype_Component_Texture2D_Dog_Back_Idle_Weapon");
		strTagCharacterWeapon[4] = TEXT("Prototype_Component_Texture2D_Dog_Front_Move_Weapon");
		strTagCharacterWeapon[5] = TEXT("Prototype_Component_Texture2D_Dog_Back_Move_Weapon");
		break;
	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		strTagCharacterWeapon[0] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Attack_Weapon");
		strTagCharacterWeapon[1] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Attack_Weapon");
		strTagCharacterWeapon[2] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Idle_Weapon");
		strTagCharacterWeapon[3] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Idle_Weapon");
		strTagCharacterWeapon[4] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Move_Weapon");
		strTagCharacterWeapon[5] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Move_Weapon");
		break;
	default:
		return E_FAIL;
	}

	CTexture* Front_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterWeapon[0], nullptr));
	CTexture* Back_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterWeapon[1], nullptr));

	CTexture* Front_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterWeapon[2], nullptr));
	CTexture* Back_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterWeapon[3], nullptr));

	CTexture* Front_Move_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterWeapon[4], nullptr));
	CTexture* Back_Move_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterWeapon[5], nullptr));

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Attack_Weapon"), Front_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Idle_Weapon"), Front_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Move_Weapon"), Front_Move_Body);

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Attack_Weapon"), Back_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Idle_Weapon"), Back_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Move_Weapon"), Back_Move_Body);
	return S_OK;
}

CWeapon* CWeapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	CWeapon* pInstance = new CWeapon(pGraphic_Device);

	if (FAILED(pInstance->Initialize(iLevelIndex, pArg, ePlayerID)))
	{
		MSG_BOX(TEXT("Create Failed : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();
}
