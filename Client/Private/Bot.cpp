#include "stdafx.h"
#include "..\Public\Bot.h"

#include "GameInstance.h"

#include "Head.h"
#include "Body.h"
#include "Weapon.h"

#include "State_Attack.h"
#include "State_Idle.h"
#include "State_Move.h"
#include "State_MoveAttack.h"

#include "Layer.h"
#include "TileMgr.h"
#include "Monster.h"
#include "Bullet.h"
#include "Player_Bullet.h"
#include "TileMgr.h"
#include "Storm.h"



#include "Effect_Shadow.h"

CBot::CBot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CActor{ pGraphic_Device }
{
}

CBot::CBot(const CBot& Prototype)
	: CActor(Prototype)
{
}

HRESULT CBot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
	m_ePlayerID = pDesc->ePlayerID;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Add_Part()))
		return E_FAIL;

	m_eCurState = PLAYER_STATE::STATE_IDLE;

	for (size_t i = 0; i < PART_END; ++i)
	{
		m_pParts[i]->Set_Angle(Calculate_FireAngle());// 각도
	}

	m_tActorDesc.iMaxHp = 500;
	m_tActorDesc.iHp = 500;
	m_pTransformCom->Scaling(1.4f, 1.4f, 1.4f);
	m_pColliderCom->Scaling(1.f, 1.f, 1.f);

	m_fSpeed = 3.f;
	m_fHitDelay = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPos);
	if (m_pShadow == nullptr)
	{
		CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

		if (nullptr == Shadow)
			return E_FAIL;

		CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_Player();
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		m_iTotalBulletCount = 12;
		m_tActorDesc.iAttack = 20;
		break;

	case PLAYER_CHARACTER::PLAYER_DOG:
		m_iTotalBulletCount = 30;
		m_tActorDesc.iAttack = 7;
		break;

	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		m_iTotalBulletCount = 7;
		m_tActorDesc.iAttack = 12;
		break;

	default:
		break;
	}
	m_iBulletCount = m_iTotalBulletCount;

	m_tActorDesc.iMaxExp = 10;
	m_tActorDesc.iExp = 0;

	m_tActorDesc.iLevel = 1;

	return S_OK;
}

_uint CBot::Priority_Update(_float fTimeDelta)
{
	if (m_tActorDesc.iHp <= 0)
	{
		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Burn_Smoke"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Rip"), &m_tActorDesc);

		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_DIE));
		m_pGameInstance->SoundPlay(L"UI_Player_Kill.wav", _uint(CHANNELID::SOUND_DIE), .8f);
		
		_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
			PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
		{
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_DIE));
		}

		return OBJ_DEAD;
	}

	if (m_bDead)
	{
		if (m_bRun) m_bRun = false;
		else if (!m_bRun) m_bRun = true;
		m_tActorDesc.iHp -= m_iGet_dmg;
		m_bDead = false;
	}

	Set_State();

	for (size_t i = 0; i < PART_END; ++i)
	{
		m_pParts[i]->Set_Angle(Calculate_FireAngle());
	}

	m_pFSM->Priority_Update(fTimeDelta);
	m_fFireDelay += fTimeDelta;
	m_SkillDelay += fTimeDelta;
	
	
	return OBJ_NOEVENT;
}

void CBot::Update(_float fTimeDelta)
{
	if (m_bPanic)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer >= 2)
		{
			m_fTimer = 0;
			m_bPanic = false;
		}
	}


	m_fMapFinder += fTimeDelta;
	m_fTrunTime += fTimeDelta;
	Calculate_Range += 0.5f * fTimeDelta;

	if (Calculate_Range >= 5.f)
	{
		Calculate_Range = 3.f;
	}

	if (m_fMapFinder >= 5)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPos.y += 1.f;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Scanner"), TEXT("Prototype_GameObject_Scanner"), &vPos);
		m_fMapFinder = 0;
	}

	if (FAILED(Player_Move(fTimeDelta)))
	{
		if (m_Detected_Enemy && m_iBulletCount > 0)
		{
			m_eCurState = PLAYER_STATE::STATE_ATTACK;
			m_Detected_Enemy = false;
		}
		else
		{
			m_eCurState = PLAYER_STATE::STATE_IDLE;

		}
	}
	else
	{
		if (m_Detected_Enemy && m_iBulletCount > 0)
		{
			m_eCurState = PLAYER_STATE::STATE_MOVEATTACK;
			m_Detected_Enemy = false;
		}
		else
		{
			m_eCurState = PLAYER_STATE::STATE_MOVE;

		}
	}

	if (m_Detected_Player && m_SkillDelay >= 20.f)
	{
		m_SkillDelay = 0.f;
		m_bUseSkill = true;
	}
	// 테스트
	m_pFSM->Update(fTimeDelta);
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CBot::Late_Update(_float fTimeDelta)
{

	if (m_Bot_Statue == BOT_STATE_SLOW)
	{
		//m_Bot_Statue = BOT_STATE_DEFAULT;
	}

	m_fHitDelay += fTimeDelta;

	if (m_iBulletCount <= 0)
		m_fReloadTime += fTimeDelta;

	if (m_tActorDesc.iExp > m_tActorDesc.iMaxExp)
	{
		++m_tActorDesc.iLevel;

		m_tActorDesc.iExp = 0;
		m_tActorDesc.iMaxExp += 80;
		m_tActorDesc.iAttack += 2;
	}

	if (m_fReloadTime > 1.f)
	{
		m_iBulletCount = m_iTotalBulletCount;
		m_fReloadTime = 0.f;
	}

	_float fStormRadius = static_cast<CStorm*>
		(*m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Storm"))->Get_Objects().begin())->Get_Radius();

	// 중점
	_float3 vCenter = { 59.5f, 1.f, 59.5f };
	_float3 vDir = vCenter - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_fStormLength = D3DXVec3Length(&vDir);

	if (m_fStormLength > fStormRadius)
	{
		if (m_fHitDelay > 0.5f)
		{
			m_tActorDesc.iHp -= 10;
			m_fHitDelay = 0.f;
		}
	}
	m_pFSM->Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBot::Render()
{

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pFSM->Render();
	m_pColliderCom->Render();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBot::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (m_fHitDelay > 0.5f)
	{
		if (TEXT("Layer_Monster") == strLayerTag)
		{
			m_bPanic = true;

			CMonster* pMonster = static_cast<CMonster*>(pOther);
			m_tActorDesc.iHp -= 0;

			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}

			m_iGet_dmg = 1;
			m_bDead = true;

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_Monster_Bullet") == strLayerTag)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			m_tActorDesc.iHp -= pBullet->Get_Bullet_DESC()->iAttack;

			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CMonster*>(pOther)->Get_ActorDesc().iAttack;
			m_bDead = true;

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_Player_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
			m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();

			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}

			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();
			m_bDead = true;

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_Friend_Bot_Bullet") == strLayerTag)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);	
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;
		
		
			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
		// 봇총알
		if (TEXT("Layer_SECOND_Bot_Bullet") == strLayerTag && m_eBullet_ID == FIRST_BOT)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;

			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_THIRD_Bot_Bullet") == strLayerTag && m_eBullet_ID == FIRST_BOT)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;


			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_Bot_Bullet") == strLayerTag && m_eBullet_ID == SECOND_BOT)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;


			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_THIRD_Bot_Bullet") == strLayerTag && m_eBullet_ID == SECOND_BOT)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;


			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_SECOND_Bot_Bullet") == strLayerTag && m_eBullet_ID == THIRD_BOT)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;


			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_Bot_Bullet") == strLayerTag && m_eBullet_ID == THIRD_BOT)
		{
			CBullet* pBullet = static_cast<CBullet*>(pOther);
			BOT_STATUE m_Bot_Statue = BOT_STATE_SLOW;


			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			}
			_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
			m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

			m_iGet_dmg = static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
			m_bDead = true;
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();

			m_fHitDelay = 0.f;
		}
	}

	return S_OK;
}

HRESULT CBot::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 20.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBot::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 0, 0));

	if (m_fHitDelay <= 0.2f)
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

	/* 알파값이 0보다 큰 것들만 그리겠다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);

	return S_OK;
}

HRESULT CBot::Reset_RenderState()
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
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CBot::Add_Part()
{
	CPart::PART_DESC desc = {};

	desc.pTransformCom = m_pTransformCom;
	desc.pVIBufferCom = m_pVIBufferCom;

	m_pParts[PART_HEAD] = CHead::Create(m_pGraphic_Device, LEVEL_GAMEPLAY, &desc, m_ePlayerID);

	m_pParts[PART_BODY] = CBody::Create(m_pGraphic_Device, LEVEL_GAMEPLAY, &desc, m_ePlayerID);

	m_pParts[PART_WEAPON] = CWeapon::Create(m_pGraphic_Device, LEVEL_GAMEPLAY, &desc, m_ePlayerID);

	return S_OK;
}

_float CBot::Calculate_MouseDir()
{
	POINT			ptMouse{};

	GetCursorPos(&ptMouse);

	/* 뷰포트 == 로컬 * 월드행렬 * 뷰행렬 * 투영행렬 /w -> 뷰포트 변환 */
	ScreenToClient(g_hWnd, &ptMouse);

	/* 투영스페이스 == 로컬 * 월드행렬 * 뷰행렬 * 투영행렬 /w */
	_float3		vMousePos{};
	vMousePos.x = ptMouse.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (g_iWinSizeY * -0.5f) + 1.f;
	vMousePos.z = 0.f; /* Near평면을 클릭한거야!! */

	/* 뷰스페이스 == 로컬 * 월드행렬 * 뷰행렬 */
	_float4x4			ProjMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

	/* 뷰스페이스 상에서의 마우스 레이와 레이의 시작점을 구해놓는다. */
	/* 뷰스페이스 상이기 때문에 카메라 좌표는 원점이다. */

	_float3 vRayPos = _float3(0.f, 0.f, 0.f);
	_float3 vRayDir = vMousePos - vRayPos;

	/* 월드스페이스 == 로컬 * 월드행렬 */
	_float4x4			ViewMatrixInv{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &ViewMatrixInv);

	D3DXVec3Normalize(&vRayDir, &vRayDir);

	_float3 vTargetPos = {};
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDist = (vBotPos.y - vRayPos.y) / vRayDir.y;


	vTargetPos.x = vRayPos.x + vRayDir.x * fDist;
	vTargetPos.y = vBotPos.y;
	vTargetPos.z = vRayPos.z + vRayDir.z * fDist;

	m_vTargetPos = vTargetPos;

	_float3 vTargetDir = vTargetPos - vBotPos;

	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float fDot = D3DXVec3Dot(&vTargetDir, &_float3(1.f, 0.f, 0.f));
	_float fAngle = acosf(fDot);

	if (vTargetPos.z < vBotPos.z)
		fAngle *= -1.f;

	return fAngle;
}

_float CBot::Calculate_FireAngle()
{
	_float fResult, fr;
	_float3 fTempDistance;


	_float3 vTargetPos = {};
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	CLayer* MonsterLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Monster")));
	CLayer* PlayerLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Player")));
	CLayer* BotLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Bot")));
	CLayer* Bot_FriEnd_Layer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Friend_Bot")));

	if (nullptr == MonsterLayer || nullptr == PlayerLayer || nullptr == Bot_FriEnd_Layer || nullptr == BotLayer)
		return false;

	list<CGameObject*> Monster_Obj = MonsterLayer->Get_Objects();
	list<CGameObject*> Player_Obj = PlayerLayer->Get_Objects();
	list<CGameObject*> Bot_Obj = BotLayer->Get_Objects();
	list<CGameObject*> BotFriEnd_Obj = Bot_FriEnd_Layer->Get_Objects();

	for (auto& pSrc : Bot_Obj)
	{
		for (auto& pDst : Monster_Obj)
		{
			fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CMonster*>(pDst)->Get_Pos().x, 2) +
				powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CMonster*>(pDst)->Get_Pos().z, 2);


			fr = powf(Calculate_Range + 1.f, 2);

			if (fResult <= fr)
			{


				m_Detected_Enemy = true;
				fTempDistance = static_cast<CMonster*>(pDst)->Get_Pos();
				//몬스터;

				_float Posx = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - fTempDistance.x;
				_float Posy = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y - fTempDistance.y;
				_float Result = sqrtf(Posx + Posy);


				if (Range > Result && m_fTrunTime >= 0.5f)
				{
					m_bAttack = false;
					m_vTargetPos = fTempDistance;
					m_EnemyDistance = fTempDistance;
					m_fTrunTime = 0.f;
				}
			}
		}
		
		for (auto& pDst : BotFriEnd_Obj)
		{

			fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
				powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

			fr = powf(Calculate_Range + 1.f, 2);

			if (fResult <= fr)
			{
				m_Detected_Enemy = true;
				m_Detected_Player = true;
				if (static_cast<CPlayer*>(pDst)->m_tActorDesc.iHp < m_tActorDesc.iHp)
				{
					m_bAttack = true;
				}
				fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				//플레이어
				_float Posx = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - fTempDistance.x;
				_float Posy = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y - fTempDistance.y;
				_float Result = sqrtf(Posx + Posy);


				if (Range > Result)
				{
					m_vTargetPos = fTempDistance;
					m_EnemyDistance = fTempDistance;
				}
			}
		}

		for (auto& pDst : Player_Obj)
		{
			fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
				powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

			fr = powf(Calculate_Range + 1.f, 2);

			if (fResult <= fr)
			{
 				if (static_cast<CPlayer*>(pDst)->m_tActorDesc.iHp < m_tActorDesc.iHp)
				{
					m_bAttack = true;
				}
				fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				//플레이어

				_float Posx = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - fTempDistance.x;
				_float Posy = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y - fTempDistance.y;
				_float Result = sqrtf(Posx + Posy);


				if (Range > Result)
				{
					m_Detected_Enemy = true;
					m_Detected_Player = true;
					m_vTargetPos = fTempDistance;
					m_EnemyDistance = fTempDistance;
				}
			}
		}
		for (auto& pDst : Bot_Obj)
		{
			// 결국같은
			//if (pSrc == pDst)
			//{
			//	fTempDistance =	m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			//	m_vTargetPos = fTempDistance;
			//	m_EnemyDistance = fTempDistance;
			//	continue;
			//}

			fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
				powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

			fr = powf(Calculate_Range + 1.f, 2);
			if (fResult <= fr)
			{
				fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				_float Posx = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - fTempDistance.x;
				_float Posy = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y - fTempDistance.y;
				_float Result = sqrtf(Posx + Posy);
				if (Result <= 1)
					continue;
				if (static_cast<CPlayer*>(pDst)->m_tActorDesc.iHp < m_tActorDesc.iHp)
				{
					m_bAttack = true;
				}

				//플레이어
				if (Range > Result)
				{
					m_Detected_Enemy = true;
					m_Detected_Player = true;
					m_vTargetPos = fTempDistance;
					m_EnemyDistance = fTempDistance;
				}
			}


		}
	}
	Range = 1000;

	_float3 vTargetDir = m_EnemyDistance - vBotPos;

	vTargetDir.y = 1.f;

	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float fDot = D3DXVec3Dot(&vTargetDir, &_float3(1.f, 0.f, 0.f));
	_float fAngle = acosf(fDot);

	if (m_EnemyDistance.z < vBotPos.z)
		fAngle *= -1.f;

	return fAngle;
}


void CBot::Set_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case PLAYER_STATE::STATE_IDLE:
			m_pFSM->Change_State(CState_Idle::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_GAMEPLAY));
			break;
			
		case PLAYER_STATE::STATE_MOVE:
			m_pFSM->Change_State(CState_Move::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_GAMEPLAY));
			break;

		case PLAYER_STATE::STATE_ATTACK:
			m_pFSM->Change_State(CState_Attack::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_GAMEPLAY));
			break;

		case PLAYER_STATE::STATE_MOVEATTACK:
			m_pFSM->Change_State(CState_MoveAttack::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_GAMEPLAY));
			break;

		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}


HRESULT CBot::Player_Move(_float fTimeDelta)
{
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	HRESULT hr = E_FAIL;


	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TileMgr"));
	CTileMgr* pMgr = static_cast<CTileMgr*>(pLayer->Get_Objects().front());

	if (m_Bot_Statue == BOT_STATE_STUN && m_fTrunTime >= 2)
	{
		m_Bot_Statue = BOT_STATE_DEFAULT;
	}

	if (m_Bot_Statue == BOT_STATE_DEFAULT)
	{
		m_fSpeed = 3.f;
	}
	else if (m_Bot_Statue == BOT_STATE_STUN)
	{
		m_Detected_Enemy = false;
		m_Detected_Player = false;
		m_fSpeed = 0.f;
	}
	else if (m_Bot_Statue == BOT_STATE_SLOW)
	{
		m_Detected_Enemy = false;
		m_Detected_Player = false;
		m_fSpeed = 0.5f;
	}

	_float3 vMoveDir = { 0.f, 0.f, 0.f };
	if (m_Detected_Player && m_bAttack)
	{
		if (m_EnemyDistance.x + 1 >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x += m_fSpeed * fTimeDelta;
		}
		if (m_EnemyDistance.x - 1 < m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x -= m_fSpeed * fTimeDelta;
		}
		if (m_EnemyDistance.z + 1 >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z += m_fSpeed * fTimeDelta;
		}
		if (m_EnemyDistance.z - 1 < m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z -= m_fSpeed * fTimeDelta;
		}
	}
	else if (m_Detected_Enemy)
	{
		if (m_EnemyDistance.x >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x -= m_fSpeed * fTimeDelta;
		}
		if (m_EnemyDistance.x < m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x += m_fSpeed * fTimeDelta;
		}
		if (m_EnemyDistance.z >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z -= m_fSpeed * fTimeDelta;
		}
		if (m_EnemyDistance.z < m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z += m_fSpeed * fTimeDelta;
		}
	}

	if (!m_Detected_Enemy && m_fFireDelay >= 1 || m_bPanic)
	{
		m_bAttack = false;
		m_Detected_Player = false;
		if(!m_bRun)
		vMoveDir = m_BotResultTarget - vBotPos;
		else if(m_bRun)
		vMoveDir = m_BotResultTarget - vBotPos* -1.f;
	}

	if (vMoveDir != _float3{ 0.f, 0.f, 0.f })
	{
		vBotPos += *D3DXVec3Normalize(&vMoveDir, &vMoveDir) * fTimeDelta * m_fSpeed;
		hr = S_OK;
	}

	if (FAILED(pMgr->Check_MoveTile(vBotPos)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBotPos);
	return hr;
}

CBot* CBot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBot* pInstance = new CBot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBot::Clone(void* pArg)
{
	CBot* pInstance = new CBot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBot::Free()
{
	__super::Free();

	for (size_t i = 0; i < PART_END; ++i)
		Safe_Release(m_pParts[i]);

	Safe_Release(m_pFSM);
	Safe_Release(m_pColliderCom);
}

