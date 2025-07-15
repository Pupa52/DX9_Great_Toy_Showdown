#include "stdafx.h"
#include "..\Public\Effect_LandMine_Explosion.h"

#include "GameInstance.h"
#include "Actor.h"

CEffect_LandMine_Explosion::CEffect_LandMine_Explosion(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_LandMine_Explosion::CEffect_LandMine_Explosion(const CEffect_LandMine_Explosion& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_LandMine_Explosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_LandMine_Explosion::Initialize(void* pArg)
{
	m_Edesc = static_cast<EFFECT_DESC*>(pArg);
	
	EXPLOSION_DESC* pDesc = static_cast<EXPLOSION_DESC*>(m_Edesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimatorCom->Change_Texture(TEXT("Effect_Mine_Explosive"));

	m_Edesc->m_Pos.y += pDesc->fScale * .4f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x, m_Edesc->m_Pos.y, m_Edesc->m_Pos.z));

	m_fScale = { pDesc->fScale, pDesc->fScale, pDesc->fScale };

	m_EffectCount = 0;

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_LandMine_Explosion::Priority_Update(_float fTimeDelta)
{
	if (m_bdead)
	{
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CEffect_LandMine_Explosion::Update(_float fTimeDelta)
{
	if (m_EffectCount >= 1.f)
	{
		m_bdead = true;
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

void CEffect_LandMine_Explosion::Late_Update(_float fTimeDelta)
{

	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_EffectCount++;

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_LandMine_Explosion::Render()
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

HRESULT CEffect_LandMine_Explosion::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_Mine_Explosive"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Texture_Mine_Explosive"))));


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

HRESULT CEffect_LandMine_Explosion::SetUp_RenderState()
{
	//_float3		vSourColor, vDestColor;
	/*_float3		vResultColor =
		vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a)
	*/

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;
}

HRESULT CEffect_LandMine_Explosion::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

CEffect_LandMine_Explosion* CEffect_LandMine_Explosion::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_LandMine_Explosion* pInstance = new CEffect_LandMine_Explosion(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_LandMine_Explosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_LandMine_Explosion::Clone(void* pArg)
{
	CEffect_LandMine_Explosion* pInstance = new CEffect_LandMine_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_LandMine_Explosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_LandMine_Explosion::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
