#include "stdafx.h"
#include "..\Public\Effect_Spark_Light.h"

#include "GameInstance.h"

CEffect_Spark_Light::CEffect_Spark_Light(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Spark_Light::CEffect_Spark_Light(const CEffect_Spark_Light& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Spark_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Spark_Light::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x, m_Edesc->m_Pos.y, m_Edesc->m_Pos.z));

	m_fScale = { 1.0f, 1.0f, 1.0f };


	return S_OK;
}

_uint CEffect_Spark_Light::Priority_Update(_float fTimeDelta)
{
	m_EffectCount += fTimeDelta;
	if (m_fScale.x >= 3.f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Spark_Light::Update(_float fTimeDelta)
{
	if (m_EffectCount >= 0.0001f)
	{
		m_fScale.x += 1.f;
		m_fScale.y += 1.f;
		m_fScale.z += 1.f;
		m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	}

	m_fFrame += 90.0f * fTimeDelta;

	if (m_fFrame >= 90.0f)
	{
		m_FrameCount++;
		m_fFrame = 0.f;
	}
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Spark_Light::Late_Update(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Spark_Light::Render()
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

HRESULT CEffect_Spark_Light::Ready_Components()
{
	/* For.Com_Animator*/
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
	//	TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
	//	return E_FAIL;

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Spark_Light"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cylinder"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
	//	return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Spark_Light::SetUp_RenderState()
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

HRESULT CEffect_Spark_Light::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Spark_Light* CEffect_Spark_Light::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Spark_Light* pInstance = new CEffect_Spark_Light(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Spark_Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Spark_Light::Clone(void* pArg)
{
	CEffect_Spark_Light* pInstance = new CEffect_Spark_Light(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Spark_Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Spark_Light::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
