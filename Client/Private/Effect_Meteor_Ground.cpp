#include "stdafx.h"
#include "Effect_Meteor_Ground.h"

CEffect_Meteor_Ground::CEffect_Meteor_Ground(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Meteor_Ground::CEffect_Meteor_Ground(const CEffect_Meteor_Ground& Prototype)
	: CEffect(Prototype)
{
}

HRESULT CEffect_Meteor_Ground::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Meteor_Ground::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimatorCom->Change_Texture(TEXT("Effect_Meteor_Ground"));

	METEORGROUND_DESC* pDesc = static_cast<METEORGROUND_DESC*>(pArg);

	pDesc->m_Pos.y = 1.01f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->m_Pos);
	m_pTransformCom->Scaling(3.f, 3.f, 3.f);
	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));

	cout << m_pTransformCom->Get_State(CTransform::STATE_POSITION).z << endl;

	m_pDead = pDesc->pDead;
	return S_OK;
}

_uint CEffect_Meteor_Ground::Priority_Update(_float fTimeDelta)
{
	if (true == *m_pDead)
		m_bdead = true;

	if (true == m_bdead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Meteor_Ground::Update(_float fTimeDelta)
{
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Meteor_Ground::Late_Update(_float fTimeDelta)
{
	if (SUCCEEDED(m_pAnimatorCom->Move_Frame(fTimeDelta * 2.f)))
		++m_FrameCount;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Meteor_Ground::Render()
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

HRESULT CEffect_Meteor_Ground::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_Meteor_Ground"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Meteor_Ground"))));

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

HRESULT CEffect_Meteor_Ground::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CEffect_Meteor_Ground::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Meteor_Ground* CEffect_Meteor_Ground::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Meteor_Ground* pInstance = new CEffect_Meteor_Ground(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create_Failed : CEffect_Meteor_Ground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Meteor_Ground::Clone(void* pArg)
{
	CEffect_Meteor_Ground* pInstance = new CEffect_Meteor_Ground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone_Failed : CEffect_Meteor_Ground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Meteor_Ground::Free()
{
	__super::Free();
}
