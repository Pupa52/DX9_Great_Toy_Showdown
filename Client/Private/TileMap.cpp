#include "stdafx.h"
#include "..\Public\TileMap.h"

#include "GameInstance.h"

CTileMap::CTileMap(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CTileMap::CTileMap(const CTileMap& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTileMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTileMap::Initialize(void* pArg)
{
	TILEMAP_DESC* pDesc = static_cast<TILEMAP_DESC*>(pArg);
	m_eLevelID = pDesc->eLevelID;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 1.f, 0.f));

	return S_OK;
}

_uint CTileMap::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CTileMap::Update(_float fTimeDelta)
{
}

void CTileMap::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTileMap::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	//if (FAILED(SetUp_RenderState()))
	//	return E_FAIL;

	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;

	//if (FAILED(Reset_RenderState()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CTileMap::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CTileMap* CTileMap::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTileMap* pInstance = new CTileMap(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTileMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTileMap::Clone(void* pArg)
{
	CTileMap* pInstance = new CTileMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTileMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTileMap::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
