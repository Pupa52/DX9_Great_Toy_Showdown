#include "stdafx.h"
#include "ProgressArrow.h"
#include "Layer.h"
#include "Storm.h"

CProgressArrow::CProgressArrow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CProgressArrow::CProgressArrow(const CProgressArrow& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CProgressArrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProgressArrow::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 20;
	m_fSizeY = 20;

	m_fX = (g_iWinSizeX >> 1) - 342.f;
	m_fY = 35.f;

	m_fMinRange = (g_iWinSizeX >> 1) - 342.f;
	m_fMaxRange = (g_iWinSizeX >> 1) + 342.f;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

_uint CProgressArrow::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CProgressArrow::Update(_float fTimeDelta)
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Storm"));
	if (nullptr == pLayer)
		return;

	CStorm* pStorm = static_cast<CStorm*>(pLayer->Get_Objects().front());

	_float fCurRange = 85.f - pStorm->Get_Radius();

	m_fX = m_fMinRange + fCurRange * (m_fMaxRange / 85.f);
	if (m_fX >= m_fMaxRange)
		m_fX = m_fMaxRange;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CProgressArrow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CProgressArrow::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture()))
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

HRESULT CProgressArrow::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ProgressArrow"),
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

CProgressArrow* CProgressArrow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CProgressArrow* pInstance = new CProgressArrow(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CProgressArrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProgressArrow::Clone(void* pArg)
{
	CProgressArrow* pInstance = new CProgressArrow(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CProgressArrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CProgressArrow::Free()
{
	__super::Free();
}
