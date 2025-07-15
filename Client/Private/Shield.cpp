#include "stdafx.h"
#include "Shield.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Bot.h"
#include "Layer.h"
#include "Electric_Bolt.h"
#include "Damage.h"

CShield::CShield(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CSkill(pGraphic_Device)
{
}

CShield::CShield(const CShield& Prototype)
	: CSkill(Prototype)
{
}

HRESULT CShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShield::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	SKILL_SHIELD_DESC* pDesc = static_cast<SKILL_SHIELD_DESC*>(pArg);

	m_iRank = pDesc->iRank;
	m_eLevelID = pDesc->eLevelID;

	m_pPlayerTransform = pDesc->pPlayerTransform;
	Safe_AddRef(m_pPlayerTransform);

	
	switch (m_iRank)
	{
	case 0:
		m_iShiledAmount += 100;
		m_fTotalActiveTime = m_fActiveTime = 5.f;
		m_iAttack = 5;
		break;

	case 1:
		m_iShiledAmount += 250;
		m_fTotalActiveTime = m_fActiveTime = 7.f;
		m_iAttack = 10;
		break;

	case 2:
		m_iShiledAmount += 400;
		m_fTotalActiveTime = m_fActiveTime = 10.f;
		m_iAttack = 15;
		break;

	default:
		return E_FAIL;
	}
	m_pHp = pDesc->pHp;
	m_pMaxHp = pDesc->pMaxHp;
	*m_pHp += m_iShiledAmount;
	*m_pMaxHp += m_iShiledAmount;
	m_pTransformCom->Scaling(1.2f, 1.2f, 1.2f);
	m_pColliderCom->Scaling(0.5f, 0.5f, 0.5f);

	m_eCharacter = PLAYER_CHARACTER::PLAYER_DOG;

	return S_OK;
}

_uint CShield::Priority_Update(_float fTimeDelta)
{
	//SKILL_SHIELD_DESC Desc;
	//Desc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (m_iShiledAmount <= 0 || m_fActiveTime < 0.f)
	{
		*m_pHp -= m_iShiledAmount;
		*m_pMaxHp -= m_iShiledAmount;
		return OBJ_DEAD;
	}

	_float3 vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.z -= 0.05f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	return OBJ_NOEVENT;
}

void CShield::Update(_float fTimeDelta)
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if(LEVEL_GAMEPLAY == m_eLevelID)
	{
		if (m_fElectricTime > 1.f)
		{
			CLayer* pMonsterLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

			if (nullptr != pMonsterLayer)
			{
				auto& MonsterList = pMonsterLayer->Get_Objects();

				for (auto& elem : MonsterList)
				{
					CMonster* pMonster = static_cast<CMonster*>(elem);
					_float3 vDir = pMonster->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_float fLength = D3DXVec3Length(&vDir);

					if (fLength < 5.f)
					{
						SKILL_DESC desc = {};
						desc.iRank = m_iRank;
						desc.vPos = pMonster->Get_Pos();
						desc.vTargetPos = desc.vPos;
						if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Skill"),
							TEXT("Prototype_GameObject_PlayerSkill_ElectricBolt"), &desc)))
							return;

						pMonster->Set_Hp(pMonster->Get_Hp() - m_iAttack);
						CDamage::DAMAGE_DESC descDamage = {};
						descDamage.iDamage = m_iAttack;
						descDamage.vPos = static_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

						m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
							TEXT("Prototype_GameObject_Damage"), &descDamage);
					}

				}
			}

			CLayer* pBotLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Bot"));

			if (nullptr != pBotLayer)
			{
				auto& BotList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Bot"))->Get_Objects();

				for (auto& elem : BotList)
				{
					CBot* pBot = static_cast<CBot*>(elem);
					_float3 vDir = pBot->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_float fLength = D3DXVec3Length(&vDir);

					if (fLength < 5.f)
					{
						SKILL_DESC desc = {};
						desc.iRank = m_iRank;
						desc.vPos = pBot->Get_Pos();
						desc.vTargetPos = desc.vPos;
						if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player_Skill"),
							TEXT("Prototype_GameObject_PlayerSkill_ElectricBolt"), &desc)))
							return;

						pBot->Set_Hp(pBot->Get_Hp() - m_iAttack);
						CDamage::DAMAGE_DESC descDamage = {};
						descDamage.iDamage = m_iAttack;
						descDamage.vPos = static_cast<CTransform*>(pBot->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

						m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Damage"),
							TEXT("Prototype_GameObject_Damage"), &descDamage);
					}
				}
			}
			m_fElectricTime = 0.f;
		}
	}


	m_pColliderCom->Update_Collider(vPos);
	if (FAILED(Compute_ViewZ(TEXT("Com_Transform"))))
		return;
}

void CShield::Late_Update(_float fTimeDelta)
{
	m_fActiveTime -= fTimeDelta;
	m_fShieldHitDelay += fTimeDelta;
	m_fElectricTime += fTimeDelta;
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CShield::Render()
{
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_Texture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShield::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if(LEVEL_GAMEPLAY == m_eLevelID)
	{
		// 받은 데미지만큼 쉴드 깍고 피는 도로 채울 것.
		if (TEXT("Layer_Monster") == strLayerTag && m_fShieldHitDelay > .5f)
		{
			CMonster* pMonster = static_cast<CMonster*>(pOther);
			m_iShiledAmount -= pMonster->Get_ActorDesc().iAttack;
			m_fShieldHitDelay = 0.f;
		}
	}

	return S_OK;
}

HRESULT CShield::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Skill_Shield"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShield::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	return S_OK;
}

HRESULT CShield::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

CShield* CShield::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShield* pInstance = new CShield(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShield::Clone(void* pArg)
{
	CShield* pInstance = new CShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShield::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPlayerTransform);
}
