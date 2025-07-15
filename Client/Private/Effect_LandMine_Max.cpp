#include "stdafx.h"
#include "..\Public\Effect_LandMine_max.h"

#include "GameInstance.h"

CEFFECT_LandMine_Max::CEFFECT_LandMine_Max(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEFFECT_LandMine_Max::CEFFECT_LandMine_Max(const CEFFECT_LandMine_Max& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEFFECT_LandMine_Max::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEFFECT_LandMine_Max::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimatorCom->Change_Texture(TEXT("Effect_MineTiemr_Max"));

	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x, m_Edesc->m_Pos.y-.4f, m_Edesc->m_Pos.z));

	m_pTransformCom->Scaling(1.f, 1.f, 1.f);

	return S_OK;
}

_uint CEFFECT_LandMine_Max::Priority_Update(_float fTimeDelta)
{
	if (m_FrameCount >= 1.8f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEFFECT_LandMine_Max::Update(_float fTimeDelta)
{
	m_FrameCount += fTimeDelta;


	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEFFECT_LandMine_Max::Late_Update(_float fTimeDelta)
{

	m_pAnimatorCom->Move_Frame(fTimeDelta);
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEFFECT_LandMine_Max::Render()
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

HRESULT CEFFECT_LandMine_Max::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pAnimatorCom->Add_Texture(TEXT("Effect_MineTiemr_Max"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MineTimer_Max"))));


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

HRESULT CEFFECT_LandMine_Max::SetUp_RenderState()
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

HRESULT CEFFECT_LandMine_Max::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEFFECT_LandMine_Max* CEFFECT_LandMine_Max::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEFFECT_LandMine_Max* pInstance = new CEFFECT_LandMine_Max(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEFFECT_LandMine_Max"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEFFECT_LandMine_Max::Clone(void* pArg)
{
	CEFFECT_LandMine_Max* pInstance = new CEFFECT_LandMine_Max(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEFFECT_LandMine_Max"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEFFECT_LandMine_Max::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
