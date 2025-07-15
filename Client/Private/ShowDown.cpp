#include "stdafx.h"
#include "..\Public\ShowDown.h"

#include "GameInstance.h"

CShowDown::CShowDown(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CShowDown::CShowDown(const CShowDown& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CShowDown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShowDown::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 800;
	m_fSizeY = 160;

	m_fX = g_iWinSizeX >> 1;
	m_fY = -200;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_pGameInstance->SoundPlay(L"Ingame_StartGameDelay.wav", (_uint)CHANNELID::SOUND_GAMEPLAY_DELAY, 1.f);

	return S_OK;
}

_uint CShowDown::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CShowDown::Update(_float fTimeDelta)
{
	if (m_fY < (g_iWinSizeY >> 1))
		m_fY += 10.f;

	if (m_fY >= (g_iWinSizeY >> 1))
		m_fDeadTime += fTimeDelta;

	if (m_fDeadTime >= 2.f)
	{
		m_pGameInstance->StopSound((_uint)CHANNELID::SOUND_GAMEPLAY_DELAY);
		m_pGameInstance->SoundPlay(L"Ingame_StartGameGo.wav", (_uint)CHANNELID::SOUND_GAMEPLAY_START, 1.f);
		m_bDead = true;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CShowDown::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CShowDown::Render()
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

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShowDown::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShowDown"),
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

CShowDown* CShowDown::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShowDown* pInstance = new CShowDown(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShowDown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CShowDown::Clone(void* pArg)
{
	CShowDown* pInstance = new CShowDown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShowDown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShowDown::Free()
{
	__super::Free();
}
