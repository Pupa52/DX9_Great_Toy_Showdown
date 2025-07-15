#include "stdafx.h"
#include "..\Public\Effect_Tile_Rect.h"

#include "GameInstance.h"

CEffect_Tile_Rect::CEffect_Tile_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Tile_Rect::CEffect_Tile_Rect(const CEffect_Tile_Rect& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Tile_Rect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Tile_Rect::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	_float3 vPos = *static_cast<_float3*>(pArg);
	vPos.y += 1.001f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bdead = false;
	m_FrameCount = 0.f;
	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));

	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

_uint CEffect_Tile_Rect::Priority_Update(_float fTimeDelta)
{
	if (true == m_bdead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Tile_Rect::Update(_float fTimeDelta)
{
	m_FrameCount += fTimeDelta;
	if (m_FrameCount >= 0.1f)
	{
		if (m_iSpriteCount >= 14)
		{
			m_bdead = true;
		}
		else
		{
			m_iSpriteCount++;
		}

		m_FrameCount = 0;
	}
}

void CEffect_Tile_Rect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_Tile_Rect::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_iSpriteCount)))
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

HRESULT CEffect_Tile_Rect::Ready_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Tile_Rect"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Tile_Rect::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 230);

	return S_OK;
}

HRESULT CEffect_Tile_Rect::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CEffect_Tile_Rect* CEffect_Tile_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Tile_Rect* pInstance = new CEffect_Tile_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Tile_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Tile_Rect::Clone(void* pArg)
{
	CEffect_Tile_Rect* pInstance = new CEffect_Tile_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Tile_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Tile_Rect::Free()
{
	__super::Free();
}
