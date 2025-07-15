#include "stdafx.h"
#include "..\Public\CanMoveCube.h"

#include "GameInstance.h"

CCanMoveCube::CCanMoveCube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CCanMoveCube::CCanMoveCube(const CCanMoveCube& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCanMoveCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanMoveCube::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float3 vPos{1.f, 1.f, 1.f};
	if (nullptr != pArg)
		vPos = *static_cast<_float3*>(pArg);

	m_pTransformCom->Scaling(1.f, 2.5f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

_uint CCanMoveCube::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CCanMoveCube::Update(_float fTimeDelta)
{
}

void CCanMoveCube::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CCanMoveCube::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanMoveCube::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CanMoveCube"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CCanMoveCube* CCanMoveCube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCanMoveCube* pInstance = new CCanMoveCube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCanMoveCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CCanMoveCube::Clone(void* pArg)
{
	CCanMoveCube* pInstance = new CCanMoveCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCanMoveCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCanMoveCube::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
