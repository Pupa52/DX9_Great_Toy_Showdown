#include "stdafx.h"
#include "../Public/Storm.h"

#include "GameInstance.h"

CStorm::CStorm(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CStorm::CStorm(const CStorm& Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CStorm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStorm::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_fRadius = 85.f;
	m_fHeight = 5.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(59.5f, m_fHeight * 0.5f + 1.f, 59.5f));

	return S_OK;
}

_uint CStorm::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CStorm::Update(_float fTimeDelta)
{
	m_pTransformCom->Scaling(m_fRadius * 2.f, m_fHeight, m_fRadius * 2.f);

	m_fViewZ = 0.01f;
}

void CStorm::Late_Update(_float fTimeDelta)
{
	if (m_fRadius >= 12.f)
		m_fRadius -= .2f * fTimeDelta;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CStorm::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
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

HRESULT CStorm::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Storm"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cylinder"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStorm::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CStorm::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	return S_OK;
}

CStorm* CStorm::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStorm* pInstance = new CStorm(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CStorm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStorm* CStorm::Clone(void* pArg)
{
	CStorm* pInstance = new CStorm(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStorm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStorm::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
