#include "Culling.h"

#include "GameInstance.h"

CCulling::CCulling(LPDIRECT3DDEVICE9 pGraphic_Device)
    : m_pGraphic_Device { pGraphic_Device }
{
    Safe_AddRef(m_pGraphic_Device);
}

HRESULT CCulling::Initialize()
{
    // front - near ÀÇ Åõ¿µ ÁÂÇ¥
    m_CulVertices[0].vPosition = { -1.f,  1.f, 0.f };
    m_CulVertices[1].vPosition = {  1.f,  1.f, 0.f };
    m_CulVertices[2].vPosition = {  1.f, -1.f, 0.f };
    m_CulVertices[3].vPosition = { -1.f, -1.f, 0.f };

    // back - far ÀÇ Åõ¿µ ÁÂÇ¥
    m_CulVertices[4].vPosition = { -1.f,  1.f, 1.f };
    m_CulVertices[5].vPosition = {  1.f,  1.f, 1.f };
    m_CulVertices[6].vPosition = {  1.f, -1.f, 1.f };
    m_CulVertices[7].vPosition = { -1.f, -1.f, 1.f };

    return S_OK;
}

void CCulling::Culling_Update(_float fDeltaTime)
{
    ::ZeroMemory(m_WorldVertices, sizeof(VTXTEX) * 8);
    ::ZeroMemory(m_Plane, sizeof(D3DXPLANE) * 6);

    _float4x4 ViewMatrixInv{}, ProjMatrixInv{};
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);

    D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
    D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);

    for (_int i = 0; i < 8; i++)
    {
        D3DXVec3TransformCoord(&m_WorldVertices[i].vPosition, &m_CulVertices[i].vPosition, &ProjMatrixInv);
        D3DXVec3TransformCoord(&m_WorldVertices[i].vPosition, &m_WorldVertices[i].vPosition, &ViewMatrixInv);
    }

    // +x
    D3DXPlaneFromPoints(&m_Plane[0], &m_WorldVertices[1].vPosition, &m_WorldVertices[5].vPosition, &m_WorldVertices[6].vPosition);
    // -x
    D3DXPlaneFromPoints(&m_Plane[1], &m_WorldVertices[4].vPosition, &m_WorldVertices[0].vPosition, &m_WorldVertices[3].vPosition);

    // +y
    D3DXPlaneFromPoints(&m_Plane[2], &m_WorldVertices[4].vPosition, &m_WorldVertices[5].vPosition, &m_WorldVertices[1].vPosition);
    // -y
    D3DXPlaneFromPoints(&m_Plane[3], &m_WorldVertices[3].vPosition, &m_WorldVertices[2].vPosition, &m_WorldVertices[6].vPosition);

    // +z
    D3DXPlaneFromPoints(&m_Plane[4], &m_WorldVertices[5].vPosition, &m_WorldVertices[4].vPosition, &m_WorldVertices[7].vPosition);
    // -z
    D3DXPlaneFromPoints(&m_Plane[5], &m_WorldVertices[0].vPosition, &m_WorldVertices[1].vPosition, &m_WorldVertices[2].vPosition);
}

HRESULT CCulling::is_Culling(CTransform* pTransform, _float fOffset)
{
    _float3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);

    for (_int i = 0; i < 6; i++)
    {
        _float fResult = m_Plane[i].a * vPos.x + m_Plane[i].b * vPos.y + m_Plane[i].c * vPos.z + m_Plane[i].d;
        if (fResult >= fOffset)
            return S_OK;
    }

    return E_FAIL;
}

CCulling* CCulling::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCulling* pInstance = new CCulling(pGraphic_Device);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CCulling"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCulling::Free()
{
    __super::Free();

    Safe_Release(m_pGraphic_Device);
}
