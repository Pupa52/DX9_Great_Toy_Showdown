#include "stdafx.h"
#include "../Public/Wall.h"

#include "GameInstance.h"

CWall::CWall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CWall::CWall(const CWall& Prototype)
	: CGameObject{ Prototype }
	, m_eCurWall{ Prototype.m_eCurWall }
	, m_iWallNum{ Prototype.m_iWallNum }
{
}

HRESULT CWall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWall::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	WALL_DESC* pWallDesc = static_cast<WALL_DESC*>(pArg);

	m_eCurWall = (CWall::TILE_TYPE)pWallDesc->iWallIndex;
	m_iWallNum = pWallDesc->iWallNum;
	m_eWallType = pWallDesc->eWallType;

	_float fAngle = 0.f;
	if (m_eWallType == WALL_RIGHT)
	{
		fAngle = D3DXToRadian(90.f);
	}
	else if (m_eWallType == WALL_LEFT)
	{
		fAngle = D3DXToRadian(-90.f);
	}
	pWallDesc->vPos.y += 0.01f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pWallDesc->vPos);
	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), fAngle);

	return S_OK;
}

_uint CWall::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CWall::Update(_float fTimeDelta)
{
}

void CWall::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWall::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if ((m_eCurWall * 2 + m_iWallNum) != TILE_END &&
		FAILED(m_pTextureCom->Bind_Texture(m_eCurWall * 2 + m_iWallNum)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWall::Ready_Components()
{
	/* For.Com_Texture_Tile */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Tile"),
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

CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWall* pInstance = new CWall(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWall::Clone(void* pArg)
{
	CWall* pInstance = new CWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWall::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
