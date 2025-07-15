#include "stdafx.h"
#include "Item.h"
#include "Transform.h"
#include "Collider_Cube.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject(pGraphic_Device)
{
}

CItem::CItem(const CItem& Prototype)
	: CLandObject(Prototype)
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fOffset = 1.f;

	return S_OK;
}

_uint CItem::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CItem::Update(_float fTimeDelta)
{
}

void CItem::Late_Update(_float fTimeDelta)
{
	Move_OffSet_Updown(fTimeDelta);
	__super::SetUp_OnTerrain(m_pTransformCom, m_fOffset);
}

HRESULT CItem::Render()
{
	return S_OK;
}


HRESULT CItem::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* �ȼ��� ���� �׽�Ʈ�� ����Ͽ� �׷��� ������ �����Ѵ�. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	/* ���İ��� 0���� ū �͵鸸 �׸��ڴ�. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}

HRESULT CItem::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CItem::Move_OffSet_Updown(_float fTimeDelta)
{
	if (m_fMoveSpeed < 0.01f)
	{
		m_bUpOrDown = !m_bUpOrDown;
		m_fMoveSpeed = 0.2f;
	}
	m_fMoveSpeed -= 0.005f;

	if (true == m_bUpOrDown)
		m_fOffset += m_fMoveSpeed * fTimeDelta;
	else
		m_fOffset -= m_fMoveSpeed * fTimeDelta;


	m_fMoveTime += fTimeDelta;
}

void CItem::Free()
{
	__super::Free();
}
