#include "stdafx.h"
#include "..\Public\Effect_MiniGame_Hit.h"

#include "GameInstance.h"

CEffect_MiniGame_Hit::CEffect_MiniGame_Hit(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_MiniGame_Hit::CEffect_MiniGame_Hit(const CEffect_MiniGame_Hit& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_MiniGame_Hit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_MiniGame_Hit::Initialize(void* pArg)
{
	m_Edesc = static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Edesc->m_Pos);

	fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_fScale = { .5f, .5f, .5f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_MiniGame_Hit::Priority_Update(_float fTimeDelta)
{
	if (true == m_bdead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_MiniGame_Hit::Update(_float fTimeDelta)
{
	m_fFrame += fTimeDelta * 16.f * 2.f;

	if (m_fFrame >= 16)
	{
		m_bdead = true;
	}
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_MiniGame_Hit::Late_Update(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_MiniGame_Hit::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture((_int)m_fFrame)))
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

HRESULT CEffect_MiniGame_Hit::Ready_Components()
{
	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, TEXT("Prototype_Component_Texture_Effect_Blast"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_MiniGame_Hit::SetUp_RenderState()
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

HRESULT CEffect_MiniGame_Hit::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_MiniGame_Hit* CEffect_MiniGame_Hit::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_MiniGame_Hit* pInstance = new CEffect_MiniGame_Hit(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_MiniGame_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_MiniGame_Hit::Clone(void* pArg)
{
	CEffect_MiniGame_Hit* pInstance = new CEffect_MiniGame_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_MiniGame_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_MiniGame_Hit::Free()
{
	__super::Free();
}
