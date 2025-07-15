#include "stdafx.h"
#include "..\Public\Monster_Police.h"

#include "GameInstance.h"
#include "Player_Bullet.h"
#include "Monster_Police_Gun.h"

#include "Layer.h"
#include "TileMgr.h"
#include "Item.h"
#include "Damage.h"
#include "Item_Object.h"

CMonster_Police::CMonster_Police(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Police::CMonster_Police(const CMonster_Police& Prototype)
	:CMonster{ Prototype }
{
}


HRESULT CMonster_Police::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Police::Initialize(void* pArg)
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

	m_iFrameCounter = 0;
	m_tActorDesc.iAttack = 30;
	m_tActorDesc.iMaxHp = 500;
	m_tActorDesc.iHp = 500;
	m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Idle"));
	m_iSpeed = 1.f;
	m_eAttack_State = POLICE_PEACE;

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(tMonsterDesc.m_Pos.x, 1.f, tMonsterDesc.m_Pos.z));

	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//m_ptBody_Info.fFrist_Spint_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//m_ptBody_Info.fGun_Pos = _float3(m_tActorDesc.m_Pos.x, m_tActorDesc.m_Pos.y + 1, m_tActorDesc.m_Pos.z);
	
	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);
	
	
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Police_Gun_Part"), TEXT("Prototype_GameObject_Monster_Police_Gun"), &m_tActorDesc);

	if (m_pPolice_Part == nullptr)
	{
		CLayer* m_pPolice_Gun = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Monster_Police_Gun_Part")));
		
		if (nullptr == m_pPolice_Gun)
			return E_FAIL;

		CGameObject* Part_Obj = m_pPolice_Gun->Get_Objects().back();

		static_cast<CMonster_Police_Gun*>(Part_Obj)->Set_pMonster(this);
	}

	return S_OK;
}

_uint CMonster_Police::Priority_Update(_float fTimeDelta)
{
	CItem::ITEM_DESC tItem_DESC = {};

	if (m_bDeadMotion && (m_iFrameCounter > 0))
	{
		tItem_DESC.vPos = m_tActorDesc.m_Pos;
		tItem_DESC.eItemType = CItem::ITEM_EXP;

		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if(m_bDropExp)
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Exp"), &tItem_DESC);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Burn_Smoke"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Police_Die"), &m_tActorDesc);


		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_EXPLOSIVE), .8f);

		_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
			PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
		{
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		}


		CItem_Object::ITEM_DESC desc = {};

		desc.pTerrainTransform = m_pTerrainTransform;
		desc.pTerrainVIBuffer = m_pTerrainVIBuffer;
		desc.eItemType = CItem_Object::ITEM_MISSILE;
		desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
			return E_FAIL;
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
		// 경험치

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Die_Dust_Effect"), &m_tActorDesc);

		m_tActorDesc.iHp -= m_iGet_dmg;
		m_bdead = false;
	}


	return OBJ_NOEVENT;
}

void CMonster_Police::Update(_float fTimeDelta)
{
	if (m_fHitEffect_Time >= 0.1f)
	{
		m_fHitEffect_Time = 0;
		m_bHitEffect = false;
	}
	if (m_bHitEffect)
	{
		m_fHitEffect_Time += fTimeDelta;
	}
	m_fHittimer += fTimeDelta;
	m_fTimer += fTimeDelta;
	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_eAttack_State == POLICE_PEACE && m_tActorDesc.iMaxHp != m_tActorDesc.iHp)
	{
		m_eAttack_State = POLICE_IDLE;
		m_iFrameCounter = 0;
	}
	if (m_eAttack_State == POLICE_DIE && m_iFrameCounter > 0)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Die"));
		m_iFrameCounter = 0;

	}
	if (m_eAttack_State == POLICE_IDLE && m_iFrameCounter > 0)
	{
		m_eAttack_State = POLICE_MOVE;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Move"));
		m_iFrameCounter = 0;
	}
	else if (m_eAttack_State == POLICE_MOVE && m_iFrameCounter > 0)
	{
		m_eAttack_State = POLICE_SHOT_READY;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Shot"));
		m_iFrameCounter = 0;
	}
	else if (m_eAttack_State == POLICE_SHOT_READY && m_iFrameCounter > 0)
	{
		m_eAttack_State = POLICE_AIM;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Aim"));
		m_iFrameCounter = 0;
	}
	else if (m_eAttack_State == POLICE_AIM && m_iFrameCounter > 5)
	{
		m_eAttack_State = POLICE_SHOT_RETURN;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Shot_Back"));
		m_iFrameCounter = 0;
	}
	else if (m_eAttack_State == POLICE_SHOT_RETURN && m_iFrameCounter > 0)
	{
		m_eAttack_State = POLICE_IDLE;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Idle"));
		m_iFrameCounter = 0;
	}
	

	if (m_tActorDesc.iHp <= 0 && (m_bDeadMotion == false))
	{
		m_tActorDesc.iHp = 0;

		m_eAttack_State = POLICE_DIE;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Die"));
		m_bDeadMotion = true;
		m_iFrameCounter = 0;
	}


	if (m_eAttack_State == POLICE_MOVE)
	{
		_float3 PlayerMatrix;
		_float3 MonsterMatrix;
		PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		D3DXVECTOR3 dir = MonsterMatrix - PlayerMatrix;
		D3DXVec3Normalize(&dir, &dir);
		MonsterMatrix = MonsterMatrix + dir * fTimeDelta * m_iSpeed;



		_float3 vMoveDir = { 0.f, 0.f, 0.f };

		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TileMgr"));
		CTileMgr* pMgr = static_cast<CTileMgr*>(pLayer->Get_Objects().front());

		if ((S_OK == pMgr->Check_MoveTile(MonsterMatrix)))
		{
			if (!m_bDeadMotion)
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, MonsterMatrix);
		}
	}
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CMonster_Police::Late_Update(_float fTimeDelta)
{

	if (m_Monster_Statue == MONSTER_STATE_SLOW)
	{
		if (m_fHittimer >= 1.f)
		{

			m_Monster_Statue = MONSTER_STATE_DEFAULT;
			m_fHittimer = 0;
			m_tActorDesc.iHp -= 10;
			CDamage::DAMAGE_DESC desc = {};
			desc.iDamage = 10;
			desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
				TEXT("Prototype_GameObject_Damage"), &desc);
		}
	}/*
	else if (m_Monster_Statue == MONSTER_STATE_STUN)
	{
		m_eAttack_State = POLICE_IDLE;
		m_iFrameCounter = 0;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Police_Idle"));
		m_fAnim_Time += fTimeDelta;
		if (m_fAnim_Time >= 3.f)
		{
			m_Monster_Statue = MONSTER_STATE_DEFAULT;
			m_fAnim_Time = 0;
		}
	}*/

	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
		m_iFrameCounter++;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster_Police::Render()
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

HRESULT CMonster_Police::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	_float3 _fpOther_Pos, _fmyPos;
	if (strLayerTag == TEXT("Layer_Monster"))
	{
		_fpOther_Pos = static_cast<CActor*>(pOther)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_fmyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (_fmyPos.x < _fpOther_Pos.x)
			m_pTransformCom->Go_Left(m_dTimeTemp * 0.5f);
		else if (_fmyPos.x >= _fpOther_Pos.x)
			m_pTransformCom->Go_Right(m_dTimeTemp * 0.5f);

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
	}
	if (strLayerTag == TEXT("Layer_Bot_Bullet"))
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

HRESULT CMonster_Police::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Die"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Die"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Move"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Move"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Shot_Back"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Shot_Back"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Shot"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Shot"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Police_Aim"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Police_Aim"))));

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

HRESULT CMonster_Police::SetUp_RenderState()
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

HRESULT CMonster_Police::Reset_RenderState()
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

CMonster_Police* CMonster_Police::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CMonster_Police* pInstance = new CMonster_Police(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Police"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Police::Clone(void* pArg)
{
	CMonster_Police* pInstance = new CMonster_Police(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Police"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Police::Free()
{
	__super::Free();


}
