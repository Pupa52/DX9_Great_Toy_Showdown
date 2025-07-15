#include "Collider_Rect.h"

CCollider_Rect::CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CCollider { pGraphic_Device }
{
}

CCollider_Rect::CCollider_Rect(const CCollider_Rect& Prototype)
    : CCollider { Prototype }
{
}

HRESULT CCollider_Rect::Initialize_Prototype()
{
    D3DXMatrixIdentity(&m_WorldMatrix);

    m_iVertexStride = sizeof(VTXTEX);
    m_iNumVertices = 4;
    m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
    m_iNumFaces = 2;
    m_iIndexStride = 2;
    m_iNumIndices = 6;
    m_eIndexFormat = D3DFMT_INDEX16;

    /* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
    if (FAILED(__super::Ready_VertexBuffer()))
        return E_FAIL;

    VTXTEX* pVertices = { nullptr };
    /* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
    m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.f, 1.f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.f, 1.f);

    m_pVB->Unlock();

    if (FAILED(Ready_IndexBuffer()))
        return E_FAIL;

    _ushort* pIndices = { nullptr };

    m_pIB->Lock(0, 0, (void**)&pIndices, 0);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;
    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    m_pIB->Unlock();

    return S_OK;
}

HRESULT CCollider_Rect::Initialize(void* pArg)
{
    m_WorldMatrix = *static_cast<_float4x4*>(pArg);
    return S_OK;
}

CCollider_Rect* CCollider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCollider_Rect* pInstance = new CCollider_Rect(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCollider_Rect* CCollider_Rect::Clone(void* pArg)
{
    CCollider_Rect* pInstance = new CCollider_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCollider_Rect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Rect::Free()
{
    __super::Free();
}
