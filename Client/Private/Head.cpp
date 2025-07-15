#include "stdafx.h"
#include "Head.h"

CHead::CHead(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPart(pGraphic_Device)
{
}

HRESULT CHead::Initialize(_uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	__super::Initialize(iLevelIndex, pArg, ePlayerID);

	if (FAILED(Ready_Components(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CHead::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	Inverse_Vertex();
}

void CHead::Update(_float fTimeDelta)
{
	_float fAngle = m_fAngle;
	if (m_bInverse)
	{
		if (m_eCurDir == PLAYER_DIR::LEFTDOWN)
			fAngle += D3DX_PI;
		else if (m_eCurDir == PLAYER_DIR::LEFTTOP)
			fAngle -= D3DX_PI;

		m_pTransformPart->Rotation(_float3{ 0.f, 0.f, 1.f }, fAngle * 0.2f);
		m_pTransformPart->Scaling(-1.4f, 1.4f, 1.4f);
	}
	else
		m_pTransformPart->Rotation(_float3{ 0.f, 0.f, 1.f }, fAngle * 0.2f);
}

void CHead::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CHead::Render()
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

HRESULT CHead::Set_Animation_Attack()
{
	m_fDist = { 0.f, 0.f, 0.02f };
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Attack_Head"))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Attack_Head"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHead::Set_Animation_Idle()
{
	m_fDist = { 0.f, 0.f, 0.02f };
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Idle_Head"))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Idle_Head"))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CHead::Set_Animation_Move()
{
	m_fDist = { 0.f, 0.f, 0.02f };
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Move_Head"))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Move_Head"))))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CHead::Ready_Components(_uint iLevelIndex)
{
	wstring strTagCharacterHead[6] = {};

	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		strTagCharacterHead[0] = TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Head");
		strTagCharacterHead[1] = TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Head");
		strTagCharacterHead[2] = TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Head");
		strTagCharacterHead[3] = TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Head");
		strTagCharacterHead[4] = TEXT("Prototype_Component_Texture2D_Cat_Front_Move_Head");
		strTagCharacterHead[5] = TEXT("Prototype_Component_Texture2D_Cat_Back_Move_Head");
		break;
	case PLAYER_CHARACTER::PLAYER_DOG:
		strTagCharacterHead[0] = TEXT("Prototype_Component_Texture2D_Dog_Front_Attack_Head");
		strTagCharacterHead[1] = TEXT("Prototype_Component_Texture2D_Dog_Back_Attack_Head");
		strTagCharacterHead[2] = TEXT("Prototype_Component_Texture2D_Dog_Front_Idle_Head");
		strTagCharacterHead[3] = TEXT("Prototype_Component_Texture2D_Dog_Back_Idle_Head");
		strTagCharacterHead[4] = TEXT("Prototype_Component_Texture2D_Dog_Front_Move_Head");
		strTagCharacterHead[5] = TEXT("Prototype_Component_Texture2D_Dog_Back_Move_Head");
		break;
	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		strTagCharacterHead[0] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Attack_Head");
		strTagCharacterHead[1] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Attack_Head");
		strTagCharacterHead[2] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Idle_Head");
		strTagCharacterHead[3] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Idle_Head");
		strTagCharacterHead[4] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Move_Head");
		strTagCharacterHead[5] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Move_Head");
		break;
	default:
		return E_FAIL;
	}

	__super::Ready_Components(iLevelIndex);
	CTexture* Front_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterHead[0], nullptr));
	CTexture* Back_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterHead[1], nullptr));

	CTexture* Front_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterHead[2], nullptr));
	CTexture* Back_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterHead[3], nullptr));

	CTexture* Front_Move_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterHead[4], nullptr));
	CTexture* Back_Move_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterHead[5], nullptr));

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Attack_Head"), Front_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Idle_Head"), Front_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Move_Head"), Front_Move_Body);

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Attack_Head"), Back_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Idle_Head"), Back_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Move_Head"), Back_Move_Body);
	return S_OK;
}

CHead* CHead::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	CHead* pInstance = new CHead(pGraphic_Device);

	if (FAILED(pInstance->Initialize(iLevelIndex, pArg, ePlayerID)))
	{
		MSG_BOX(TEXT("Create Failed : CHead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHead::Free()
{
	__super::Free();
}

