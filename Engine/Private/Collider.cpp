#include "Collider.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_pVB{ Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_dwFVF{ Prototype.m_dwFVF }
	, m_iNumFaces{ Prototype.m_iNumFaces }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_eIndexFormat{ Prototype.m_eIndexFormat }
	, m_WorldMatrix{ Prototype.m_WorldMatrix }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCollider::Render()
{
	if (FAILED(Bind_Transform()))
		return E_FAIL;

	if (FAILED(Bind_Buffers()))
		return E_FAIL;

#ifdef _DEBUG
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#else
	//m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumFaces);
#endif

	return S_OK;
}

void CCollider::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	_float3		vRight = Get_State(STATE_RIGHT);
	Set_State(STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * fScaleX);

	_float3		vUp = Get_State(STATE_UP);
	Set_State(STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * fScaleY);

	_float3		vLook = Get_State(STATE_LOOK);
	Set_State(STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * fScaleZ);
}

void CCollider::Update_Collider(_float3 vPos)
{
	Set_State(CCollider::STATE_POSITION, vPos);
}

HRESULT CCollider::Bind_Buffers()
{
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	return S_OK;
}

HRESULT CCollider::Bind_Transform()
{
	return m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
}

HRESULT CCollider::Ready_VertexBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices,
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCollider::Ready_IndexBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;
	return S_OK;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
