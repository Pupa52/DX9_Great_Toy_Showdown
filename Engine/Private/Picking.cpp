#include "Picking.h"

CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphic_Device)
    : m_pGraphic_Device { pGraphic_Device }
{
    Safe_AddRef(m_pGraphic_Device);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;

    m_iWinSizeX = iWinSizeX;

    m_iWinSizeY = iWinSizeY;

    return S_OK;
}

void CPicking::Update()
{
    POINT       ptMouse{};

    GetCursorPos(&ptMouse);

    /* ����Ʈ == ���� * ������� * ����� * ������� /w -> ����Ʈ ��ȯ */
    ScreenToClient(m_hWnd, &ptMouse);

    /* ���������̽� == ���� * ������� * ����� * ������� /w */
    _float3     vMousePos{};
    vMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
    vMousePos.y = ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
    vMousePos.z = 0.f; /* Near����� Ŭ���Ѱž�!! */

    /* �佺���̽� == ���� * ������� * ����� */
    _float4x4   ProjMatrixInv{};
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
    D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

    /* �佺���̽� �󿡼��� ���콺 ���̿� ������ �������� ���س��´�. */
    /* �佺���̽� ���̱� ������ ī�޶� ��ǥ�� �����̴�. */
    m_vRayPos = _float3(0.f, 0.f, 0.f);
    m_vRayDir = vMousePos - m_vRayPos;

    /* ���彺���̽� == ���� * ������� */
    _float4x4   ViewMatrixInv{};
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
    D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

    D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
    D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

    D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{
    _float4x4  WorldMatrixInverse = *D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);

    D3DXVec3TransformCoord(&m_vRayPos_InLocalSpace, &m_vRayPos, &WorldMatrixInverse);
    D3DXVec3TransformNormal(&m_vRayDir_InLocalSpace, &m_vRayDir, &WorldMatrixInverse);

    D3DXVec3Normalize(&m_vRayDir_InLocalSpace, &m_vRayDir_InLocalSpace);
}

_bool CPicking::isPicked_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
    _float  fU{}, fV{}, fDist{};

    if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist))
    {
        *pOut = m_vRayPos + m_vRayDir * fDist;
        return true;
    }

    return false;
}

_bool CPicking::isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
    _float		fU{}, fV{}, fDist{};

    if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos_InLocalSpace, &m_vRayDir_InLocalSpace, &fU, &fV, &fDist))
    {
        *pOut = m_vRayPos_InLocalSpace + m_vRayDir_InLocalSpace * fDist;
        return true;
    }

    return false;
}

CPicking* CPicking::Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pGraphic_Device);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX(TEXT("Failed to Created : CPicking"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pGraphic_Device);
}
