#include "stdafx.h"
#include "..\Public\BotHpBar.h"

#include "GameInstance.h"

CBotHpBar::CBotHpBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CBotHpBar::CBotHpBar(const CBotHpBar& Prototype)
	: CUI(Prototype)
{
}

HRESULT CBotHpBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBotHpBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_tUIDesc = *static_cast<CUI::UI_DESC*>(pArg);
	m_pActor = dynamic_cast<CActor*>(m_tUIDesc.pOwner);
	m_pOwnerTransformCom = dynamic_cast<CTransform*>(m_pActor->Find_Component(TEXT("Com_Transform")));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_eType = m_tUIDesc.iNum == 1 ? BOT_TYPE::FRIEND : BOT_TYPE::ENEMY;

	m_fSizeX = 120;
	m_fSizeY = 20;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

_uint CBotHpBar::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CBotHpBar::Update(_float fTimeDelta)
{
	_float4x4		ViewMatrix{};
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	_float3		vOwnerPos = m_pOwnerTransformCom->Get_State(CTransform::STATE_POSITION);

	D3DXVec3TransformCoord(&vOwnerPos, &vOwnerPos, &ViewMatrix);
	D3DXVec3TransformCoord(&vOwnerPos, &vOwnerPos, &ProjMatrix);

	_float fOffset = 70.f;
	m_fX = (vOwnerPos.x + 1) * (g_iWinSizeX * 0.5f) + 5.f;
	m_fY = (-vOwnerPos.y + 1) * (g_iWinSizeY * 0.5f) - fOffset;

	CActor::ACTOR_DESC tDesc = m_pActor->Get_ActorDesc();
	_int iHp = tDesc.iHp <= 0 ? 0 : tDesc.iHp;
	_int iMaxHp = tDesc.iMaxHp;

	m_fCurHpPer = (iHp * 1.f) / iMaxHp;

	if (iHp <= 0 && m_tUIDesc.iType != LEVEL_MINIGAME)
		m_bDead = true;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CBotHpBar::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CBotHpBar::Render()
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

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float fMovePos = m_fSizeX * (1.f - m_fCurHpPer);

	m_pTransformCom->Scaling(m_fSizeX * m_fCurHpPer + 0.0001f, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - (fMovePos * 0.5f) - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_eType)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBotHpBar::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Hp_Bar"),
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

CBotHpBar* CBotHpBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBotHpBar* pInstance = new CBotHpBar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBotHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBotHpBar::Clone(void* pArg)
{
	CBotHpBar* pInstance = new CBotHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBotHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBotHpBar::Free()
{
	__super::Free();
}
