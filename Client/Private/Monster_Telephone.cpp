#include "stdafx.h"
#include "..\Public\Monster_Telephone.h"

#include "Player_Bullet.h"
#include "Item.h"
#include "Damage.h"
#include "Item_Object.h"
#include "Effect_LandMine_Explosion.h"

CMonster_Telephone::CMonster_Telephone(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Telephone::CMonster_Telephone(const CMonster_Telephone& Prototype)
	:CMonster{ Prototype }
{
}


HRESULT CMonster_Telephone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Telephone::Initialize(void* pArg)
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

	m_tActorDesc.iAttack = 20;
	m_tActorDesc.iMaxHp = 600;
	m_tActorDesc.iHp = 600;
	m_pAnimatorCom->Change_Texture(TEXT("Monster_Telephone_Idle"));
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_eAttack_State = Telephone_Idle;
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, tMonsterDesc.m_Pos);
	m_pTransformCom->Scaling(3.f, 3.f, 3.f);
	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);
	return S_OK;
}

_uint CMonster_Telephone::Priority_Update(_float fTimeDelta)
{
	CItem::ITEM_DESC tItem_DESC;

	if (m_bDeadMotion && (m_iFrameCounter > 0))
	{
		tItem_DESC.vPos = m_tActorDesc.m_Pos;
		tItem_DESC.eItemType = CItem::ITEM_EXP;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Exp"), &tItem_DESC)))
			return E_FAIL;

		CItem_Object::ITEM_DESC desc = {};

		desc.pTerrainTransform = m_pTerrainTransform;
		desc.pTerrainVIBuffer = m_pTerrainVIBuffer;
		desc.eItemType = CItem_Object::ITEM_MINE;
		desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
			return E_FAIL;

		return OBJ_DEAD;
	}

	if (m_bdead)
	{
		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"), &m_tActorDesc);
		m_tActorDesc.iHp -= m_iGet_dmg;
		m_bdead = false;
	}


	return OBJ_NOEVENT;
}

void CMonster_Telephone::Update(_float fTimeDelta)
{
	if (m_bFire)
	{
		m_fFireTime += fTimeDelta;
		if (m_fFireTime >= 1.f)
		{
			m_bFire = false;
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_Telephone_Meteor"), &m_tActorDesc);
			m_fFireTime = 0.f;
		}
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
	m_fHittimer += fTimeDelta;


	m_fTimer += fTimeDelta;
	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_int Rand = rand() % 2;


	if (fTimeDelta * 60 >= 1)
	{
		m_fAnim_Time++;
	}

	if (m_tActorDesc.iHp <= 0 && !m_bDeadMotion)
	{
		m_tActorDesc.iHp = 0;
		m_bAttack = false;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Telephone_Die"));
		m_eAttack_State = Telephone_Attack_DIE;
		m_bDeadMotion = true;
		m_iFrameCounter = 0;
	}
	else if ((m_eAttack_State == Telephone_Idle) && (m_tActorDesc.iMaxHp != m_tActorDesc.iHp))
	{
		m_eAttack_State = Telephone_Attack;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Telephone_Active"));
		m_fAnim_Time = 0;
	}
	else if (m_eAttack_State == Telephone_Attack && (m_iFrameCounter >= 2))
	{

		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
		m_pGameInstance->SoundPlay(L"sfx_EliteStart_Phone.wav", _uint(CHANNELID::SOUND_BOSSFIGHTSTART), .5f);
		// 플레이어 멀어지면 소리 안남
		_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
			PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
		{
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
		}

		m_bAttack = true;
		if (rand() % 2 == 1)
			m_eAttack_State = Telephone_Attack_LandMine;
		else
			m_eAttack_State = Telephone_Attack_Drop_Telephone;
	}
	_float3 TempPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	if (m_eAttack_State == Telephone_Attack_LandMine && m_bAttack)
	{
		for (int i = 0; i < 5; i++)
		{
			if (rand() % 2)
				m_tActorDesc.m_Pos = { TempPos.x + rand() % 5,TempPos.y,TempPos.z + rand() % 5 };
			else
				m_tActorDesc.m_Pos = { TempPos.x - rand() % 5,TempPos.y,TempPos.z - rand() % 5 };


			CEffect_LandMine_Explosion::EXPLOSION_DESC desc;
			desc.m_Pos = m_tActorDesc.m_Pos;
			desc.fScale = 4.f;

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_LandMine_Count_Effect"), &desc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_LandMine_Max_Effect"), &desc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_LandMine"), &m_tActorDesc);

			v_pos.push_back(m_tActorDesc.m_Pos);
		}
		m_eAttack_State = Telephone_Attack;
		m_iFrameCounter = 0;
		m_bAttack = false;
	}
	else if (m_eAttack_State == Telephone_Attack_Drop_Telephone && m_bAttack)
	{
		m_pPlayerTransform = dynamic_cast<CTransform*>
			(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
		m_tActorDesc.m_targetPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (m_tActorDesc.m_targetPos.x < (m_tActorDesc.m_Pos.x + 5) && m_tActorDesc.m_targetPos.x >(m_tActorDesc.m_Pos.x - 5) &&
			m_tActorDesc.m_targetPos.z < (m_tActorDesc.m_Pos.z + 5) && m_tActorDesc.m_targetPos.z >(m_tActorDesc.m_Pos.z - 5))
		{
			//v_pos.push_back(m_tActorDesc.m_targetPos);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Telephone_Count_Effect"), &m_tActorDesc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Telephone_Max_Effect"), &m_tActorDesc);
			m_bFire = true;
		}
		m_eAttack_State = Telephone_Attack;
		m_iFrameCounter = 0;
		m_bAttack = false;
	}


	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);

}

void CMonster_Telephone::Late_Update(_float fTimeDelta)
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

HRESULT CMonster_Telephone::Render()
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

HRESULT CMonster_Telephone::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{

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
		m_bdead = true;
	}
	return S_OK;
}

HRESULT CMonster_Telephone::Ready_Components()
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Telephone_Idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_Idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Telephone_Active"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_Active"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Telephone_End"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_End"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Telephone_Die"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Telephone_Die"))));

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

HRESULT CMonster_Telephone::SetUp_RenderState()
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

HRESULT CMonster_Telephone::Reset_RenderState()
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


CMonster_Telephone* CMonster_Telephone::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CMonster_Telephone* pInstance = new CMonster_Telephone(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Telephone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Telephone::Clone(void* pArg)
{
	CMonster_Telephone* pInstance = new CMonster_Telephone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Telephone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Telephone::Free()
{
	__super::Free();


}
