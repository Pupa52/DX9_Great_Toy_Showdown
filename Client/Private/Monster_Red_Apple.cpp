#include "stdafx.h"
#include "..\Public\Monster_Red_Apple.h"

#include "Player_Bullet.h"
#include "Effect_Shadow.h"
#include "Layer.h"
#include "TileMgr.h"
#include "Item.h"
#include "Damage.h"

#include "Player.h"
#include "Bot.h"

CMonster_Red_Apple::CMonster_Red_Apple(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Red_Apple::CMonster_Red_Apple(const CMonster_Red_Apple& Prototype)
	:CMonster{ Prototype }
{
}


HRESULT CMonster_Red_Apple::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Red_Apple::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	// 0624 심현우
	MONSTER_DESC tMonsterDesc = *static_cast<MONSTER_DESC*>(pArg);
	m_pTerrainVIBuffer = tMonsterDesc.pTerrainVIBuffer;
	Safe_AddRef(m_pTerrainVIBuffer);
	m_pTerrainTransform = tMonsterDesc.pTerrainTransform;
	Safe_AddRef(m_pTerrainTransform);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_fAnim_Time = 0.f;
	m_tActorDesc.iAttack = 5;
	m_tActorDesc.iHp = 15;
	m_tActorDesc.iMaxHp = 15;
	m_pAnimatorCom->Change_Texture(TEXT("Monster_Red_Apple_Move"));
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Shadow"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);

	if (m_pShadow == nullptr)
	{
		CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

		if (nullptr == Shadow)
			return E_FAIL;

		CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, tMonsterDesc.m_targetPos);
	
	m_pTransformCom->Scaling(4.f, 4.f, 4.f);
	m_pColliderCubeCom->Scaling(0.3f, 2.f, 0.3f);
	return S_OK;
}

_uint CMonster_Red_Apple::Priority_Update(_float fTimeDelta)
{
	CItem::ITEM_DESC tItem_DESC = {};

	if (m_bDeadMotion && (m_iFrameCounter > 0))
	{

		tItem_DESC.vPos = m_tActorDesc.m_Pos;
		tItem_DESC.eItemType = CItem::ITEM_EXP;

		for (int i = 0; i < 8; i++)
		{
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Red_Apple_Die"), &m_tActorDesc);
		}


		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_EXPLOSIVE), .8f);

		_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
			PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
		{
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		}

		_int RandHealkit = rand() % 15;
		if (!RandHealkit)
		{
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Healkit"), &tItem_DESC);
		}

		if(m_bDropExp)
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Exp"), &tItem_DESC);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Burn_Smoke"), &m_tActorDesc);
		return OBJ_DEAD;
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
		m_tActorDesc.iHp -= m_iGet_dmg;
		m_bdead = false;
	}

	return OBJ_NOEVENT;
}

void CMonster_Red_Apple::Update(_float fTimeDelta)
{

	Calculate_Range += 0.05f;
	if (Calculate_Range >= 10.f)
		Calculate_Range = 1.f;


	m_dTimeTemp = fTimeDelta;
	m_fHittimer += fTimeDelta;


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
	if (m_Monster_Statue == MONSTER_STATE_STUN)
	{
		m_fAnim_Time += fTimeDelta;
		if (m_fAnim_Time > 3.f)
		{
			m_fAnim_Time = 0;
		}
	}

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
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Red_Apple_Die"));
		m_bDeadMotion = true;
		m_iFrameCounter = 0;
	}

	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Find_Enemy();

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVECTOR3 dir = MonsterMatrix - m_fEnemyDistance;
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

	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CMonster_Red_Apple::Late_Update(_float fTimeDelta)
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

HRESULT CMonster_Red_Apple::Render()
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

HRESULT CMonster_Red_Apple::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	_float3 _fpOther_Pos, _fmyPos;
	if (strLayerTag == TEXT("Layer_Monster"))
	{
		_fpOther_Pos = static_cast<CActor*>(pOther)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_fmyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (_fmyPos.x < _fpOther_Pos.x)
		{
			m_pTransformCom->Go_Left(m_dTimeTemp * 0.5f);
		}
		else if (_fmyPos.x >= _fpOther_Pos.x)
		{
			m_pTransformCom->Go_Right(m_dTimeTemp * 0.5f);
		}

		if (_fmyPos.z < _fpOther_Pos.z)
			m_pTransformCom->Go_Backward(m_dTimeTemp * 0.5f);
		else if (_fmyPos.z >= _fpOther_Pos.z)
			m_pTransformCom->Go_Straight(m_dTimeTemp * 0.5f);
	}
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
		
		m_bDropExp = true;
		m_bdead = true;
	}if (strLayerTag == TEXT("Layer_Bot_Bullet"))
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
	
		m_bDropExp = false;
		m_bdead = true;
	}
	if (strLayerTag == TEXT("Layer_SECOND_Bot_Bullet"))
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
		m_bDropExp = false;
		m_bdead = true;
	}
	if (strLayerTag == TEXT("Layer_THIRD_Bot_Bullet"))
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
		m_bDropExp = false;
		m_bdead = true;
	}
	if (strLayerTag == TEXT("Layer_Friend_Bullet"))
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
		m_bDropExp = false;
		m_bdead = true;
	}


	return S_OK;
}

HRESULT CMonster_Red_Apple::Find_Enemy()
{
	_float fResult, fr;
	_float3 fTempDistance;

	_float3 vTargetPos = {};
	_float3 vBotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CLayer* PlayerLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Player")));
	CLayer* BotLayer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Bot")));
	CLayer* Bot_FriEnd_Layer = m_pGameInstance->Get_Instance()->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Friend_Bot")));

	if (nullptr == PlayerLayer || nullptr == Bot_FriEnd_Layer || nullptr == BotLayer)
		return E_FAIL;

	list<CGameObject*> Player_Obj = PlayerLayer->Get_Objects();
	list<CGameObject*> Bot_Obj = BotLayer->Get_Objects();
	list<CGameObject*> BotFriEnd_Obj = Bot_FriEnd_Layer->Get_Objects();

	for (auto& pDst : Player_Obj)
	{

		fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
			powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

		fr = powf(Calculate_Range + 1.f, 2);

		if (fResult <= fr)
		{
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_fEnemyDistance > fTempDistance)
			{
				m_vTargetPos = fTempDistance;
				m_fEnemyDistance = fTempDistance;
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
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_fEnemyDistance > fTempDistance)
			{
				m_vTargetPos = fTempDistance;
				m_fEnemyDistance = fTempDistance;
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
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_fEnemyDistance > fTempDistance)
			{
				m_vTargetPos = fTempDistance;
				m_fEnemyDistance = fTempDistance;
			}
		}
	}
	return S_OK;
}

HRESULT CMonster_Red_Apple::Ready_Components() 
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Red_Apple_Move"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Red_Apple_Move"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Red_Apple_Die"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Red_Apple_Die"))));

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

HRESULT CMonster_Red_Apple::SetUp_RenderState()
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

HRESULT CMonster_Red_Apple::Reset_RenderState()
{
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	// arg2 는 상수컬러

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);



	return S_OK;
}


CMonster_Red_Apple* CMonster_Red_Apple::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CMonster_Red_Apple* pInstance = new CMonster_Red_Apple(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Red_Apple"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Red_Apple::Clone(void* pArg)
{
	CMonster_Red_Apple* pInstance = new CMonster_Red_Apple(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Red_Apple"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Red_Apple::Free()
{
	__super::Free();


}
