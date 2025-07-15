#include "stdafx.h"
#include "..\Public\Monster_Cuptrio.h"

#include "GameInstance.h"
#include "Damage.h"

CMonster_Cuptrio::CMonster_Cuptrio(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Cuptrio::CMonster_Cuptrio(const CMonster_Cuptrio& Prototype)
	:CMonster{ Prototype }
{
}


HRESULT CMonster_Cuptrio::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Cuptrio::Initialize(void* pArg)
{
	if (FAILED(CActor::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(_float(rand() % 100), 0.f, _float(rand() % 100)));
	m_pTransformCom->Scaling(5.f, 5.f, 1.f);
	return S_OK;
}

_uint CMonster_Cuptrio::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CMonster_Cuptrio::Update(_float fTimeDelta)
{
	
	if (GetAsyncKeyState('M'))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Cup_Move"));
	}
	if (GetAsyncKeyState('N'))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Cup_Die"));
	}

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CMonster_Cuptrio::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Move_Frame(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster_Cuptrio::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

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

HRESULT CMonster_Cuptrio::Ready_Components() 
{

	// 위험해
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

//Texture

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Cuptrio_Die"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Texture(TEXT("Cup_Die"), m_pTextureCom);
	m_pAnimatorCom->Change_Texture(TEXT("Cup_Die"));
	Safe_AddRef(m_pTextureCom);

	///* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Cuptrio_Move"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Texture(TEXT("Cup_Move"), m_pTextureCom);
	m_pAnimatorCom->Change_Texture(TEXT("Cup_Move"));
	Safe_AddRef(m_pTextureCom);
//  ENd


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Cuptrio::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;

}

HRESULT CMonster_Cuptrio::Reset_RenderState()
{
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	// arg2 는 상수컬러

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}


CMonster_Cuptrio* CMonster_Cuptrio::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CMonster_Cuptrio* pInstance = new CMonster_Cuptrio(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Cuptrio"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Cuptrio::Clone(void* pArg)
{

	CMonster_Cuptrio* pInstance = new CMonster_Cuptrio(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Cuptrio"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Cuptrio::Free()
{
	__super::Free();

}
