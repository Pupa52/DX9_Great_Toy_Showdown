#include "..\Public\VIBuffer_Cylinder.h"

CVIBuffer_Cylinder::CVIBuffer_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Cylinder::CVIBuffer_Cylinder(const CVIBuffer_Cylinder & Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Cylinder::Initialize_Prototype(_float fBottomRadius, _float fTopRadius, _float fHeight, int iSliceCount)
{
	_float dTheta = -(D3DX_PI * 2) / _float(iSliceCount);

	m_iVertexStride = sizeof(VTXTEX);
	m_iNumVertices = (iSliceCount + 1) * 2;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
	m_iNumFaces = (iSliceCount * 2);
	m_iIndexStride = 2;
	m_iNumIndices = m_iNumFaces * 3;
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(__super::Ready_VertexBuffer()))
		return E_FAIL;

	VTXTEX* pVertices = { nullptr };
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	// bottom 정점 찍기
	for (_int i = 0; i <= iSliceCount; i++)
	{
		_float fTheta = dTheta * _float(i);
		_float fCosTheta = cos(fTheta);
		_float fSinTheta = sin(fTheta);

		// Position
		pVertices[i].vPosition.x = fBottomRadius * fCosTheta;
		pVertices[i].vPosition.y = -0.5f * fHeight;
		pVertices[i].vPosition.z = -fBottomRadius * fSinTheta;

		pVertices[i].vTexcoord.x = _float(i) / iSliceCount;
		pVertices[i].vTexcoord.y = 1.f;
	}

	// top 정점 찍기
	for (_int i = 0; i <= iSliceCount; i++)
	{
		_float fTheta = dTheta * _float(i);
		_float fCosTheta = cos(fTheta);
		_float fSinTheta = sin(fTheta);

		// Position
		pVertices[iSliceCount + 1 + i].vPosition.x = fTopRadius * fCosTheta;
		pVertices[iSliceCount + 1 + i].vPosition.y = 0.5f * fHeight;
		pVertices[iSliceCount + 1 + i].vPosition.z = -fTopRadius * fSinTheta;

		pVertices[iSliceCount + 1 + i].vTexcoord.x = _float(i) / iSliceCount;
		pVertices[iSliceCount + 1 + i].vTexcoord.y = 0.f;
	}

	m_pVB->Unlock();

	if (FAILED(Ready_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_ushort	iNumIndices = { 0 };
	for (_int i = 0; i < iSliceCount; i++)
	{
		_int iIndices[6] = {
			i,
			i + iSliceCount + 1,
			i + 1 + iSliceCount + 1,
			i,
			i + 1 + iSliceCount + 1,
			i + 1
		};

		pIndices[iNumIndices++] = iIndices[0];
		pIndices[iNumIndices++] = iIndices[1];
		pIndices[iNumIndices++] = iIndices[2];

		pIndices[iNumIndices++] = iIndices[3];
		pIndices[iNumIndices++] = iIndices[4];
		pIndices[iNumIndices++] = iIndices[5];
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Cylinder::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Cylinder* CVIBuffer_Cylinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _float fBottomRadius, _float fTopRadius, _float fHeight, int iSliceCount)
{
	CVIBuffer_Cylinder* pInstance = new CVIBuffer_Cylinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(fBottomRadius, fTopRadius, fHeight, iSliceCount)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Cylinder::Clone(void* pArg)
{
	CVIBuffer_Cylinder*		pInstance = new CVIBuffer_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Cylinder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cylinder::Free()
{
	__super::Free();
}

