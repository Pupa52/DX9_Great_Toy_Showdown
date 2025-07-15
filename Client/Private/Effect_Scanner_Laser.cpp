#include "stdafx.h"
#include "..\Public\Effect_Scanner_Laser.h"

#include "GameInstance.h"

CEffect_Scanner_Laser::CEffect_Scanner_Laser(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect{ pGraphic_Device }
{
}

CEffect_Scanner_Laser::CEffect_Scanner_Laser(const CEffect_Scanner_Laser& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CEffect_Scanner_Laser::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Scanner_Laser::Initialize(void* pArg)
{
	m_Edesc = static_cast<TAG_Effect_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float Rand=0;
	m_iRand = rand() % 3;
	if (m_iRand == 0)
	{
		m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian((_float(rand() % 30))));
		m_iRotation = 0;
	}
	else if (m_iRand == 1)
	{
		m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian((_float(rand() % -30))));
		m_iRotation = 1;
	}
	else if (m_iRand == 2)
	{
		m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian((_float(rand() % 10))));
		m_iRotation = 2;
	}

	if(rand()%2 == 0)
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x + Rand,
		m_Edesc->m_Pos.y + Rand, m_Edesc->m_Pos.z - 0.1f));
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Edesc->m_Pos.x - Rand,
			m_Edesc->m_Pos.y - Rand, m_Edesc->m_Pos.z - 0.1f));

	fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	m_fScale = { .1f, 12.0f, .1f };

	m_pTransformCom->Scaling(m_fScale.x, m_fScale.y, m_fScale.z);

	return S_OK;
}

_uint CEffect_Scanner_Laser::Priority_Update(_float fTimeDelta)
{
	if (m_fTimer >= 1.5f)
	{
	
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CEffect_Scanner_Laser::Update(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;
	m_fAngle += 0.5f * fTimeDelta;

	if(m_iRotation == 1)
		m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), m_fAngle);
	else if (m_iRotation == 0)
		m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), -m_fAngle);
	else if (m_iRotation == 2)
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), m_fAngle);
	
	m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), m_fAngle);

	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CEffect_Scanner_Laser::Late_Update(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Scanner_Laser::Render()
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

HRESULT CEffect_Scanner_Laser::Ready_Components()
{
	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Scanner_Laser"),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(1.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Scanner_Laser::SetUp_RenderState()
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

HRESULT CEffect_Scanner_Laser::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CEffect_Scanner_Laser* CEffect_Scanner_Laser::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Scanner_Laser* pInstance = new CEffect_Scanner_Laser(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Scanner_Laser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CEffect_Scanner_Laser::Clone(void* pArg)
{
	CEffect_Scanner_Laser* pInstance = new CEffect_Scanner_Laser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Scanner_Laser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Scanner_Laser::Free()
{
	__super::Free();
}
