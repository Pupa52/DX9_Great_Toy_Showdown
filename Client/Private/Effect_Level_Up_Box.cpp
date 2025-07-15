#include "stdafx.h"
#include "..\Public\Effect_Level_Up_Box.h"

#include "GameInstance.h"

CEffect_Level_Up_Box::CEffect_Level_Up_Box(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Level_Up_Box::CEffect_Level_Up_Box(const CEffect_Level_Up_Box& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Level_Up_Box::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Level_Up_Box::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x,
		1.5f, m_Edesc->m_Pos.z - 0.01f));

	fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_fScale = { 2.f , 2.f , 2.f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);
	m_iRand = rand() % 4;
	return S_OK;
}

_uint CEffect_Level_Up_Box::Priority_Update(_float fTimeDelta)
{
	if (m_EffectCount >= 10.f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Level_Up_Box::Update(_float fTimeDelta)
{
	m_FrameCount += fTimeDelta;
	m_fFrame += 90.0f * fTimeDelta;
	
	//m_pTransformCom->Go_Up(fTimeDelta);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Level_Up_Box::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Level_Up_Box::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_iRand)))
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

HRESULT CEffect_Level_Up_Box::Ready_Components()
{
	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Level_Up_Box"),
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

HRESULT CEffect_Level_Up_Box::SetUp_RenderState()
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

HRESULT CEffect_Level_Up_Box::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Level_Up_Box* CEffect_Level_Up_Box::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Level_Up_Box* pInstance = new CEffect_Level_Up_Box(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Level_Up_Box"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Level_Up_Box::Clone(void* pArg)
{
	CEffect_Level_Up_Box* pInstance = new CEffect_Level_Up_Box(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Level_Up_Box"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Level_Up_Box::Free()
{
	__super::Free();
}
