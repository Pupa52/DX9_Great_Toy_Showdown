#include "stdafx.h"
#include "..\Public\Effect_Tile_Smoke.h"

#include "GameInstance.h"

CEffect_Tile_Smoke::CEffect_Tile_Smoke(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Tile_Smoke::CEffect_Tile_Smoke(const CEffect_Tile_Smoke& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Tile_Smoke::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Tile_Smoke::Initialize(void* pArg)
{
	m_vPos = static_cast<_float3*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(64.f,
		0.6f , 64.f));

	///m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 90.f);
	m_fScale = { 128.f , 128.f, 128.f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Tile_Smoke::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CEffect_Tile_Smoke::Update(_float fTimeDelta)
{
	m_FrameCount += fTimeDelta;
	m_fAirTime += fTimeDelta;
	//m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//m_pTransformCom->Go_Up(fTimeDelta);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Tile_Smoke::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Tile_Smoke::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

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

HRESULT CEffect_Tile_Smoke::Ready_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Tile_Smoke"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(1.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Tile_Smoke::SetUp_RenderState()
{
	//_float3		vSourColor, vDestColor;
	/*_float3		vResultColor =
		vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a)
	*/

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CEffect_Tile_Smoke::Reset_RenderState()
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Tile_Smoke* CEffect_Tile_Smoke::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Tile_Smoke* pInstance = new CEffect_Tile_Smoke(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Tile_Smoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Tile_Smoke::Clone(void* pArg)
{
	CEffect_Tile_Smoke* pInstance = new CEffect_Tile_Smoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Tile_Smoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Tile_Smoke::Free()
{
	__super::Free();
}
