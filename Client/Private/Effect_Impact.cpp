#include "stdafx.h"
#include "..\Public\Effect_Impact.h"

#include "GameInstance.h"

CEffect_Impact::CEffect_Impact(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Impact::CEffect_Impact(const CEffect_Impact& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Impact::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Impact::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimatorCom->Change_Texture(TEXT("Effect_impact"));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x,
		m_Edesc->m_Pos.y, m_Edesc->m_Pos.z - 0.01f));

	fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	m_fScale = { 1.f , 1.f , 1.f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Impact::Priority_Update(_float fTimeDelta)
{
	if (m_EffectCount >= 1.f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Impact::Update(_float fTimeDelta)
{
	m_FrameCount += fTimeDelta;
	m_fFrame += 90.0f * fTimeDelta;
	
	//m_pTransformCom->Go_Up(fTimeDelta);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Impact::Late_Update(_float fTimeDelta)
{
	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_EffectCount++;

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Impact::Render()
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

HRESULT CEffect_Impact::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_impact"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Impact"))));

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Impact::SetUp_RenderState()
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

HRESULT CEffect_Impact::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Impact* CEffect_Impact::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Impact* pInstance = new CEffect_Impact(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Impact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Impact::Clone(void* pArg)
{
	CEffect_Impact* pInstance = new CEffect_Impact(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Impact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Impact::Free()
{
	__super::Free();
}
