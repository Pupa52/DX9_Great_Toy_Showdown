#include "stdafx.h"
#include "..\Public\Friend_Bot.h"

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
#include "Effect_Shadow.h"

CFriend_Bot::CFriend_Bot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CActor{ pGraphic_Device }
{
}

CFriend_Bot::CFriend_Bot(const CFriend_Bot& Prototype)
	: CActor(Prototype)
{
}

HRESULT CFriend_Bot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFriend_Bot::Initialize(void* pArg)
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

	m_tActorDesc.iMaxHp = 400;
	m_tActorDesc.iHp = 400;
	m_tActorDesc.iAttack = 5;
	m_pTransformCom->Scaling(1.4f, 1.4f, 1.4f);
	m_pColliderCom->Scaling(0.5f, 0.5f, 0.5f);

	m_fSpeed = 3.f;
	m_fHitDelay = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPos);
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Shadow"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);

	if (m_pShadow == nullptr)
	{
		CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

		if (nullptr == Shadow)
			return E_FAIL;

		CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_Player();
	}
	return S_OK;
}

_uint CFriend_Bot::Priority_Update(_float fTimeDelta)
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

void CFriend_Bot::Update(_float fTimeDelta)
{
	m_fMapFinder += fTimeDelta;
	m_fTrunTime += fTimeDelta;

	Calculate_Range += 0.05f * fTimeDelta;
	if (Calculate_Range >= 8.f)
		Calculate_Range = 6.f;

	if (m_fMapFinder >= 5)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPos.y += 1.f;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Scanner"), TEXT("Prototype_GameObject_Scanner"), &vPos);
		m_fMapFinder = 0;
	}
	if (FAILED(Player_Move(fTimeDelta)))
	{
		if (m_Detected_Enemy)
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
		if (m_Detected_Enemy)
		{
			m_eCurState = PLAYER_STATE::STATE_MOVEATTACK;
			m_Detected_Enemy = false;
		}
		else
		{
			m_eCurState = PLAYER_STATE::STATE_MOVE;

		}
	}

	if (m_SkillDelay >= 10.f)
	{

		m_SkillDelay = 0.f;
		m_bUseSkill = true;
	}

	
	// 테스트
	m_pFSM->Update(fTimeDelta);
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CFriend_Bot::Late_Update(_float fTimeDelta)
{
	m_fHitDelay += fTimeDelta;
	m_pFSM->Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CFriend_Bot::Render()
{

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pFSM->Render();
	m_pColliderCom->Render();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CFriend_Bot::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (m_fHitDelay > 0.5f)
	{
		if (TEXT("Layer_Monster") == strLayerTag)
		{
			CMonster* pMonster = static_cast<CMonster*>(pOther);
			m_tActorDesc.iHp -= static_cast<CMonster*>(pOther)->Get_Damage();

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
			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_Bot_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);

			m_tActorDesc.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
		
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
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();
			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_SECOND_Bot_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
			m_tActorDesc.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
		
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
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();
			m_fHitDelay = 0.f;
		}
		if (TEXT("Layer_THIRD_Bot_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
			m_tActorDesc.iHp -= static_cast<CPlayer_Bullet*>(pOther)->Get_Player_mDamage();
		
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
			static_cast<CPlayer_Bullet*>(pOther)->Set_Dead();
			m_fHitDelay = 0.f;
		}
	}

	return S_OK;
}

HRESULT CFriend_Bot::Ready_Components()
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

HRESULT CFriend_Bot::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 0, 0));

	if (m_fHitDelay <= 0.1f)
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

HRESULT CFriend_Bot::Reset_RenderState()
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

HRESULT CFriend_Bot::Add_Part()
{
	CPart::PART_DESC desc = {};

	desc.pTransformCom = m_pTransformCom;
	desc.pVIBufferCom = m_pVIBufferCom;

	m_pParts[PART_HEAD] = CHead::Create(m_pGraphic_Device, LEVEL_GAMEPLAY, &desc, m_ePlayerID);

	m_pParts[PART_BODY] = CBody::Create(m_pGraphic_Device, LEVEL_GAMEPLAY, &desc, m_ePlayerID);

	m_pParts[PART_WEAPON] = CWeapon::Create(m_pGraphic_Device, LEVEL_GAMEPLAY, &desc, m_ePlayerID);

	return S_OK;
}

_float CFriend_Bot::Calculate_MouseDir()
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

_float CFriend_Bot::Calculate_FireAngle()
{
	_float fResult, fr;
	_float3 fTempDistance;

	_float3 vTargetPos = {};
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CLayer* MonsterLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Monster")));
	CLayer* PlayerLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Player")));
	CLayer* BotLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Bot")));

	if (nullptr == MonsterLayer || nullptr == PlayerLayer || nullptr == BotLayer)
		return false;

	list<CGameObject*> Monster_Obj = MonsterLayer->Get_Objects();
	list<CGameObject*> Player_Obj = PlayerLayer->Get_Objects();
	list<CGameObject*> Bot_Obj = BotLayer->Get_Objects();

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
			if (m_EnemyDistance > fTempDistance && m_fTrunTime >= 0.5f)
			{
				m_bAttack = false;
				m_Follow_Player = true;
				m_vTargetPos = fTempDistance;
				m_EnemyDistance = fTempDistance;
				m_fTrunTime = 0.f;
			}
		}
	}
	for (auto& pDst : Bot_Obj)
	{

		fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
			powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

		fr = powf(Calculate_Range + 1.f, 2);

		if (fResult <= fr)
		{
			m_Detected_Enemy = true;
			m_Follow_Player = true;
			m_Detected_EnemyBot = true;
			//if (static_cast<CPlayer*>(pDst)->m_tActorDesc.iHp <= m_tActorDesc.iHp)
			//{
			//	m_bAttack = true;
			//}
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_EnemyDistance > fTempDistance)
			{
				m_vTargetPos = fTempDistance;
				m_EnemyDistance = fTempDistance;
			}
		}
	}
	_float3 vTargetDir = m_EnemyDistance - vBotPos;

	vTargetDir.y = 1.f;

	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float fDot = D3DXVec3Dot(&vTargetDir, &_float3(1.f, 0.f, 0.f));
	_float fAngle = acosf(fDot);

	if (m_EnemyDistance.z < vBotPos.z)
		fAngle *= -1.f;

	return fAngle;
}

void CFriend_Bot::Set_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case PLAYER_STATE::STATE_IDLE:
			m_pFSM->Change_State(CState_Idle::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, false, LEVEL_GAMEPLAY));
			break;

		case PLAYER_STATE::STATE_MOVE:
			m_pFSM->Change_State(CState_Move::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, false, LEVEL_GAMEPLAY));
			break;

		case PLAYER_STATE::STATE_ATTACK:
			m_pFSM->Change_State(CState_Attack::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, false, LEVEL_GAMEPLAY));
			break;

		case PLAYER_STATE::STATE_MOVEATTACK:
			m_pFSM->Change_State(CState_MoveAttack::Create(m_pParts, PART_END, m_pTransformCom,
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp,
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, false, LEVEL_GAMEPLAY));
			break;

		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}


HRESULT CFriend_Bot::Player_Move(_float fTimeDelta)
{
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	HRESULT hr = E_FAIL;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TileMgr"));
	CTileMgr* pMgr = static_cast<CTileMgr*>(pLayer->Get_Objects().front());

	_float3 vMoveDir = { 0.f, 0.f, 0.f };

	if (m_Follow_Player)
	{
		if (m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).x + 1 >= vBotPos.x)
		{
			vMoveDir.x += 1.f;
		}
		if (m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).x - 1 < vBotPos.x)
		{
			vMoveDir.x -= 1.f;
		}
		if (m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).z + 1 >= vBotPos.z)
		{
			vMoveDir.z += 1.f;
		}
		if (m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).z - 1 < vBotPos.z)
		{
			vMoveDir.z -= 1.f;
		}
	}
	else if (m_Detected_EnemyBot && m_bAttack)
	{
		if (m_EnemyDistance.x + 1 >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x += 1.f;
		}
		if (m_EnemyDistance.x - 1 < m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x -= 1.f;
		}
		if (m_EnemyDistance.z + 1 >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z += 1.f;
		}
		if (m_EnemyDistance.z - 1 < m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z -= 1.f;
		}
	}
	else if (m_Detected_Enemy)
	{
		if (m_EnemyDistance.x >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x -= 1.f;
		}
		if (m_EnemyDistance.x < m_pTransformCom->Get_State(CTransform::STATE_POSITION).x)
		{
			vMoveDir.x += 1.f;
		}
		if (m_EnemyDistance.z >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z -= 1.f;
		}
		if (m_EnemyDistance.z < m_pTransformCom->Get_State(CTransform::STATE_POSITION).z)
		{
			vMoveDir.z += 1.f;
		}
	}

	if (!m_Detected_Enemy && m_fFireDelay > 3)
	{
		m_bAttack = false;
		m_Detected_EnemyBot = false;
		m_Follow_Player = true;
		//vMoveDir = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - vBotPos;
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

CFriend_Bot* CFriend_Bot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFriend_Bot* pInstance = new CFriend_Bot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFriend_Bot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CFriend_Bot::Clone(void* pArg)
{
	CFriend_Bot* pInstance = new CFriend_Bot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFriend_Bot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFriend_Bot::Free()
{
	__super::Free();

	for (size_t i = 0; i < PART_END; ++i)
		Safe_Release(m_pParts[i]);

	Safe_Release(m_pFSM);
	Safe_Release(m_pColliderCom);
}

