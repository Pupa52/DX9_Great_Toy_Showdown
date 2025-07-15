#include "stdafx.h"
#include "..\Public\Monster_Police_First_Spine.h"

#include "GameInstance.h"
#include "Monster_Police.h"
CMonster_Police_First_Spine::CMonster_Police_First_Spine(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Police_First_Spine::CMonster_Police_First_Spine(const CMonster_Police_First_Spine& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CMonster_Police_First_Spine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Police_First_Spine::Initialize(void* pArg)
{
	//if (FAILED(CActor::Initialize(pArg)))
	//	return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_tActorDesc.iAttack = 10;
	m_tActorDesc.iMaxHp = 100;
	m_tActorDesc.iHp = 100;
	m_pAnimatorCom->Change_Texture(TEXT("Monster_First_Spine_Shot"));
	
	
	m_pTransformCom->Scaling(3.f, 3.f, 3.f);

	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);
	return S_OK;
}

_uint CMonster_Police_First_Spine::Priority_Update(_float fTimeDelta)
{
	if (m_bDeadMotion && (m_iFrameCounter > 0))
		return OBJ_DEAD;

	if (m_bdead)
	{
		m_bdead = false;
	}


	return OBJ_NOEVENT;
}

void CMonster_Police_First_Spine::Update(_float fTimeDelta)
{
	
	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	int Rand = rand() % 2;

	//if (Get_Attack_State() == CMonster_Police::POLICE_IDLE)
	//{
	//	m_pAnimatorCom->Change_Texture(TEXT("Monster_First_Spine_Idle"));
	//}
	//else if (Get_Attack_State() == CMonster_Police::POLICE_DIE)
	//{
	//	m_pAnimatorCom->Change_Texture(TEXT("Monster_First_Spine_Die"));
	//}
	//else if (Get_Attack_State() == CMonster_Police::POLICE_MOVE)
	//{
	//	m_pAnimatorCom->Change_Texture(TEXT("Monster_First_Spine_Move"));
	//}
	//else if (Get_Attack_State() == CMonster_Police::POLICE_SHOT_RETURN)
	//{
	//	m_pAnimatorCom->Change_Texture(TEXT("Monster_First_Spine_Shot_Back"));
	//}
	//else if (Get_Attack_State() == CMonster_Police::POLICE_SHOT)
	//{
	//	m_pAnimatorCom->Change_Texture(TEXT("Monster_First_Spine_Shot"));
	//}




}

void CMonster_Police_First_Spine::Late_Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_iFrameCounter++;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster_Police_First_Spine::Render()
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

	if (FAILED(m_pColliderCubeCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Police_First_Spine::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CMonster_Police_First_Spine::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_First_Spine_Idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_First_Spine_Die"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Die"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_First_Spine_Move"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Move"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_First_Spine_Shot_Back"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Shot_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_First_Spine_Shot"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_First_Spine_Shot"))));

	//  ENd


		/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Police_First_Spine::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 0, 0));

	if ((m_fHittimer >= 0.9f) && (m_bdead || (m_Monster_Statue == MONSTER_STATE_SLOW)))
	{
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		// arg2 는 상수컬러
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	}
	else
	{
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 255, 255));

		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		// arg2 는 상수컬러
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	}

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;

}

HRESULT CMonster_Police_First_Spine::Reset_RenderState()
{
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	// arg2 는 상수컬러

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}


CMonster_Police_First_Spine* CMonster_Police_First_Spine::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CMonster_Police_First_Spine* pInstance = new CMonster_Police_First_Spine(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Police_First_Spine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Police_First_Spine::Clone(void* pArg)
{
	CMonster_Police_First_Spine* pInstance = new CMonster_Police_First_Spine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Police_First_Spine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Police_First_Spine::Free()
{
	__super::Free();


}
