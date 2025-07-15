#include "stdafx.h"
#include "..\Public\MiniGame_Bot.h"

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
#include "Player_Bullet.h"
#include "TileMgr.h"
#include "Skill.h"
#include "Damage.h"

CMiniGame_Bot::CMiniGame_Bot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CActor{ pGraphic_Device }
{
}

CMiniGame_Bot::CMiniGame_Bot(const CMiniGame_Bot& Prototype)
	: CActor(Prototype)
{
}

HRESULT CMiniGame_Bot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Bot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	MINIGAME_BOT_DESC* pDesc = static_cast<MINIGAME_BOT_DESC*>(pArg);

	m_ePlayerID = pDesc->ePlayerID;
	m_eBullet_ID = pDesc->eBulletID;
	m_eCurState = PLAYER_STATE::STATE_IDLE;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Add_Part()))
		return E_FAIL;

	for (size_t i = 0; i < PART_END; ++i)
	{
		m_pParts[i]->Set_Angle(Calculate_FireAngle());// 각도
	}

	m_tActorDesc.iMaxHp = 100;
	m_tActorDesc.iHp = 100;
	m_tActorDesc.iLevel = 1;
	m_pTransformCom->Scaling(1.4f, 1.4f, 1.4f);
	m_pColliderCom->Scaling(1.f, 1.f, 1.f);

	m_fSpeed = 3.f;
	m_fHitDelay = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPos);
	
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_MINIGAME, TEXT("Layer_Player"), TEXT("Com_Transform")));

	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		m_iTotalBulletCount = 12;
		m_tActorDesc.iAttack = 20;
		m_fFireDistance = 6.5f;
		break;

	case PLAYER_CHARACTER::PLAYER_DOG:
		m_iTotalBulletCount = 30;
		m_tActorDesc.iAttack = 7;
		m_fFireDistance = 4.5f;
		break;

	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		m_iTotalBulletCount = 7;
		m_tActorDesc.iAttack = 12;
		m_fFireDistance = 2.5f;
		break;

	default:
		break;
	}
	m_iBulletCount = m_iTotalBulletCount;

	m_tActorDesc.iLevel = 1;

	return S_OK;
}

_uint CMiniGame_Bot::Priority_Update(_float fTimeDelta)
{
	Set_State();

	if (true == m_bSlow)
		m_fSpeed = 1.f;
	else 
		m_fSpeed = 3.f;

	if(false == m_bStun)
	{
		for (size_t i = 0; i < PART_END; ++i)
		{
			m_pParts[i]->Set_Angle(Calculate_FireAngle());
		}
	}

	m_pFSM->Priority_Update(fTimeDelta);
	
	return OBJ_NOEVENT;
}

void CMiniGame_Bot::Update(_float fTimeDelta)
{
	if (true == m_bStun)
	{
		m_eCurState = PLAYER_STATE::STATE_IDLE;
	}
	else
	{
		if (FAILED(Player_Move(fTimeDelta)))
		{
			if (m_iBulletCount > 0 && true == m_bFire)
				m_eCurState = PLAYER_STATE::STATE_ATTACK;
			else
				m_eCurState = PLAYER_STATE::STATE_IDLE;
		}
		else
		{
			if (m_iBulletCount > 0 && true == m_bFire)
				m_eCurState = PLAYER_STATE::STATE_MOVEATTACK;
			else
				m_eCurState = PLAYER_STATE::STATE_MOVE;
		}
	}
	// 테스트
	m_pFSM->Update(fTimeDelta);
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CMiniGame_Bot::Late_Update(_float fTimeDelta)
{
	m_fHitDelay += fTimeDelta;
	m_fFireDelay += fTimeDelta;
	m_SkillDelay += fTimeDelta;
	m_fFindNearTime += fTimeDelta;
	if (m_iBulletCount <= 0)
		m_fReloadTime += fTimeDelta;
	if (true == m_bStun)
		m_fStunTime += fTimeDelta;

	if (m_SkillDelay >= 10.f && false == m_bStun)
	{
		m_SkillDelay = 0.f;
		m_bUseSkill = true;
	}
	if (m_fReloadTime > 1.f)
	{
		m_iBulletCount = m_iTotalBulletCount;
		m_fReloadTime = 0.f;
	}
	if (m_fStunTime > 3.f)
	{
		m_fStunTime = 0.f;
		m_bStun = false;
		m_tActorDesc.iHp = m_tActorDesc.iMaxHp;
	}

	if (m_tActorDesc.iHp <= 0)
	{
		m_bStun = true;
	}
	m_bSlow = false;

	m_pFSM->Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMiniGame_Bot::Render()
{

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pFSM->Render();
	m_pColliderCom->Render();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMiniGame_Bot::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (m_fHitDelay > 0.5f)
	{
		if (TEXT("Layer_Player_Skill") == strLayerTag)
		{
			CSkill* pSkill = static_cast<CSkill*>(pOther);
			if(true == pSkill->Get_Active())
			{
				CDamage::DAMAGE_DESC desc = {};
				switch (pSkill->Get_Character())
				{
				case PLAYER_CHARACTER::PLAYER_CAT:
					m_bSlow = true;
					m_tActorDesc.iHp -= pSkill->Get_Attack();

					desc.iDamage = pSkill->Get_Attack();
					desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

					m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
						TEXT("Prototype_GameObject_Damage"), &desc);
					break;
				case PLAYER_CHARACTER::PLAYER_PENGUIN:
					m_bStun = true;
					m_tActorDesc.iHp -= pSkill->Get_Attack();

					desc.iDamage = pSkill->Get_Attack();
					desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

					m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
						TEXT("Prototype_GameObject_Damage"), &desc);
					break;
				}
			}
			m_fHitDelay = 0.f;
		}

		switch (m_eBullet_ID)
		{
		case FIRST_BOT:
			if (TEXT("Layer_SECOND_Bot_Skill") == strLayerTag)
			{
				CSkill* pSkill = static_cast<CSkill*>(pOther);
				if(true == pSkill->Get_Active())
				{
					switch (pSkill->Get_Character())
					{
					case PLAYER_CHARACTER::PLAYER_CAT:
						m_bSlow = true;
						m_tActorDesc.iHp -= pSkill->Get_Attack();
						m_fHitDelay = 0.f;
						break;
					case PLAYER_CHARACTER::PLAYER_PENGUIN:
						m_bStun = true;
						m_tActorDesc.iHp -= pSkill->Get_Attack();
						m_fHitDelay = 0.f;
						break;
					}
				}
			}
			break;

		case SECOND_BOT:
			if (TEXT("Layer_Bot_Skill") == strLayerTag)
			{
				CSkill* pSkill = static_cast<CSkill*>(pOther);
				if(true == pSkill->Get_Active())
				{
					switch (pSkill->Get_Character())
					{
					case PLAYER_CHARACTER::PLAYER_CAT:
						m_bSlow = true;
						m_tActorDesc.iHp -= pSkill->Get_Attack();
						m_fHitDelay = 0.f;
						break;
					case PLAYER_CHARACTER::PLAYER_PENGUIN:
						m_bStun = true;
						m_tActorDesc.iHp -= pSkill->Get_Attack();
						m_fHitDelay = 0.f;
						break;
					}
				}
			}
			break;
		}
	}
	if (TEXT("Layer_Player_Bullet") == strLayerTag)
	{
		CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
		m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();
		pBullet->Set_Dead();
		m_fHitDelay = 0.f;

		CDamage::DAMAGE_DESC desc = {};
		desc.iDamage = pBullet->Get_Player_mDamage();
		desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
			TEXT("Prototype_GameObject_Damage"), &desc);
	}

	switch (m_eBullet_ID)
	{
	case FIRST_BOT:
		if (TEXT("Layer_SECOND_Bot_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
			m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();
			pBullet->Set_Dead();
			m_fHitDelay = 0.f;
		}
		break;
	case SECOND_BOT:
		if (TEXT("Layer_Bot_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
			m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();
			pBullet->Set_Dead();
			m_fHitDelay = 0.f;
		}
		break;
	}


	return S_OK;
}

HRESULT CMiniGame_Bot::Ready_Components()
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

HRESULT CMiniGame_Bot::SetUp_RenderState()
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

HRESULT CMiniGame_Bot::Reset_RenderState()
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

HRESULT CMiniGame_Bot::Add_Part()
{
	CPart::PART_DESC desc = {};

	desc.pTransformCom = m_pTransformCom;
	desc.pVIBufferCom = m_pVIBufferCom;

	m_pParts[PART_HEAD] = CHead::Create(m_pGraphic_Device, LEVEL_MINIGAME, &desc, m_ePlayerID);

	m_pParts[PART_BODY] = CBody::Create(m_pGraphic_Device, LEVEL_MINIGAME, &desc, m_ePlayerID);

	m_pParts[PART_WEAPON] = CWeapon::Create(m_pGraphic_Device, LEVEL_MINIGAME, &desc, m_ePlayerID);

	return S_OK;
}

_float CMiniGame_Bot::Calculate_FireAngle()
{
	CLayer* pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_MINIGAME, TEXT("Layer_Player"));
	CLayer* pBotLayer = m_pGameInstance->Find_Layer(LEVEL_MINIGAME, TEXT("Layer_MiniGame_Bot"));

	if (nullptr == pPlayerLayer && nullptr == pBotLayer)
		return 0.f;

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vPlayerPos = {};
	_float3 vPlayerDir = {};
	if (nullptr != pPlayerLayer)
	{
		auto& PlayerList = pPlayerLayer->Get_Objects();
		CPlayer* pPlayer = static_cast<CPlayer*>(*PlayerList.begin());

		vPlayerDir = pPlayer->Get_Pos() - vPos;
		vPlayerPos = pPlayer->Get_Pos();
	}

	_float3 vBotPos = {};
	_float3 vBotDir = {};
	if (nullptr != pBotLayer)
	{
		auto& BotList = pBotLayer->Get_Objects();
		_bool bFirst = { false };
		for (auto& elem : BotList)
		{
			CMiniGame_Bot* pBot = static_cast<CMiniGame_Bot*>(elem);
			if (m_ePlayerID == pBot->Get_Character())
				continue;

			// 가장 짧은 놈 찾을 것.
			if(false == bFirst)
			{
				vBotDir = pBot->Get_Pos() - vPos;
				vBotPos = pBot->Get_Pos();
				bFirst = true;
			}
			else
			{
				_float3 vNowDir = pBot->Get_Pos() - vPos;
				if (D3DXVec3Length(&vBotDir) > D3DXVec3Length(&vNowDir))
				{
					vBotDir = vNowDir;
					vBotPos = pBot->Get_Pos();
				}
			}
		}
	}

	_float3 vShortestDir = {};
	if (nullptr != pPlayerLayer && nullptr != pBotLayer)
	{
		if (D3DXVec3Length(&vPlayerDir) > D3DXVec3Length(&vBotDir))
		{
			vShortestDir = vBotDir;
			m_vTargetPos = vBotPos;
		}
		else
		{
			vShortestDir = vPlayerDir;
			m_vTargetPos = vPlayerPos;
		}
	}
	else if (nullptr == pPlayerLayer)
	{
		vShortestDir = vBotDir;
		m_vTargetPos = vBotPos;
	}
	else if (nullptr == pBotLayer)
	{
		vShortestDir = vPlayerDir;
		m_vTargetPos = vPlayerPos;
	}

	if (D3DXVec3Length(&vShortestDir) < m_fFireDistance)
		m_bFire = true;
	else
		m_bFire = false;

	// 각도
	return Compute_RadianAngle(vShortestDir);
}

_float CMiniGame_Bot::Compute_RadianAngle(_float3 vDir)
{
	D3DXVec3Normalize(&vDir, &vDir);
	_float fCos = D3DXVec3Dot(&vDir, &_float3(1.f, 0.f, 0.f));

	_float fRadian = acos(fCos);

	if (m_vTargetPos.z < m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		fRadian *= -1.f;

	return fRadian;
}


void CMiniGame_Bot::Set_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case PLAYER_STATE::STATE_IDLE:
			m_pFSM->Change_State(CState_Idle::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_MINIGAME));
			break;
			
		case PLAYER_STATE::STATE_MOVE:
			m_pFSM->Change_State(CState_Move::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_MINIGAME));
			break;

		case PLAYER_STATE::STATE_ATTACK:
			m_pFSM->Change_State(CState_Attack::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_MINIGAME));
			break;

		case PLAYER_STATE::STATE_MOVEATTACK:
			m_pFSM->Change_State(CState_MoveAttack::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, (_int)m_eBullet_ID, LEVEL_MINIGAME));
			break;

		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

HRESULT CMiniGame_Bot::Player_Move(_float fTimeDelta)
{
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	CLayer* pTileMgrLayer = m_pGameInstance->Find_Layer(LEVEL_MINIGAME, TEXT("Layer_TileMgr"));
	CTileMgr* pTileMgr = static_cast<CTileMgr*>(pTileMgrLayer->Get_Objects().front());

	CTile::TILE_TYPE eTileType = { CTile::TILE_END };
	switch (m_ePlayerID)
	{
	case PLAYER_CHARACTER::PLAYER_CAT:
		eTileType = CTile::TILE_MUSEUM_0;
		break;

	case PLAYER_CHARACTER::PLAYER_DOG:
		eTileType = CTile::TILE_PHONE_0;
		break;

	case PLAYER_CHARACTER::PLAYER_PENGUIN:
		eTileType = CTile::TILE_MAGIC_0;
		break;
	}

	if(m_fFindNearTime > 1.f)
	{
		m_vMarkPos = pTileMgr->Search_NearTile(vBotPos, eTileType);
		m_fFindNearTime = 0.f;
	}

	_float3 vDir = m_vMarkPos - vBotPos;
	D3DXVec3Normalize(&vDir, &vDir);

	vBotPos += vDir * m_fSpeed * fTimeDelta;

	if (FAILED(pTileMgr->Change_Three(vBotPos, eTileType)))
		return E_FAIL;

	if (FAILED(pTileMgr->Check_MoveTile(vBotPos)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBotPos);

	return S_OK;
}

CMiniGame_Bot* CMiniGame_Bot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMiniGame_Bot* pInstance = new CMiniGame_Bot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMiniGame_Bot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMiniGame_Bot::Clone(void* pArg)
{
	CMiniGame_Bot* pInstance = new CMiniGame_Bot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CMiniGame_Bot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Bot::Free()
{
	__super::Free();

	for (size_t i = 0; i < PART_END; ++i)
		Safe_Release(m_pParts[i]);

	Safe_Release(m_pFSM);
	Safe_Release(m_pColliderCom);
}

