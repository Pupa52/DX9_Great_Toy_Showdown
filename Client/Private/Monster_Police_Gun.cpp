#include "stdafx.h"
#include "..\Public\Monster_Police_Gun.h"

#include "GameInstance.h"
#include "Monster_Police.h"
CMonster_Police_Gun::CMonster_Police_Gun(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Police_Gun::CMonster_Police_Gun(const CMonster_Police_Gun& Prototype)
	:CMonster{ Prototype }
{
}


HRESULT CMonster_Police_Gun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Police_Gun::Initialize(void* pArg)
{

	if (FAILED(CActor::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_tActorDesc.iAttack = 0;
	m_tActorDesc.iMaxHp = 100;
	m_tActorDesc.iHp = 100;
	m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Shot"));
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
	
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(44.f, 10.f, 36.f));
	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	m_pColliderCubeCom->Scaling(0.1f, 0.1f, 0.1f);
	return S_OK;
}

_uint CMonster_Police_Gun::Priority_Update(_float fTimeDelta)
{
	if (m_bDeadMotion && (m_iFrameCounter > 0))
		return OBJ_DEAD;

	if (m_iFrameCounter > 0)
	{
		m_bCheck = false;
	}

	if (static_cast<CMonster_Police*>(m_pMonster)->Get_Dead())
	{
		m_bdead = true;
		m_pMonster = nullptr;	
	}
	if (m_pMonster != nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(static_cast<CMonster_Police*>(m_pMonster)->Get_Pos().x, static_cast<CMonster_Police*>(m_pMonster)->Get_Pos().y, static_cast<CMonster_Police*>(m_pMonster)->Get_Pos().z));
	}

	if (m_bdead)
	{
		return OBJ_DEAD;
	}


	return OBJ_NOEVENT;
}

void CMonster_Police_Gun::Update(_float fTimeDelta)
{
	MoveFrame += fTimeDelta;
	int Rand = rand() % 2;


	if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_IDLE && !m_bCheck)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Idle"));
		m_bCheck = true;
		m_iFrameCounter = 0;
	}
	else if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_DIE && !m_bCheck)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Die"));
		m_bCheck = true;
		m_iFrameCounter = 0;
	}
	else if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_MOVE && !m_bCheck)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Move"));
		m_bCheck = true;
		m_iFrameCounter = 0;
	}
	else if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_SHOT_READY && !m_bCheck)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Shot_Ready"));
		m_bCheck = true;
		m_bAttack = true;
		MoveFrame = 0;
		m_iFrameCounter = 0;
	}
	else if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_AIM)
	{
		_float3 fGunPos = {};
		fGunPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(fGunPos.x, fGunPos.y+0.5f, fGunPos.z));
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Aim"));
		m_bCheck = true;

		if (MoveFrame <= 4)
		{
			m_fLastAngle = Gun_Angle();
			//MoveFrame = 0;
		}
		m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), m_fLastAngle);

		//m_iFrameCounter = 0;
	}
	else if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_SHOT_RETURN && !m_bCheck)
	{
		m_bAttack = false;
		_float3 fGunPos = {};
		fGunPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(static_cast<CMonster_Police*>(m_pMonster)->Get_Pos().x, static_cast<CMonster_Police*>(m_pMonster)->Get_Pos().y, static_cast<CMonster_Police*>(m_pMonster)->Get_Pos().z));
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Gun_Shot_Back"));
		m_bCheck = true;
		m_iFrameCounter = 0;
	}

	if (static_cast<CMonster_Police*>(m_pMonster)->Get_Attack_State() == CMonster_Police::POLICE_AIM)
	{
		if (MoveFrame >= 2 && m_bAttack)
		{

			m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_Police_Laser"), &m_tActorDesc);
			m_bAttack = false;
		}
		//m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), 90.f);

	}
	else
	{
		m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), 0.f);
	}
	//__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CMonster_Police_Gun::Late_Update(_float fTimeDelta)
{

	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_iFrameCounter++;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster_Police_Gun::Render()
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

HRESULT CMonster_Police_Gun::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CMonster_Police_Gun::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Gun_Idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Gun_Die"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Die"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Gun_Move"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Move"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Gun_Shot_Back"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Shot_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Gun_Shot_Ready"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Shot_Ready"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Gun_Aim"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Gun_Aim"))));

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

HRESULT CMonster_Police_Gun::SetUp_RenderState()
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

HRESULT CMonster_Police_Gun::Reset_RenderState()
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

_float CMonster_Police_Gun::Gun_Angle()
{
	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMonster_Pos, vPlayer_Pos;
	vPlayer_Pos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vMonster_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	MonsterMatrix = PlayerMatrix - MonsterMatrix;

	D3DXVec3Normalize(&MonsterMatrix, &MonsterMatrix);

	_float fDot = D3DXVec3Dot(&MonsterMatrix, &_float3(0.f, 0.f, 1.f));
	_float fAngle = acosf(fDot);

	if (vMonster_Pos.x < vPlayer_Pos.x)
		fAngle *= -1.f;

	return fAngle;
}


CMonster_Police_Gun* CMonster_Police_Gun::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonster_Police_Gun* pInstance = new CMonster_Police_Gun(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Police_Gun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Police_Gun::Clone(void* pArg)
{
	CMonster_Police_Gun* pInstance = new CMonster_Police_Gun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Police_Gun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Police_Gun::Free()
{
	__super::Free();


}
