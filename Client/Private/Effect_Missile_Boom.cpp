#include "stdafx.h"
#include "Effect_Missile_Boom.h"

CEffect_Missile_Boom::CEffect_Missile_Boom(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Missile_Boom::CEffect_Missile_Boom(const CEffect_Missile_Boom& Prototype)
	: CEffect(Prototype)
{
}

HRESULT CEffect_Missile_Boom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Missile_Boom::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimatorCom->Change_Texture(TEXT("Effect_Missile_Boom"));

	m_Edesc = static_cast<EFFECT_DESC*>(pArg);
	m_Edesc->m_Pos.y += .25f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Edesc->m_Pos);
	m_pTransformCom->Scaling(2.f, 2.f, 2.f);

	return S_OK;
}

_uint CEffect_Missile_Boom::Priority_Update(_float fTimeDelta)
{
	if (true == m_bdead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Missile_Boom::Update(_float fTimeDelta)
{

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Missile_Boom::Late_Update(_float fTimeDelta)
{
	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		++m_FrameCount;

	if (0 < m_FrameCount)
		m_bdead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Missile_Boom::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
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

HRESULT CEffect_Missile_Boom::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_Missile_Boom"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Missile_Boom"))));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Missile_Boom::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CEffect_Missile_Boom::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Missile_Boom* CEffect_Missile_Boom::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Missile_Boom* pInstance = new CEffect_Missile_Boom(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create_Failed : CEffect_Missile_Boom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Missile_Boom::Clone(void* pArg)
{
	CEffect_Missile_Boom* pInstance = new CEffect_Missile_Boom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone_Failed : CEffect_Missile_Boom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Missile_Boom::Free()
{
	__super::Free();
}
