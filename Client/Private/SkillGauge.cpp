#include "stdafx.h"
#include "SkillGauge.h"

#include "GameInstance.h"

CSkillGauge::CSkillGauge(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI { pGraphic_Device }
{
}

CSkillGauge::CSkillGauge(const CSkillGauge& Prototype)
	: CUI { Prototype }
{
}

HRESULT CSkillGauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkillGauge::Initialize(void* pArg)
{
	m_tUIDesc = *static_cast<CUI::UI_DESC*>(pArg);
	m_pPlayer = dynamic_cast<CPlayer*>(m_tUIDesc.pOwner);

	switch (m_pPlayer->Get_Character())
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		m_iCharacter = 3;
		break;
	case PLAYER_CHARACTER::PLAYER_DOG:
		m_iCharacter = 4;
		break;
	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		m_iCharacter = 5;
		break;
	default:
		break;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 110;
	m_fSizeY = 110;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY - 70;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

_uint CSkillGauge::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CSkillGauge::Update(_float fTimeDelta)
{
	_float fCurCool = m_pPlayer->Get_CurSkillCoolTime();
	_float fMaxCool = m_pPlayer->Get_SkillCoolTime();

	m_fCurPer = fCurCool / fMaxCool;

	m_fCurPer = m_fCurPer >= 1.f ? 1.f : m_fCurPer;
}

void CSkillGauge::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CSkillGauge::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	// background
	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	m_pTransformCom->Scaling(m_fSizeX * m_fCurPer + 0.0001f, m_fSizeY * m_fCurPer + 0.0001f, 1.f);
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	// gauge
	if (FAILED(m_pTextureCom->Bind_Texture(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	// cover
	if (FAILED(m_pTextureCom->Bind_Texture(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// icon
	if (FAILED(m_pTextureCom->Bind_Texture(m_iCharacter)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillGauge::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillGauge::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 230);
	return S_OK;
}

CSkillGauge* CSkillGauge::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkillGauge* pInstance = new CSkillGauge(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkillGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillGauge::Clone(void* pArg)
{
	CSkillGauge* pInstance = new CSkillGauge(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkillGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillGauge::Free()
{
	__super::Free();
}
