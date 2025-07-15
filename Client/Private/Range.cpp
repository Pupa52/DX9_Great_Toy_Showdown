#include "stdafx.h"
#include "../Public/Range.h"

#include "GameInstance.h"

CRange::CRange(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject(pGraphic_Device)
{
}

CRange::CRange(const CRange& Prototype)
	: CBlendObject(Prototype)
{
}

HRESULT CRange::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRange::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	RANGE_DESC* pDesc = static_cast<RANGE_DESC*>(pArg);

	m_pPlayerTransform = pDesc->pTransform;
	Safe_AddRef(m_pPlayerTransform);
	m_ePlayerCharacter = pDesc->ePlayerCharacter;

	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	_float fSize = 0.f;
	switch (m_ePlayerCharacter)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		fSize = 14.8f;
		break;

	case PLAYER_CHARACTER::PLAYER_DOG:
		fSize = 11.1f;
		break;

	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		fSize = 7.7f;
		break;

	default:
		return E_FAIL;
	}
	m_pTransformCom->Scaling(fSize, fSize, fSize);

	return S_OK;
}

_uint CRange::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CRange::Update(_float fTimeDelta)
{
	
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CRange::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CRange::Render()
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

HRESULT CRange::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player_Bullet_Range"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRange::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CRange::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CRange* CRange::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRange* pInstance = new CRange(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRange::Clone(void* pArg)
{
	CRange* pInstance = new CRange(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRange::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
