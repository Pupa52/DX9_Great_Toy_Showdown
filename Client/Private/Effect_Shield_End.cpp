#include "stdafx.h"
#include "..\Public\Effect_Shield_End.h"

#include "GameInstance.h"

CEffect_Shield_End::CEffect_Shield_End(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Shield_End::CEffect_Shield_End(const CEffect_Shield_End& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Shield_End::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Shield_End::Initialize(void* pArg)
{
	RandPos = rand() % 8;
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian((_float(rand() % 180))));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x + (RandPos / 10),
		m_Edesc->m_Pos.y, m_Edesc->m_Pos.z - 0.3f - RandPos/10));

	m_fScale = { .8f, .8f, .8f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Shield_End::Priority_Update(_float fTimeDelta)
{
	if (m_EffectCount >= 0.5f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Shield_End::Update(_float fTimeDelta)
{
	m_EffectCount += fTimeDelta;
	m_fFrame += 90.0f * fTimeDelta;
	//if (RandPos == 0)
	//	m_pTransformCom->Go_Right(fTimeDelta);
	//if (RandPos == 1)
	//	m_pTransformCom->Go_Left(fTimeDelta);
	//if (RandPos == 2)
	//	m_pTransformCom->Go_Left(fTimeDelta);
	//if (RandPos == 3)
	//	m_pTransformCom->Go_Left(fTimeDelta);

	if (fTimeDelta * 60 > 1.f)
	{
			m_fScale.x -= 0.005f;
			m_fScale.y -= 0.005f;
			m_fScale.z -= 0.005f;
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

void CEffect_Shield_End::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Shield_End::Render()
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

HRESULT CEffect_Shield_End::Ready_Components()
{

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Shield_End"),
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

HRESULT CEffect_Shield_End::SetUp_RenderState()
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

HRESULT CEffect_Shield_End::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Shield_End* CEffect_Shield_End::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Shield_End* pInstance = new CEffect_Shield_End(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Shield_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Shield_End::Clone(void* pArg)
{
	CEffect_Shield_End* pInstance = new CEffect_Shield_End(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Shield_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Shield_End::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
}
