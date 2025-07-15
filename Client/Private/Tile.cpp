#include "stdafx.h"
#include "../Public/Tile.h"

#include "GameInstance.h"

CTile::CTile(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CTile::CTile(const CTile& Prototype)
	: CGameObject{ Prototype }
	, m_eCurTile { Prototype.m_eCurTile }
	, m_iTileNum { Prototype.m_iTileNum }
{
}

HRESULT CTile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTile::Initialize(void* pArg)
{
	TILE_DESC* pTileDesc = static_cast<TILE_DESC*>(pArg);
	m_eLevelID = pTileDesc->eLevelID;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTileDesc->vPos);
	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	m_eCurTile = (CTile::TILE_TYPE)pTileDesc->iTileNum;
	m_iTileNum = pTileDesc->iTileIndex;

	return S_OK;
}

_uint CTile::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CTile::Update(_float fTimeDelta)
{

}

void CTile::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTile::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture((_uint)m_eCurTile)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (m_eCurTile != CTile::TILE_END &&
		FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CTile::Ready_Components()
{
	/* For.Com_Texture_Tile */
	if (FAILED(__super::Add_Component(m_eLevelID, TEXT("Prototype_Component_Texture_Tile"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CTile* CTile::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTile* pInstance = new CTile(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTile::Clone(void* pArg)
{
	CTile* pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTile::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
