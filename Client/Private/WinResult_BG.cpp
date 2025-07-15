#include "stdafx.h"
#include "..\Public\WinResult_BG.h"

CWinResult_BG::CWinResult_BG(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CWinResult_BG::CWinResult_BG(const CWinResult_BG& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CWinResult_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWinResult_BG::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 1280;
	m_fSizeY = 720;

	m_fX = (g_iWinSizeX >> 1) + g_iWinSizeX;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_fSpeed = 20.f;

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

_uint CWinResult_BG::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CWinResult_BG::Update(_float fTimeDelta)
{
	if ((g_iWinSizeX * 0.5f) >= m_fX)
	{
		m_fX = (g_iWinSizeX * 0.5f);
		return;
	}

	m_fX -= m_fSpeed;
}

void CWinResult_BG::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Move_Frame(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CWinResult_BG::Render()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
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

HRESULT CWinResult_BG::Ready_Components()
{
	/* For.Com_Animator */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Texture(TEXT("BackGround"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_BackGround"))));

	m_pAnimatorCom->Change_Texture(TEXT("BackGround"));

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

CWinResult_BG* CWinResult_BG::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWinResult_BG* pInstance = new CWinResult_BG(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWinResult_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CWinResult_BG::Clone(void* pArg)
{
	CWinResult_BG* pInstance = new CWinResult_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWinResult_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWinResult_BG::Free()
{
	__super::Free();

	Safe_Release(m_pAnimatorCom);
}
