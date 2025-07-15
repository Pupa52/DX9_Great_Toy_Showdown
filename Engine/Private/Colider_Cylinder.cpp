#include "..\Public\Colider_Cylinder.h"

CColider_Cylinder::CColider_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollider{ pGraphic_Device }
{
}

CColider_Cylinder::CColider_Cylinder(const CColider_Cylinder& Prototype)
	: CCollider{ Prototype }
{
}

HRESULT CColider_Cylinder::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);

	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = 8;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_iNumFaces = 12;
	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = D3DFMT_INDEX16;

	/* 내가 의도한 사이즈만큼의 정점 배열공간을 할당했다. */
	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;

	VTXCUBE* pVertices = { nullptr };
	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, (void**)&pVertices, 0);

	for (DWORD i = 0; i < 50; i++)
	{
		FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
		pVertices[2 * i + 0].vPosition = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));	/// 실린더의 아래쪽 원통의 좌표
		pVertices[2 * i + 0].vTexcoord = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));	/// 실린더의 아래쪽 원통의 노멀
		pVertices[2 * i + 1].vPosition = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));	/// 실린더의 위쪽 원통의 좌표
		pVertices[2 * i + 1].vTexcoord = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));	/// 실린더의 위쪽 원통의 노멀
	}
	m_pVB->Unlock();

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_pIB->Unlock();



	return S_OK;
}

HRESULT CColider_Cylinder::Initialize(void* pArg)
{
	return S_OK;
}

CColider_Cylinder* CColider_Cylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CColider_Cylinder* pInstance = new CColider_Cylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CColider_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CColider_Cylinder::Clone(void* pArg)
{
	CColider_Cylinder* pInstance = new CColider_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CColider_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColider_Cylinder::Free()
{
	__super::Free();
}

