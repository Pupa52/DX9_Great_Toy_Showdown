#include "stdafx.h"
#include "..\Public\Effect_Boss_Arrow.h"

#include "GameInstance.h"

CEffect_Boss_Arrow::CEffect_Boss_Arrow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Boss_Arrow::CEffect_Boss_Arrow(const CEffect_Boss_Arrow& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Boss_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Boss_Arrow::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x,
		1.5f, m_Edesc->m_Pos.z));
	
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	fPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	m_bTurn = true;

	m_fScale = { 0.5f, 5.f, .5f };


	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Boss_Arrow::Priority_Update(_float fTimeDelta)
{
	if (m_fDelay >= 1.f)
	{
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CEffect_Boss_Arrow::Update(_float fTimeDelta)
{
	m_fDelay += fTimeDelta;

	m_fAngle = Calculator_Arrow();
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), m_fAngle);
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 90.f);
	if (m_bTurn)
	{
		m_pTransformCom->Go_Up(2);
		m_bTurn = false;
	}

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Boss_Arrow::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Boss_Arrow::Render()
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

HRESULT CEffect_Boss_Arrow::Ready_Components()
{
	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Boss_Arrow"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(1.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Boss_Arrow::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	return S_OK;
}

HRESULT CEffect_Boss_Arrow::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


	return S_OK;
}

_float CEffect_Boss_Arrow::Calculator_Arrow()
{
	m_Edesc->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vPlayer;
	_float3 vEffect;
	vPlayer = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vEffect = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMonster_Pos, vPlayer_Pos;
	vPlayer_Pos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vMonster_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vEffect = fPos - vEffect;
	m_vLastPos = vEffect;
	D3DXVec3Normalize(&vEffect, &vEffect);

	_float fDot = D3DXVec3Dot(&vEffect, &_float3(0.f, 0.f, 1.f));
	_float fAngle = acosf(fDot);

	if (vMonster_Pos.x < fPos.x)
		fAngle *= -1.f;

	return fAngle;

}

CEffect_Boss_Arrow* CEffect_Boss_Arrow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Boss_Arrow* pInstance = new CEffect_Boss_Arrow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Boss_Arrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Boss_Arrow::Clone(void* pArg)
{
	CEffect_Boss_Arrow* pInstance = new CEffect_Boss_Arrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Boss_Arrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Boss_Arrow::Free()
{
	__super::Free();
}
