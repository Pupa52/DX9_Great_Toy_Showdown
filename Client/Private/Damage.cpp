#include "stdafx.h"
#include "Damage.h"
#include "GameInstance.h"

CDamage::CDamage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDamage::CDamage(const CDamage& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CDamage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDamage::Initialize(void* pArg)
{
	DAMAGE_DESC* pDesc = static_cast<DAMAGE_DESC*>(pArg);

	m_iDamage = pDesc->iDamage;
	m_vPos = pDesc->vPos;
	m_tColor = pDesc->tColor;

	return S_OK;
}

_uint CDamage::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CDamage::Update(_float fTimeDelta)
{
	m_vPos.y += 1.f * fTimeDelta;
	m_vPoint = Compute_WinPoint(m_vPos);
}

void CDamage::Late_Update(_float fTimeDelta)
{
	m_fCheckTime += fTimeDelta;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	if (1.f < m_fCheckTime)
		m_bDead = true;
}

HRESULT CDamage::Render()
{
	_wstring strDamage = to_wstring(m_iDamage);

	D3DCOLOR tColor = {};
	FONT_TYPE eFontType = { FT_END };

	if (m_iDamage < 50)
	{
		if (D3DCOLOR_ARGB(0, 0, 0, 0) == m_tColor)
			m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		eFontType = FT_LARGE;
	}
	else
	{

		if (D3DCOLOR_ARGB(0, 0, 0, 0) == m_tColor)
			m_tColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		eFontType = FT_L_LARGE;
	}

	m_pGameInstance->Draw_Text(eFontType, strDamage, (_uint)m_vPoint.x, (_uint)m_vPoint.y, m_tColor);
	return S_OK;
}

_float2 CDamage::Compute_WinPoint(_float3 vPos)
{
	_float4x4 ViewMatrix = {};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXVec3TransformCoord(&vPos, &vPos, &ViewMatrix);

	_float4x4 ProjMatrix = {};
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	D3DXVec3TransformCoord(&vPos, &vPos, &ProjMatrix);

	_float2 vPoint = {};

	vPoint.x = g_iWinSizeX * 0.5f * vPos.x + g_iWinSizeX * 0.5f;
	vPoint.y = -1 * (_int)g_iWinSizeY * 0.5f * vPos.y + g_iWinSizeY * 0.5f;

	return vPoint;
}

CDamage* CDamage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDamage* pInstance = new CDamage(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CDamage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDamage::Clone(void* pArg)
{
	CDamage* pInstance = new CDamage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CDamage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDamage::Free()
{
	__super::Free();
}
