#include "stdafx.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI { pGraphic_Device }
{
}

CBackGround::CBackGround(const CBackGround & Prototype)
	: CUI { Prototype }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	
	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

_uint CBackGround::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CBackGround::Update(_float fTimeDelta)
{
}

void CBackGround::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CBackGround::Render()
{	
	/*m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/*m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
*/	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Background"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{0.f, 0.f};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
			
	return S_OK;
}

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround*		pInstance = new CBackGround(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject * CBackGround::Clone(void* pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
