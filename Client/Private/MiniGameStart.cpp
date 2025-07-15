#include "stdafx.h"
#include "..\Public\MiniGameStart.h"

CMiniGameStart::CMiniGameStart(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CMiniGameStart::CMiniGameStart(const CMiniGameStart& Prototype)
	: CUI(Prototype)
{
}

HRESULT CMiniGameStart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGameStart::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_DESC* pDesc = static_cast<CUI::UI_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	m_fKeyDownDelay = 0.5f;

	return S_OK;
}

_uint CMiniGameStart::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CMiniGameStart::Update(_float fTimeDelta)
{
	POINT       ptMouse{};

	GetCursorPos(&ptMouse);
	/* 뷰포트 == 로컬 * 월드행렬 * 뷰행렬 * 투영행렬 /w -> 뷰포트 변환 */
	ScreenToClient(g_hWnd, &ptMouse);

	if (ptMouse.x > m_fX - m_fSizeX / 2 && ptMouse.x < m_fX + m_fSizeX / 2
		&& ptMouse.y > m_fY - m_fSizeY / 2 && ptMouse.y < m_fY + m_fSizeY / 2)
	{
		if (m_pGameInstance->Key_Down(VK_LBUTTON) && m_fKeyDownDelay > 0.5f)
		{
		}
		m_bPicking = true;
	}
	else
		m_bPicking = false;
}

void CMiniGameStart::Late_Update(_float fTimeDelta)
{
	m_fKeyDownDelay += fTimeDelta;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CMiniGameStart::Render()
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

HRESULT CMiniGameStart::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Arrow"),
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

HRESULT CMiniGameStart::SetUp_RenderState()
{
	if (FAILED(__super::SetUp_RenderState()))
		return E_FAIL;

	if (true == m_bPicking)
		m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CMiniGameStart::Reset_RenderState()
{
	if (FAILED(__super::Reset_RenderState()))
		return E_FAIL;

	if (true == m_bPicking)
		m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

CMiniGameStart* CMiniGameStart::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMiniGameStart* pInstance = new CMiniGameStart(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMiniGameStart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMiniGameStart::Clone(void* pArg)
{
	CMiniGameStart* pInstance = new CMiniGameStart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMiniGameStart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGameStart::Free()
{
	__super::Free();
}
