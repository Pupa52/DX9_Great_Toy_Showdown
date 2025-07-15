#include "stdafx.h"
#include "..\Public\Monster_Magic_mirror.h"

#include "Player_Bullet.h"
#include "Effect_Shadow.h"
#include "Layer.h"
#include "TileMgr.h"
#include "Damage.h"

CMonster_Magic_mirror::CMonster_Magic_mirror(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Magic_mirror::CMonster_Magic_mirror(const CMonster_Magic_mirror& Prototype)
	:CMonster{ Prototype }
{
}

HRESULT CMonster_Magic_mirror::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Magic_mirror::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC	Desc = {};

	Desc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	ACTOR_DESC* Act_Desc = static_cast<ACTOR_DESC*>(pArg);
	if (FAILED(CActor::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pAnimatorCom->Change_Texture(TEXT("Monster_Mirror_Back_to_Front"));
	m_fAnim_Time = 0.f;
	// 피해반사
	m_tActorDesc.iMaxHp = 900;
	m_tActorDesc.iHp = 900;
	m_tActorDesc.iAttack = 10;
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_eState = Mirror_State_Back_to_Front;
	
	m_bDangerous_Monster = true;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(Act_Desc->m_targetPos.x, Act_Desc->m_targetPos.y, Act_Desc->m_targetPos.z));

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Shadow"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);

	if (m_pShadow == nullptr)
	{
		CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

		if (nullptr == Shadow)
			return E_FAIL;

		CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
	}

	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	m_pColliderCubeCom->Scaling(0.3f, 1.f, 0.2f);
	return S_OK;
}

_uint CMonster_Magic_mirror::Priority_Update(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_bDeadMotion && (m_iFrameCounter > 0))
		return OBJ_DEAD;

	if (m_fLifeTime >= 10)
	{
		m_tActorDesc.iHp = 0;
	}
	if (m_bdead)
	{
		m_bHitEffect = true;
		if (m_bHitEffect)
		{
			m_fHitEffect_Time = 0;
		}
		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Die_Dust_Effect"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"), &m_tActorDesc);
		//m_tActorDesc.iHp -= m_iGet_dmg;
		m_bdead = false;
	}

	return OBJ_NOEVENT;
}

void CMonster_Magic_mirror::Update(_float fTimeDelta)
{

	if (m_bStuned)
	{
		m_fAnim_Time += fTimeDelta;
		if (m_fAnim_Time >= 3)
			m_bStuned = false;
	}
	if (m_fHitEffect_Time >= 0.1f)
	{
		m_fHitEffect_Time = 0;
		m_bHitEffect = false;
	}
	if (m_bHitEffect)
	{
		m_fHitEffect_Time += fTimeDelta;
	}
	m_fTimeTemp = fTimeDelta;
	m_fHittimer += fTimeDelta;

	if (m_Monster_Statue == MONSTER_STATE_DEFAULT)
	{
		m_iSpeed = 1.f;
	}
	else if (m_Monster_Statue == MONSTER_STATE_STUN)
	{
		m_bStuned = true;
		m_Monster_Statue = MONSTER_STATE_DEFAULT;
	}
	else if (m_Monster_Statue == MONSTER_STATE_SLOW)
	{
		m_iSpeed = .5f;
	}
	if (m_tActorDesc.iHp <= 0 && !m_bDeadMotion)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Mirror_Die_Front"));
		m_eState = Mirror_State_Die;
		m_bDeadMotion = true;
		m_iFrameCounter = 0;
	}

	if (m_eState == Mirror_State_Back_to_Front && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Mirror_Idle_Front"));
		m_eState = Mirror_State_Idle;
		m_iFrameCounter = 0;
		m_bCanMove = true;
	}
	else if (m_eState == Mirror_State_Idle && (m_iFrameCounter > 0))
	{
		m_iFrameCounter = 0;
	}
	if (m_bCanMove)
	{
		Player_Stalking(fTimeDelta);
	}

	__super::SetUp_OnTerrain(m_pTransformCom, 1.f);
}

void CMonster_Magic_mirror::Late_Update(_float fTimeDelta)
{

	if (m_Monster_Statue == MONSTER_STATE_SLOW)
	{
		if (m_fHittimer >= 1.f)
		{
			m_bHitEffect = true;
			if (m_bHitEffect)
			{
				m_fHitEffect_Time = 0;
			}
			m_Monster_Statue = MONSTER_STATE_DEFAULT;
			m_fHittimer = 0;
			m_tActorDesc.iHp -= 10;
			CDamage::DAMAGE_DESC desc = {};
			desc.iDamage = 10;
			desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
				TEXT("Prototype_GameObject_Damage"), &desc);
		}
	}

	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_iFrameCounter++;

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster_Magic_mirror::Render()
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

HRESULT CMonster_Magic_mirror::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	_float3 _fpOther_Pos, _fmyPos;

	if (strLayerTag == TEXT("Layer_Player_Bullet"))
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_HIT));
		m_pGameInstance->SoundPlay(L"MonsterHit.wav", _uint(CHANNELID::SOUND_HIT), .5f);
		
		// 플레이어 멀어지면 소리 안남
		_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
			PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
		{
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_HIT));
		}

		m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
		CDamage::DAMAGE_DESC desc = {};
		desc.iDamage = m_iGet_dmg;
		desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
			TEXT("Prototype_GameObject_Damage"), &desc);
		m_bdead = true;
	}
	if (strLayerTag == TEXT("Layer_Monster"))
	{
		_fpOther_Pos = static_cast<CMonster_Magic_mirror*>(pOther)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_fmyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (_fmyPos.x < _fpOther_Pos.x)
			m_pTransformCom->Go_Left(m_fTimeTemp * 0.5f);
		else if (_fmyPos.x >= _fpOther_Pos.x)
			m_pTransformCom->Go_Right(m_fTimeTemp * 0.5f);
	
		if (_fmyPos.z < _fpOther_Pos.z)
			m_pTransformCom->Go_Backward(m_fTimeTemp * 0.5f);
		else if (_fmyPos.z >= _fpOther_Pos.z)
			m_pTransformCom->Go_Straight(m_fTimeTemp * 0.5f);
	}


	return S_OK;
}


HRESULT CMonster_Magic_mirror::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Mirror_Back_to_Front"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Back_to_Front"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Mirror_Front_to_Back"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Front_to_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Mirror_Die_Back"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Die_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Mirror_Die_Front"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Mirror_Front_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Mirror_Idle_Back"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Idle_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Mirror_Idle_Front"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Idle_Front"))));

	//  ENd


		/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CMonster_Magic_mirror::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 0, 0));

	if (m_bHitEffect)
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

HRESULT CMonster_Magic_mirror::Reset_RenderState()
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

void CMonster_Magic_mirror::Player_Stalking(_float fTimeDelta)
{
	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVECTOR3 dir = MonsterMatrix - PlayerMatrix;
	D3DXVec3Normalize(&dir, &dir);
	MonsterMatrix = MonsterMatrix - dir * fTimeDelta * m_iSpeed;



	_float3 vMoveDir = { 0.f, 0.f, 0.f };

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TileMgr"));
	CTileMgr* pMgr = static_cast<CTileMgr*>(pLayer->Get_Objects().front());

	if ((S_OK == pMgr->Check_MoveTile(MonsterMatrix)))
	{
		if (!m_bDeadMotion && !m_bStuned)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, MonsterMatrix);
	}

}


CMonster_Magic_mirror* CMonster_Magic_mirror::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CMonster_Magic_mirror* pInstance = new CMonster_Magic_mirror(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Magic_mirror"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Magic_mirror::Clone(void* pArg)
{

	CMonster_Magic_mirror* pInstance = new CMonster_Magic_mirror(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Magic_mirror"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Magic_mirror::Free()
{
	__super::Free();

}
