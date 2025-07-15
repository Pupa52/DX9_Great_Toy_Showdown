#include "stdafx.h"
#include "..\Public\Effect_Boss_Hit.h"

#include "GameInstance.h"

CEffect_Boss_Hit::CEffect_Boss_Hit(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Boss_Hit::CEffect_Boss_Hit(const CEffect_Boss_Hit& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Boss_Hit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Boss_Hit::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimatorCom->Change_Texture(TEXT("Boss_Hit_Effect"));

	_float Rand = 0;
	_float m_iRand = _float(rand() % 3);
	Rand += rand() % 10 * 0.01f;
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian((_float(rand() % 20))));
	if (rand() % 2 == 0)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x + Rand,
			m_Edesc->m_Pos.y + Rand, m_Edesc->m_Pos.z - 0.1f));
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x - Rand,
			m_Edesc->m_Pos.y - Rand, m_Edesc->m_Pos.z - 0.1f));



	m_fScale = { 0.3f, 0.3f, 0.3f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Boss_Hit::Priority_Update(_float fTimeDelta)
{
	if (m_EffectCount >= 0.5f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Boss_Hit::Update(_float fTimeDelta)
{
	m_fFrame += 90.0f * fTimeDelta;
	
	//m_pTransformCom->Go_Up(fTimeDelta);

	if (fTimeDelta * 60 > 1.f)
	{
			m_fScale.x += 0.005f;
			m_fScale.y += 0.005f;
			m_fScale.z += 0.005f;
		m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	}
	if (m_fFrame >= 90.0f)
	{
		m_FrameCount++;
		m_fFrame = 0.f;
	}
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Boss_Hit::Late_Update(_float fTimeDelta)
{

	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_EffectCount++;
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Boss_Hit::Render()
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

HRESULT CEffect_Boss_Hit::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Boss_Hit_Effect"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet_Boss_Hit_Effect"))));


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 1.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Boss_Hit::SetUp_RenderState()
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

HRESULT CEffect_Boss_Hit::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Boss_Hit* CEffect_Boss_Hit::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Boss_Hit* pInstance = new CEffect_Boss_Hit(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Boss_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Boss_Hit::Clone(void* pArg)
{
	CEffect_Boss_Hit* pInstance = new CEffect_Boss_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Boss_Hit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Boss_Hit::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
