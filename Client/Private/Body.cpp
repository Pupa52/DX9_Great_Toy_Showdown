#include "stdafx.h"
#include "Body.h"

CBody::CBody(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPart(pGraphic_Device)
{
}

HRESULT CBody::Initialize(_uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	__super::Initialize(iLevelIndex, pArg, ePlayerID);

	if (FAILED(Ready_Components(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CBody::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	Inverse_Vertex();
}

void CBody::Update(_float fTimeDelta)
{
	if (m_bInverse)
		m_pTransformPart->Scaling(-1.4f, 1.4f, 1.4f);
}

void CBody::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CBody::Render()
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

HRESULT CBody::Set_Animation_Attack()
{
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Attack_Body"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.01f };
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Attack_Body"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.03f };
	}
	
	return S_OK;
}

HRESULT CBody::Set_Animation_Idle()
{
	
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Idle_Body"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.01f };
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Idle_Body"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.03f };
	}

	return S_OK;
}

HRESULT CBody::Set_Animation_Move()
{
	if (PLAYER_DIR::LEFTDOWN == m_eCurDir || PLAYER_DIR::RIGHTDOWN == m_eCurDir)
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Front_Move_Body"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.01f };
	}
	else
	{
		if (FAILED(m_pAnimatorCom->Change_Texture(TEXT("Prototype_Component_Texture2D_Back_Move_Body"))))
			return E_FAIL;
		m_fDist = { 0.f, 0.f, 0.03f };
	}

	return S_OK;
}


HRESULT CBody::Ready_Components(_uint iLevelIndex)
{
	__super::Ready_Components(iLevelIndex);

	wstring strTagCharacterBody[6] = {};

	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		strTagCharacterBody[0] = TEXT("Prototype_Component_Texture2D_Cat_Front_Attack_Body");
		strTagCharacterBody[1] = TEXT("Prototype_Component_Texture2D_Cat_Back_Attack_Body");
		strTagCharacterBody[2] = TEXT("Prototype_Component_Texture2D_Cat_Front_Idle_Body");
		strTagCharacterBody[3] = TEXT("Prototype_Component_Texture2D_Cat_Back_Idle_Body");
		strTagCharacterBody[4] = TEXT("Prototype_Component_Texture2D_Cat_Front_Move_Body");
		strTagCharacterBody[5] = TEXT("Prototype_Component_Texture2D_Cat_Back_Move_Body");
		break;
	case PLAYER_CHARACTER::PLAYER_DOG:
		strTagCharacterBody[0] = TEXT("Prototype_Component_Texture2D_Dog_Front_Attack_Body");
		strTagCharacterBody[1] = TEXT("Prototype_Component_Texture2D_Dog_Back_Attack_Body");
		strTagCharacterBody[2] = TEXT("Prototype_Component_Texture2D_Dog_Front_Idle_Body");
		strTagCharacterBody[3] = TEXT("Prototype_Component_Texture2D_Dog_Back_Idle_Body");
		strTagCharacterBody[4] = TEXT("Prototype_Component_Texture2D_Dog_Front_Move_Body");
		strTagCharacterBody[5] = TEXT("Prototype_Component_Texture2D_Dog_Back_Move_Body");
		break;
	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		strTagCharacterBody[0] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Attack_Body");
		strTagCharacterBody[1] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Attack_Body");
		strTagCharacterBody[2] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Idle_Body");
		strTagCharacterBody[3] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Idle_Body");
		strTagCharacterBody[4] = TEXT("Prototype_Component_Texture2D_Penguin_Front_Move_Body");
		strTagCharacterBody[5] = TEXT("Prototype_Component_Texture2D_Penguin_Back_Move_Body");
		break;
	default:
		return E_FAIL;
	}

	CTexture* Front_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterBody[0], nullptr));
	CTexture* Back_Attack_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterBody[1], nullptr));

	CTexture* Front_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterBody[2], nullptr));
	CTexture* Back_Idle_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterBody[3], nullptr));

	CTexture* Front_Move_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterBody[4], nullptr));
	CTexture* Back_Move_Body = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC,
		strTagCharacterBody[5], nullptr));

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Attack_Body"), Front_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Idle_Body"), Front_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Front_Move_Body"), Front_Move_Body);

	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Attack_Body"), Back_Attack_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Idle_Body"), Back_Idle_Body);
	m_pAnimatorCom->Add_Texture(TEXT("Prototype_Component_Texture2D_Back_Move_Body"), Back_Move_Body);

	return S_OK;
}



CBody* CBody::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iLevelIndex, void* pArg, PLAYER_CHARACTER ePlayerID)
{
	CBody* pInstance = new CBody(pGraphic_Device);

	if (FAILED(pInstance->Initialize(iLevelIndex, pArg, ePlayerID)))
	{
		MSG_BOX(TEXT("Create Failed : CBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody::Free()
{
	__super::Free();
}

