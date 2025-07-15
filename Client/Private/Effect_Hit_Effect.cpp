#include "stdafx.h"
#include "..\Public\Effect_Hit_Effect.h"

#include "GameInstance.h"

CEffect_Hit_Effect::CEffect_Hit_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Hit_Effect::CEffect_Hit_Effect(const CEffect_Hit_Effect& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Hit_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Hit_Effect::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float Rand=0;
	m_iRand = rand() % 3;
	Rand += rand() % 10 * 0.01f;
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian((_float(rand() % 20))));
	if(rand()%2 == 0)
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x + Rand,
		m_Edesc->m_Pos.y + Rand, m_Edesc->m_Pos.z - 0.1f));
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x - Rand,
			m_Edesc->m_Pos.y - Rand, m_Edesc->m_Pos.z - 0.1f));

	fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	m_fScale = { .5f, .5f, .5f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Hit_Effect::Priority_Update(_float fTimeDelta)
{
	if (m_fScale.x >= 0.55f)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Hit_Effect::Update(_float fTimeDelta)
{
	m_fFrame += 90.0f * fTimeDelta;
	
	m_pTransformCom->Go_Up(fTimeDelta);

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

void CEffect_Hit_Effect::Late_Update(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Hit_Effect::Render()
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

HRESULT CEffect_Hit_Effect::Ready_Components()
{
	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_Hit_Effect"),
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

HRESULT CEffect_Hit_Effect::SetUp_RenderState()
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

HRESULT CEffect_Hit_Effect::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Hit_Effect* CEffect_Hit_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Hit_Effect* pInstance = new CEffect_Hit_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Hit_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Hit_Effect::Clone(void* pArg)
{
	CEffect_Hit_Effect* pInstance = new CEffect_Hit_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Hit_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Hit_Effect::Free()
{
	__super::Free();
}
