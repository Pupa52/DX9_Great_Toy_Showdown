#include "stdafx.h"
#include "..\Public\Effect_Big_Shadow.h"

#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"

#include "LandObject.h"
#include "Layer.h"

CEffect_Big_Shadow::CEffect_Big_Shadow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Big_Shadow::CEffect_Big_Shadow(const CEffect_Big_Shadow& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Big_Shadow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Big_Shadow::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x, 1.5f, m_Edesc->m_Pos.z));

	m_fScale = { 4.f , 4.f , 4.f };
	
	m_pAnimatorCom->Change_Texture(TEXT("Effect_Big_Shadow"));

	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	
	return S_OK;
}

_uint CEffect_Big_Shadow::Priority_Update(_float fTimeDelta)
{
	if (m_fTimer > 4)
	{
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CEffect_Big_Shadow::Update(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Big_Shadow::Late_Update(_float fTimeDelta)
{
//	m_bdead = true;

	m_pAnimatorCom->Move_Frame(fTimeDelta);

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);


}

HRESULT CEffect_Big_Shadow::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
		return E_FAIL;
	
	//if (FAILED(m_pTextureCom->Bind_Texture(0)))
	//	return E_FAIL;

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

HRESULT CEffect_Big_Shadow::Find_Obj()
{


	return S_OK;
}

HRESULT CEffect_Big_Shadow::Ready_Components()
{

	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_Big_Shadow"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Big_Shadow"))));

	///* For.Com_Textrue */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shadow"),
	//	TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	//	return E_FAIL;

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

HRESULT CEffect_Big_Shadow::SetUp_RenderState()
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

HRESULT CEffect_Big_Shadow::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Big_Shadow* CEffect_Big_Shadow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Big_Shadow* pInstance = new CEffect_Big_Shadow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Big_Shadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Big_Shadow::Clone(void* pArg)
{
	CEffect_Big_Shadow* pInstance = new CEffect_Big_Shadow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Big_Shadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Big_Shadow::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
