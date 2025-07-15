#include "stdafx.h"
#include "..\Public\Pierrot.h"

#include "Player_Bullet.h"
#include "Effect_Shadow.h"
#include "Layer.h"
#include "Damage.h"
#include "Item_Object.h"
#include "Player.h"

CPierrot::CPierrot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
	, m_pPlayerTransform { nullptr }
{
}

CPierrot::CPierrot(const CPierrot& Prototype)
	: CMonster{ Prototype }
{
}


HRESULT CPierrot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPierrot::Initialize(void* pArg)
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

	// 플레이어의 주소를 맴버 변수로 가지고 있으면 매번 호출해서 받아올 필요가 없을듯
	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_tActorDesc.iHp = 1700;
	m_tActorDesc.iMaxHp = 1700;
	m_tActorDesc.iAttack = 10;

	m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Ghost_Idle"));
	m_pState = PIERROT_GHOSTIDLE;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, tMonsterDesc.m_Pos);
	m_pTransformCom->Scaling(10.f, 10.f, 10.f);

	m_pColliderCubeCom->Scaling(2.f, 3.f, 2.f);
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Shadow"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);

	if (m_pShadow == nullptr)
	{
		CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

		if (nullptr == Shadow)
			return E_FAIL;

		CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
	}

	return S_OK;
}

_uint CPierrot::Priority_Update(_float fTimeDelta)
{

	if (m_bDeadMotion)
	{
		m_fDieTimer += fTimeDelta;

		if(m_fDieTimer <= 10)
		{
			CItem_Object::ITEM_DESC desc = {};

			desc.pTerrainTransform = m_pTerrainTransform;
			desc.pTerrainVIBuffer = m_pTerrainVIBuffer;
			desc.eItemType = CItem_Object::ITEM_METEOR;
			desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
				return E_FAIL;
			return OBJ_DEAD;
		}
	}
	
	if (m_bdead)
	{
		m_bHitEffect = true;
		if (m_bHitEffect)
		{
			m_fHitEffect_Time = 0;
		}
		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_tActorDesc.m_Pos.z += 0.1f;
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Die_Dust_Effect"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Blast"), &m_tActorDesc);
		m_tActorDesc.iHp -= m_iGet_dmg;
		m_bdead = false;
	}

	return OBJ_NOEVENT;
}

void CPierrot::Update(_float fTimeDelta)
{

	m_Calculate_Range += 0.5f;
	if (m_Calculate_Range >= 30.f)
		m_Calculate_Range = 10.f;

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

	m_tActorDesc.fAngle += 1.f;
	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (fTimeDelta >= 1 / 60)
	{
		m_fAnim_Time++;
		if (m_fAnim_Time > 120)
			m_bWake = true;
	}


	if (m_tActorDesc.iHp <= 0 && !m_bDeadMotion)
	{
		m_tActorDesc.iHp = 0;

		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Idle"));
		m_pState = PIERROT_DIE;
		m_bDeadMotion = true;
		m_iFrameCounter = 0;
	}
	else if (m_pState == PIERROT_GHOSTIDLE && (m_tActorDesc.iMaxHp != m_tActorDesc.iHp))
	{
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
		m_pGameInstance->SoundPlay(L"Boss_Pierrot_Transform.wav", _uint(CHANNELID::SOUND_BOSSFIGHTSTART), 1.f);
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Transform"));
		m_pState = PIERROT_TRANSFORM;
		m_bWake = false;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_TRANSFORM && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Idle"));
		m_pState = PIERROT_IDLE;
		m_fAnim_Time = 0;
		m_iFrameCounter = 0;
	}
	else if (m_pState == PIERROT_IDLE && (m_iFrameCounter > 0))
	{
		int Temp = rand() % 4;
		m_iFrameCounter = 0;
		if (Temp == 0)
		{
			m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Ready"));
			m_fAnim_Time = 0;
			m_pState = PIERROT_CARDREADY;
		}
		else if(Temp == 1)
		{
			m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Paperplane_ready"));
			m_fAnim_Time = 0;
			m_pState = PIERROT_PAPERPLANE_READY;
		}
		else if(Temp == 2)
		{
			m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Juggling_Ready"));
			m_fAnim_Time = 0;
			m_pState = PIERROT_JUGGLING_READY;
		}
		else if (Temp == 3)
		{
			m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Jump_Start"));
			m_fAnim_Time = 0;
			m_pState = PIERROT_JUMP_START;
		}

	}
	else if (m_pState == PIERROT_CARDREADY && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_shuffle"));
		m_pState = PIERROT_CARDSHFFLE;
		m_fAnim_Time = 0;
		m_iFrameCounter = 0;
	}
	else if (m_pState == PIERROT_CARDSHFFLE && (m_iFrameCounter > 0))
	{
		m_Bullet = PIERROT_BULLET_Card;
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Throw"));
		m_pState = PIERROT_CARDTHROW;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_CARDTHROW && (m_iFrameCounter > 4))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_End"));
		m_pState = PIERROT_CARDEND;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_CARDEND && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Idle"));
		m_pState = PIERROT_IDLE;
		//	m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}// Paper

	else if (m_pState == PIERROT_PAPERPLANE_READY && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Paperplane_rest"));
		m_pState = PIERROT_PAPERPLANE_REST;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_PAPERPLANE_REST && (m_iFrameCounter > 0)) // ready
	{
		m_Bullet = PIERROT_BULLET_Paper;
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Paperplane_summon"));
		m_pState = PIERROT_PAPERPLANE_SUMMON;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_PAPERPLANE_SUMMON && (m_iFrameCounter > 4)) // 390 
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Paperplane_end"));
		m_pState = PIERROT_PAPERPLANE_END;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_PAPERPLANE_END && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Idle"));
		m_pState = PIERROT_IDLE;
		//m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}

	else if (m_pState == PIERROT_JUGGLING_READY && (m_iFrameCounter > 0)) // ready
	{
		m_Bullet = PIERROT_BULLET_Ball;
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Juggling_Juggling"));
		m_pState = PIERROT_JUGGLING_JUGGLING;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
		}
	else if (m_pState == PIERROT_JUGGLING_JUGGLING && (m_iFrameCounter > 5)) // 390 
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Juggling_End"));
		m_pState = PIERROT_PAPERPLANE_END;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}
	else if (m_pState == PIERROT_PAPERPLANE_END && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Idle"));
		m_pState = PIERROT_IDLE;
		//m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}

	else if (m_pState == PIERROT_JUMP_START && (m_iFrameCounter > 0)) // 390 
	{
		m_bGround = false;

		if (m_iFrameCounter > 1 && m_iFrameCounter < 2)
		{
			Calculate_Target();

			m_tActorDesc.m_targetPos = m_vAttackPos;
		}
		if (m_iFrameCounter <= 2 && !m_bLandingReady)
		{
			Calculate_Target();
			m_tActorDesc.m_Pos = m_vAttackPos;
			m_tActorDesc.m_Pos.y = m_vAttackPos.y =  1.5f;
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Big_Shadow"), &m_tActorDesc);
			m_bLandingReady = true;
		}
		if (m_iFrameCounter > 4)
		{
			m_pColliderCubeCom->Scaling(0.1f, 0.1f, 0.1f);

			m_tActorDesc.m_Pos = m_vAttackPos;
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Vertical_Smoke"), &m_tActorDesc);
			m_bLandingReady = false;
			m_bGround = true;

			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
			m_pGameInstance->SoundPlay(L"Drop_SmallExplosion.wav", _uint(CHANNELID::SOUND_BOSSFIGHTSTART), 1.f);

			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tActorDesc.m_Pos);
			m_pAnimatorCom->Change_Texture(TEXT("Pierrot_Jump_Land"));
			m_pState = PIERROT_JUMP_LAND;
			m_iFrameCounter = 0;
			m_fAnim_Time = 0;
		}
	}
	else if (m_pState == PIERROT_JUMP_LAND && (m_iFrameCounter > 0)) // 390 
	{

		m_pColliderCubeCom->Scaling(2.f, 3.f, 2.f);
		m_bGround = true;
		m_pAnimatorCom->Change_Texture(TEXT("Pierrot_idle"));
		m_pState = PIERROT_IDLE;
		m_iFrameCounter = 0;
		m_fAnim_Time = 0;
	}

	if (m_pState == PIERROT_CARDTHROW || m_pState == PIERROT_PAPERPLANE_SUMMON || m_pState == PIERROT_JUGGLING_JUGGLING)
	{
		Fire_Bullet(fTimeDelta, m_Bullet);
	}
	
	if (!m_bGround)
	{
		_float3 vPierrotPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPierrotPos.y += 10.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPierrotPos);
	}
	if(m_bGround)
	{

		__super::SetUp_OnTerrain(m_pTransformCom, 1.5f);
	}

}

void CPierrot::Late_Update(_float fTimeDelta)
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

HRESULT CPierrot::Render()
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

HRESULT CPierrot::Ready_Components() 
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Ghost_Idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Ghost_idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Transform"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Transform"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_End"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_End"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Ready"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Ready"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_shuffle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Shuffle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Throw"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Throw"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Paperplane_end"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_end"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Paperplane_ready"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_ready"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Paperplane_rest"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_rest"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Paperplane_summon"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_paperPlaneMagic_summon"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Juggling_End"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Juggling_End"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Juggling_Juggling"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Juggling_Juggling"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Juggling_Ready"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Juggling_Ready"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Jump_Start"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Jump_Start"))));

	m_pAnimatorCom->Add_Texture(TEXT("Pierrot_Jump_Land"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pierrot_Jump_End"))));


	
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

HRESULT CPierrot::SetUp_RenderState()
{
	//LPDIRECT3DTEXTURE9 Test_Alpha_Texture = NULL;

	//D3DXCreateTextureFromFile(m_pGraphic_Device, TEXT("../Resources/Texture2D/Monster/Pierrot/idle_Alpha/alpha_Boss_Test_0.png"), &Test_Alpha_Texture);

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

HRESULT CPierrot::Reset_RenderState()
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

HRESULT CPierrot::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player_Bullet"))
	{
		//cout << " 충돌" << endl;
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

HRESULT CPierrot::Calculate_Target()
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

		fr = powf(m_Calculate_Range + 1.f, 2);

		if (fResult <= fr)
		{
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_vEnemyDistance > fTempDistance)
			{
				m_vAttackPos = fTempDistance;
			}
		}
	}
	for (auto& pDst : Bot_Obj)
	{
		fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
			powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

		fr = powf(m_Calculate_Range + 1.f, 2);

		if (fResult <= fr)
		{
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_vEnemyDistance > fTempDistance)
			{
				m_vAttackPos = fTempDistance;
			}
		}
	}
	for (auto& pDst : BotFriEnd_Obj)
	{
		fResult = powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
			powf(m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION).z, 2);

		fr = powf(m_Calculate_Range + 1.f, 2);

		if (fResult <= fr)
		{
			fTempDistance = static_cast<CPlayer*>(pDst)->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//플레이어
			if (m_vEnemyDistance > fTempDistance)
			{
				m_vAttackPos = fTempDistance;
			}
		}
	}
	return S_OK;
}

HRESULT CPierrot::Fire_Bullet(_float fTimeDelta, PIERROT_BULLET _m_Bullet)
{
	// 추천합니다.
	if (60 * fTimeDelta >= 1)
	{
		m_FireTime++;
		if(_m_Bullet == PIERROT_BULLET_Card)
			if (m_FireTime >= 10)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
				m_pGameInstance->SoundPlay(L"Boss_Pierrot_ThrowCard.wav", _uint(CHANNELID::SOUND_BOSSFIGHTSTART), 1.f);

				_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
					PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
				{
					m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
				}

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_Boss_Bullet"), &m_tActorDesc)))
					return E_FAIL;

				m_FireTime = 0;
			}
		if (_m_Bullet == PIERROT_BULLET_Paper)
			if (m_FireTime >= 40)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
				m_pGameInstance->SoundPlay(L"Boss_Pierrot_MagicPop.wav", _uint(CHANNELID::SOUND_BOSSFIGHTSTART), 1.f);

				_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
					PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
				{
					m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
				}

				_float Tempangle = Fire_BulletPlayerAngle();
				m_tActorDesc.fAngle = Tempangle;

				m_tActorDesc.m_targetPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				m_tActorDesc.m_Pos.y += 0.7f;
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_Bird_Bullet"), &m_tActorDesc)))
					return E_FAIL;
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Boss_Arrow"), &m_tActorDesc)))
					return E_FAIL;
				m_FireTime = 0;
			}
		if (_m_Bullet == PIERROT_BULLET_Ball)
			if (m_FireTime >= 40)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
				m_pGameInstance->SoundPlay(L"Boss_Pierrot_MagicPop.wav", _uint(CHANNELID::SOUND_BOSSFIGHTSTART), 1.f);

				_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
					PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
				{
					m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_BOSSFIGHTSTART));
				}

				_float Tempangle = Fire_BulletPlayerAngle();
				m_tActorDesc.fAngle = Tempangle;
				m_tActorDesc.m_targetPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

				m_tActorDesc.m_Pos.y += 1.f;
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_Boss_Ball"), &m_tActorDesc)))
					return E_FAIL;
				m_FireTime = 0;
			}

	}
	return S_OK;
}

_float CPierrot::Fire_BulletPlayerAngle()
{
	Calculate_Target();

	m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float v1 = (_float)sqrt(pow(PlayerMatrix.x, 2) + (_float)pow(PlayerMatrix.z, 2));
	_float v2 = (_float)sqrt(pow(MonsterMatrix.x, 2) + (_float)pow(MonsterMatrix.z, 2));
	_float inner = PlayerMatrix.x * MonsterMatrix.x + PlayerMatrix.z * MonsterMatrix.z;
	_float Theta = acos(inner / (v1 * v2));
	MonsterMatrix = PlayerMatrix - m_vAttackPos;

	D3DXVec3Normalize(&MonsterMatrix, &MonsterMatrix);
	//return (PlayerMatrix.x * MonsterMatrix.x) + (PlayerMatrix.z * MonsterMatrix.z);
	//return (PlayerMatrix.x * MonsterMatrix.x) + (PlayerMatrix.z * MonsterMatrix.z);
	return Theta;
}


CPierrot* CPierrot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CPierrot* pInstance = new CPierrot(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPierrot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CPierrot::Clone(void* pArg)
{

	CPierrot* pInstance = new CPierrot(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPierrot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPierrot::Free()
{
	__super::Free();
}
