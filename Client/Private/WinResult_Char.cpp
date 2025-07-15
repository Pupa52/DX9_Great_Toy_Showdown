#include "stdafx.h"
#include "..\Public\WinResult_Char.h"

CWinResult_Char::CWinResult_Char(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CWinResult_Char::CWinResult_Char(const CWinResult_Char& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CWinResult_Char::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWinResult_Char::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_ePlayerID = *static_cast<PLAYER_CHARACTER*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	m_fSizeX = 700;
	m_fSizeY = 700;

	m_fX = (g_iWinSizeX >> 1) + 280.f + g_iWinSizeX;
	m_fY = (g_iWinSizeY >> 1) - 100.f;

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_fSpeed = 20.f;

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

	return S_OK;
}

_uint CWinResult_Char::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CWinResult_Char::Update(_float fTimeDelta)
{
	if ((g_iWinSizeX * 0.5f) + 280.f >= m_fX)
	{
		m_fX = (g_iWinSizeX * 0.5f) + 280.f;
		return;
	}

	m_fX -= m_fSpeed;
}

void CWinResult_Char::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Move_Frame(fTimeDelta * 0.3f);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CWinResult_Char::Render()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pAnimatorCom->Bind_Texture()))
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

HRESULT CWinResult_Char::Ready_Components()
{
	/* For.Com_Animator */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		m_pAnimatorCom->Add_Texture(TEXT("Charactor"),
			static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Cat"))));
		break;
	case PLAYER_CHARACTER::PLAYER_DOG:
		m_pAnimatorCom->Add_Texture(TEXT("Charactor"),
			static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Dog"))));
		break;
	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		m_pAnimatorCom->Add_Texture(TEXT("Charactor"),
			static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Result_Penguin"))));
		break;
	}
	
	m_pAnimatorCom->Change_Texture(TEXT("Charactor"));

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

CWinResult_Char* CWinResult_Char::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWinResult_Char* pInstance = new CWinResult_Char(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWinResult_Char"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CWinResult_Char::Clone(void* pArg)
{
	CWinResult_Char* pInstance = new CWinResult_Char(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWinResult_Char"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWinResult_Char::Free()
{
	__super::Free();

	Safe_Release(m_pAnimatorCom);
}
