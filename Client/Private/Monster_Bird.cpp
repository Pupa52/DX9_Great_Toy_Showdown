#include "stdafx.h"
#include "..\Public\Monster_Bird.h"

#include "Player_Bullet.h"
#include "Effect_Shadow.h"
#include "Layer.h"
#include "Damage.h"
#include "Item_Object.h"

CMonster_Bird::CMonster_Bird(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{
}

CMonster_Bird::CMonster_Bird(const CMonster_Bird& Prototype)
	:CMonster{ Prototype }
{
}

HRESULT CMonster_Bird::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Bird::Initialize(void* pArg)
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

	m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Idle"));
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
	m_eBird_State = BIRD_PEACE;

	m_tActorDesc.iMaxHp = 600;
	m_tActorDesc.iHp = 600;
	m_bdead = false;
	m_tActorDesc.iAttack = 20;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, tMonsterDesc.m_Pos);
	Origin_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Scaling(2.f, 2.f, 2.f);

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Shadow"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);

	if (m_pShadow == nullptr)
	{
		CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

		if (nullptr == Shadow)
			return E_FAIL;

		CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

		static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
	}

	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);

	return S_OK;
}

_uint CMonster_Bird::Priority_Update(_float fTimeDelta)
{
	if (m_eBird_State == BIRD_DIE && (m_iFrameCounter > 0))
	{
		for (int i = 0; i < 32; i++)
		{
			m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Smoke_Effect"), &m_tActorDesc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Feather_Effect"), &m_tActorDesc);
		}
		CItem_Object::ITEM_DESC desc = {};

		desc.pTerrainTransform = m_pTerrainTransform;
		desc.pTerrainVIBuffer = m_pTerrainVIBuffer;

		desc.eItemType = CItem_Object::ITEM_TELEPORT;
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
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Die_Dust_Effect"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"), &m_tActorDesc);
		m_tActorDesc.iHp -= m_iGet_dmg;
		m_bdead = false;
	}


	return OBJ_NOEVENT;
}

void CMonster_Bird::Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);
	m_fHittimer += fTimeDelta;

	if (m_fHitEffect_Time >= 0.1f)
	{
		m_fHitEffect_Time = 0;
		m_bHitEffect = false;
	}
	if (m_bHitEffect)
	{
		m_fHitEffect_Time += fTimeDelta;
	}


	if (m_bStuned)
	{
		m_fAnim_Time += fTimeDelta;

		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Idle"));
		m_eBird_State = BIRD_IDLE;
		m_vTelePort_Pos.clear();
		m_iFrameCounter = 0;
		Teleport_Ready = true;
		if (m_fAnim_Time >= 3.f)
		{
			m_bStuned = false;
		}
	}

	if (m_tActorDesc.iHp <= 0 && !m_bDeadMotion)
	{
		m_tActorDesc.iHp = 0;

		for (int i = 0; i < 32; i++)
		{
			m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Smoke_Effect"), &m_tActorDesc);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Feather_Effect"), &m_tActorDesc);
		}

		m_bDeadMotion = true;
		m_eBird_State = BIRD_DIE;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_End"));
		m_iFrameCounter = 0;
	}
	else if (m_eBird_State == BIRD_PEACE && (m_tActorDesc.iMaxHp != m_tActorDesc.iHp))
	{
		m_eBird_State = BIRD_ACTIVE_START;
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Active_Start"));
		m_iFrameCounter = 0;
	}
	else if (m_eBird_State == BIRD_IDLE && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Active_Start"));
		m_eBird_State = BIRD_ACTIVE_START;
		m_iFrameCounter = 0;
		Teleport_Ready = false;
	}
	else if (m_eBird_State == BIRD_ACTIVE_START && (m_iFrameCounter > 0))
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Active"));
		m_eBird_State = BIRD_ACTIVE;
		m_iFrameCounter = 0;
		RandTemp = rand() % 3;
		Attack = true;
		if (!Teleport_Ready)
		{

			if (RandTemp == 0)
			{

				for (int i = 0; i < 4; i++)
				{
					if (rand() % 2 == 0)
						m_vTelePort_Pos.push_back({ Origin_Pos.x + rand() % 3, Origin_Pos.y, Origin_Pos.z + rand() % 3 });
					else
						m_vTelePort_Pos.push_back({ Origin_Pos.x - rand() % 3, Origin_Pos.y, Origin_Pos.z - rand() % 3 });

					if (m_vTelePort_Pos.empty())
					{
						break;
					}
					m_tActorDesc.m_targetPos = _float3(m_vTelePort_Pos.back());

					m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Bird_Teleport_Effect2"), &m_tActorDesc);
					m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Bird_Teleport_Light_Effect"), &m_tActorDesc);

				}
			}
			Teleport_Ready = true;
		}

	}
	else if (m_eBird_State == BIRD_TELEPORT)
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Active_Start"));

		if (TelePort(fTimeDelta))
			m_eBird_State = BIRD_ACTIVE;
		else
			m_eBird_State = BIRD_TELEPORT;

	}
	else if (m_eBird_State == BIRD_ACTIVE && (m_iFrameCounter > 1))//38
	{
		m_pAnimatorCom->Change_Texture(TEXT("Monster_Bird_Idle"));
		m_eBird_State = BIRD_IDLE;
	}
	else if (m_eBird_State == BIRD_END)
	{
		m_pAnimatorCom->Change_Texture(TEXT("BIRD_END"));
	}

	//dsafa
	if (m_Monster_Statue == MONSTER_STATE_STUN)
	{
		m_bStuned = true;
		m_Monster_Statue = MONSTER_STATE_DEFAULT;
	}
	else if (m_Monster_Statue == MONSTER_STATE_DEFAULT)
	{
		m_iSpeed = 1.f;
	}
	else if (m_Monster_Statue == MONSTER_STATE_SLOW)
	{
		m_iSpeed = .5f;
	}


	if(Attack)
	{
		m_fHittimer += fTimeDelta;

		if (RandTemp == 0)
		{
			m_eBird_State = BIRD_TELEPORT;
			Attack = false;
		}
		if (RandTemp == 1)
		{
			Fire_Bullet(fTimeDelta);
		}
		if (RandTemp == 2)
		{
			for (int i = 0; i < 4; i++)
			{
				_float3 mypos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				m_tActorDesc.m_targetPos = _float3((mypos.x + rand() % 5), mypos.y, (mypos.z + rand() % 2));
				
				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Monster_Spawn_Effect"), &m_tActorDesc);
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Magic_Mirror"), &m_tActorDesc)))
					continue;
				Attack = false;
			}
		}
	}
	
	__super::SetUp_OnTerrain(m_pTransformCom, 0.3f);
}

void CMonster_Bird::Late_Update(_float fTimeDelta)
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
	{
		m_iFrameCounter++;
	}

	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMonster_Bird::Render()
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

HRESULT CMonster_Bird::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
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
	return S_OK;
}


HRESULT CMonster_Bird::Fire_Bullet(_float fTimeDelta)
{
	if (60 * fTimeDelta >= 1)
	{
		m_tActorDesc.m_targetPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Bullet"), TEXT("Prototype_GameObject_Bird_Bullet"), &m_tActorDesc)))
			return E_FAIL;
		Attack = false;
	}
	return S_OK;
}

bool CMonster_Bird::TelePort(_float fTimeDelta)
{
	if (60 * fTimeDelta >= 1)
	{
		m_FireTime++;
		if (m_vTelePort_Pos.empty())
		{
			m_vTelePort_Pos.clear();
			return 1;
		}

		if (m_FireTime >= 30)// 이동
		{
			for (int i =0; i< 8;i++)
			{
				m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Smoke_Effect"), &m_tActorDesc);
				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Feather_Effect"), &m_tActorDesc);
			}
			m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
			m_pGameInstance->SoundPlay(L"sfx_MagicianMonster_Teleport.wav", _uint(CHANNELID::SOUND_EXPLOSIVE), .5f);
			// 플레이어 멀어지면 소리 안남
			_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
				PlayerPos.z > (myPos.z + 10) || PlayerPos.z <(myPos.z - 10))
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTelePort_Pos.back());
			m_vTelePort_Pos.pop_back();
			m_FireTime = 0;
		}
	}
	return 0;
}

HRESULT CMonster_Bird::Ready_Components() 
{
	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

//Texture

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Bird_Idle"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_Idle"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Bird_Active_Start"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_Active_Start"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Bird_Active"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_Active"))));

	m_pAnimatorCom->Add_Texture(TEXT("Monster_Bird_End"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bird_End"))));

//  End

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

HRESULT CMonster_Bird::SetUp_RenderState()
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

HRESULT CMonster_Bird::Reset_RenderState()
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


CMonster_Bird* CMonster_Bird::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CMonster_Bird* pInstance = new CMonster_Bird(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Bird"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CMonster_Bird::Clone(void* pArg)
{
	CMonster_Bird* pInstance = new CMonster_Bird(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Bird"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Bird::Free()
{
	__super::Free();
}
