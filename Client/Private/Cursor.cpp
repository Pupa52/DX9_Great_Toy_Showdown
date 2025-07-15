#include "stdafx.h"
#include "Cursor.h"

CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI(pGraphic_Device)
{
}

CCursor::CCursor(const CCursor& Prototype)
    : CUI{ Prototype }
{
}

HRESULT CCursor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCursor::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    ShowCursor(false);

    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixIdentity(&m_ProjMatrix);

    m_fSizeX = 50;
    m_fSizeY = 50;

    m_fX = g_iWinSizeX >> 1;
    m_fY = g_iWinSizeY >> 1;

    m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));

    /* 직교투영행렬을 만들어준다. */
    D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

    return S_OK;
}

_uint CCursor::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CCursor::Update(_float fTimeDelta)
{
    POINT ptMouse{};

    GetCursorPos(&ptMouse);

    ScreenToClient(g_hWnd, &ptMouse);

    m_fX = (_float)ptMouse.x;
    m_fY = (_float)ptMouse.y;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
        -m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CCursor::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CCursor::Render()
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

HRESULT CCursor::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
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

CCursor* CCursor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCursor* pInstance = new CCursor(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCursor"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCursor::Clone(void* pArg)
{
    CCursor* pInstance = new CCursor(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCursor"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCursor::Free()
{
    __super::Free();

    ShowCursor(true);
}
