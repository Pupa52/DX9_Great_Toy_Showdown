#include "stdafx.h"
#include "..\Public\Effect_Police_Die.h"

#include "GameInstance.h"

CEffect_Police_Die::CEffect_Police_Die(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Police_Die::CEffect_Police_Die(const CEffect_Police_Die& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Police_Die::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Police_Die::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x,
		m_Edesc->m_Pos.y+1, m_Edesc->m_Pos.z));

	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	m_fScale = { 1.f , 1.f , 1.f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Police_Die::Priority_Update(_float fTimeDelta)
{
	if (m_EffectCount >= 15.f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Police_Die::Update(_float fTimeDelta)
{
	m_FrameCount += fTimeDelta;
	m_fAirTime += fTimeDelta;
	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float result = 2.f * m_fAirTime - 0.5f * (m_fGravity * (m_fAirTime * m_fAirTime));
	m_vPos.y += result;
	if (!m_bGround)
	{
		m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), fTimeDelta);
	/*	if (m_fAirTime >= 3)
		{
			m_fAirTime = 3;
		}*/
	}

	if (m_vPos.y <= 1.3f)
	{
		m_bGround = true;
		m_vPos.y = 1.4f;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

	//m_pTransformCom->Go_Up(fTimeDelta);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Police_Die::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Police_Die::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture()))
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

HRESULT CEffect_Police_Die::Ready_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Police_Die"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
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

HRESULT CEffect_Police_Die::SetUp_RenderState()
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

HRESULT CEffect_Police_Die::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Police_Die* CEffect_Police_Die::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Police_Die* pInstance = new CEffect_Police_Die(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Police_Die"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Police_Die::Clone(void* pArg)
{
	CEffect_Police_Die* pInstance = new CEffect_Police_Die(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Police_Die"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Police_Die::Free()
{
	__super::Free();
}
