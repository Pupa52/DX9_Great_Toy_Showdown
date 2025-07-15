#include "stdafx.h"
#include "..\Public\Player.h"

#include "Head.h"
#include "Body.h"

#include "Weapon.h"

#include "State_Attack.h"
#include "State_Idle.h"
#include "State_Move.h"
#include "State_MoveAttack.h"
#include "Player_Bullet.h"

#include "Range.h"
#include "Layer.h"
#include "TileMgr.h"
#include "Monster.h"
#include "Bullet.h"
#include "Effect_Shadow.h"
#include "Item_Possesion.h"
#include "Camera.h"
#include "Storm.h"
#include "Layer.h"
#include "Item_Object.h"
#include "Skill.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CActor{ pGraphic_Device }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CActor(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);
	m_ePlayerID = pDesc->ePlayerID;
	m_eLevel = pDesc->eLevel;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Add_Part()))
		return E_FAIL;

	m_eCurState = PLAYER_STATE::STATE_IDLE;

	for (size_t i = 0; i < PART_END; ++i)
	{
		m_pParts[i]->Set_Angle(Calculate_MouseDir());// 각도
	}

	if(LEVEL_GAMEPLAY == m_eLevel)
	{
		m_tActorDesc.iHp = 300;
		m_tActorDesc.iMaxHp = 300;
	}
	else if (LEVEL_MINIGAME == m_eLevel)
	{
		m_tActorDesc.iHp = 100;
		m_tActorDesc.iMaxHp = 100;
	}

	m_pColliderCom->Scaling(0.5f, 0.5f, 0.5f);

	m_fSpeed = 3.f;
	m_fHitDelay = 0.f;
	m_pTransformCom->Scaling(1.4f, 1.4f, 1.4f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPos);


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
	m_fRealCoolTime = 5.f;
	m_fTileCoolTime = 1.f;

	m_fHitDelay = .5f;
	m_fCoolTime = m_fRealCoolTime;
	m_fTileCheckTime = m_fTileCoolTime;


	if(m_eLevel == LEVEL_GAMEPLAY)
	{
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Shadow"), TEXT("Prototype_GameObject_Shadow"), &m_tActorDesc);
		// 0620 재진추가
		if (m_pShadow == nullptr)
		{
			CLayer* Shadow = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, (TEXT("Layer_Shadow")));

			if (nullptr == Shadow)
				return E_FAIL;

			CGameObject* Shadow_Obj = Shadow->Get_Objects().back();

			static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_Player();
			static_cast<CEffect_Shadow*>(Shadow_Obj)->Set_pMonster(this);
		}

#pragma region 임시 아이템 
		CItem_Object::ITEM_DESC desc = {};

		desc.pTerrainTransform = m_pTerrainTransform;
		desc.pTerrainVIBuffer = m_pTerrainVIBuffer;

		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.x -= 3.f;
		desc.eItemType = CItem_Object::ITEM_SWORD;
		desc.vPos = vPos;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
			return E_FAIL;

		vPos.x += 3.f;
		desc.eItemType = CItem_Object::ITEM_SKATE;
		desc.vPos = vPos;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
			return E_FAIL;

		vPos.x += 3.f;
		desc.eItemType = CItem_Object::ITEM_MAGAZINE;
		desc.vPos = vPos;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_ItemObject"), &desc)))
			return E_FAIL;
#pragma endregion
	}


	return S_OK;
}

_uint CPlayer::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	Set_State();

	if (true == m_bSlow)
		m_fSpeed = 1.f;
	else
		m_fSpeed = 3.f;

	if(false == m_bStun)
	{
		for (size_t i = 0; i < PART_END; ++i)
		{
			m_pParts[i]->Set_Angle(Calculate_MouseDir());
		}
	}

	m_pFSM->Priority_Update(fTimeDelta);
	m_fFireDelay += fTimeDelta;
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (auto& Pair : m_Items)
		Pair.second->Priority_Update(fTimeDelta);

	return OBJ_NOEVENT;
}

void CPlayer::Update(_float fTimeDelta)
{
	cout << "x좌표 :" << m_pTransformCom->Get_State(CTransform::STATE_POSITION).x << "z 좌표:" << m_pTransformCom->Get_State(CTransform::STATE_POSITION).z << endl;
	if(m_eLevel == LEVEL_GAMEPLAY)
	{
		if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_fTileCheckTime >= m_fTileCoolTime)
		{
			// 0626 이재진
			_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.y += 1.f;
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Scanner"), TEXT("Prototype_GameObject_Scanner"), &vPos);


			m_fTileCheckTime = 0.f;
		}

		Compute_StormLength();
	}

	if (true == m_bStun)
	{
		m_eCurState = PLAYER_STATE::STATE_IDLE;

		if (m_fStunTime > 3.f)
		{
			m_bStun = false;
			m_fStunTime = 0.f;
			m_tActorDesc.iHp = m_tActorDesc.iMaxHp;
		}
	}
	else
	{
		if (FAILED(Player_Move(fTimeDelta)))
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && m_iBulletCount > 0)
				m_eCurState = PLAYER_STATE::STATE_ATTACK;
			else
				m_eCurState = PLAYER_STATE::STATE_IDLE;
		}
		else
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && m_iBulletCount > 0)
				m_eCurState = PLAYER_STATE::STATE_MOVEATTACK;
			else
				m_eCurState = PLAYER_STATE::STATE_MOVE;
		}

		if (m_pGameInstance->Key_Down('G') && m_fCoolTime > m_fRealCoolTime)
		{
			m_bUseSkill = true;
			m_fCoolTime = 0.f;
		}

		if (m_pGameInstance->Key_Down('R'))
			m_iBulletCount = 0;
	}

	if (m_pGameInstance->Key_Down('I'))
		m_tActorDesc.iHp = 0;

	m_pFSM->Update(fTimeDelta);

	for (auto& Pair : m_Items)
		Pair.second->Update(fTimeDelta);
	
	
	__super::SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_fHitDelay += fTimeDelta;
	m_fCoolTime += fTimeDelta;
	m_fTileCheckTime += fTimeDelta;

	if (true == m_bStun)
		m_fStunTime += fTimeDelta;

	if (m_iBulletCount <= 0)
	{
		m_fReloadTime += fTimeDelta;
		m_bReloading = true;
	}
	if (m_tActorDesc.iExp > m_tActorDesc.iMaxExp)
	{
		++m_tActorDesc.iLevel;

		m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			// 레벨업 이펙트 
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_Component_Effect_Level_UP_Paint"), &m_tActorDesc);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_Component_Effect_Level_Up_Box"), &m_tActorDesc);

		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_LEVELUP));
		m_pGameInstance->SoundPlay(L"Player_LevelUp.wav", _uint(CHANNELID::SOUND_PLAYER_LEVELUP), .7f);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Level_Up_Paint"), &m_tActorDesc);

		if ((m_tActorDesc.iLevel % 2) == 0)
		{
			// 아이템 주기
			//m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_ITEM"), TEXT("Prototype_GameObject_Burn_Smoke"), &m_tActorDesc);
		}
		m_tActorDesc.iExp = 0;
		m_tActorDesc.iMaxExp += 10;
		m_tActorDesc.iMaxHp += 80;
		m_tActorDesc.iAttack += 2;
	}
	if (m_fReloadTime > 1.f)
	{
		m_iBulletCount = m_iTotalBulletCount;
		m_fReloadTime = 0.f;
		m_bReloading = false;
	}

	m_bSlow = false;

	if(LEVEL_GAMEPLAY == m_eLevel)
	{
		_float fStormRadius = static_cast<CStorm*>
			(*m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Storm"))->Get_Objects().begin())->Get_Radius();

		if (m_fStormLength > fStormRadius)
		{
			m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
			if (m_fHitDelay > 0.5f)
			{
				m_tActorDesc.iHp -= 10;
				m_fHitDelay = 0.f;
				CCamera* pCamera = static_cast<CCamera*>
					(*m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"))->Get_Objects().begin());
				pCamera->Shake_Camera(.1f);
			}
		}
		else
			m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	m_pFSM->Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	for (auto& Pair : m_Items)
		Pair.second->Late_Update(fTimeDelta);

	if (m_tActorDesc.iHp <= 0)
	{
		if (LEVEL_GAMEPLAY == m_eLevel)
			m_bDead = true;
		else
		{
			m_bStun = true;
		}
	}
}

HRESULT CPlayer::Render()
{
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pFSM->Render();
	m_pColliderCom->Render();

	if (FAILED(Reset_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	CCamera* pCamera = static_cast<CCamera*>
		(*m_pGameInstance->Find_Layer(m_eLevel, TEXT("Layer_Camera"))->Get_Objects().begin());

	if (m_fHitDelay > 0.5f)
	{
		if(LEVEL_GAMEPLAY == m_eLevel)
		{

			if (TEXT("Layer_Monster") == strLayerTag)
			{

				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
				m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);

				CMonster* pMonster = static_cast<CMonster*>(pOther);
				m_tActorDesc.iHp -= pMonster->Get_Damage();

				m_fHitDelay = 0.f;

				pCamera->Shake_Camera(.1f);
			}
			if (TEXT("Layer_Monster_Bullet") == strLayerTag)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
				m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);
				CBullet* pBullet = static_cast<CBullet*>(pOther);
				m_tActorDesc.iHp -= pBullet->Get_Bullet_DESC()->iAttack;

				m_fHitDelay = 0.f;
				pCamera->Shake_Camera(.1f);
			}
			if (TEXT("Layer_Bot_Bullet") == strLayerTag)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
				m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);
				CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
				m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();

				m_fHitDelay = 0.f;
				pCamera->Shake_Camera(.1f);
			}
			if (TEXT("Layer_SECOND_Bot_Bullet") == strLayerTag)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
				m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);
				CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
				m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();

				m_fHitDelay = 0.f;
				pCamera->Shake_Camera(.1f);
			}
			if (TEXT("Layer_THIRD_Bot_Bullet") == strLayerTag)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_PLAYER_Hit));
				m_pGameInstance->SoundPlay(L"SFX_PlayerHit_voice.wav", _uint(CHANNELID::SOUND_PLAYER_Hit), .5f);
				CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
				m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();

					m_fHitDelay = 0.f;
					pCamera->Shake_Camera(.1f);
			}
		}

		if (m_eLevel == LEVEL_MINIGAME)
		{
			if (TEXT("Layer_Bot_Skill") == strLayerTag || TEXT("Layer_SECOND_Bot_Skill") == strLayerTag)
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
						cout << "플레이어 : " << m_tActorDesc.iHp << endl;
						pCamera->Shake_Camera(.1f);
						break;
					case PLAYER_CHARACTER::PLAYER_PENGUIN:
						m_bStun = true;
						m_tActorDesc.iHp -= pSkill->Get_Attack();
						m_fHitDelay = 0.f;
						cout << "플레이어 : " << m_tActorDesc.iHp << endl;
						pCamera->Shake_Camera(.1f);
						break;
					}
				}
			}
		}
	}
	if (m_eLevel == LEVEL_MINIGAME)
	{
		// 여기서 미니게임 봇 충돌.
		if (TEXT("Layer_Bot_Bullet") == strLayerTag || TEXT("Layer_SECOND_Bot_Bullet") == strLayerTag)
		{
			CPlayer_Bullet* pBullet = static_cast<CPlayer_Bullet*>(pOther);
			m_tActorDesc.iHp -= pBullet->Get_Player_mDamage();
			pBullet->Set_Dead();
			m_fHitDelay = 0.f;
			pCamera->Shake_Camera(.1f);
			cout << "플레이어 : " << m_tActorDesc.iHp << endl;
		}
	}



	return S_OK;
}

HRESULT CPlayer::Add_Item(wstring strItemKey, CItem_Possesion* pItem_Possesion)
{
	if (nullptr == pItem_Possesion)
		return E_FAIL;

	auto& iter = m_Items.find(strItemKey);

	if (m_Items.end() != iter)
		return E_FAIL;

	m_Items.emplace(strItemKey, pItem_Possesion);

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
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

HRESULT CPlayer::SetUp_RenderState()
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

HRESULT CPlayer::Reset_RenderState()
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

HRESULT CPlayer::Add_Part()
{
	CPart::PART_DESC desc = {};

	desc.pTransformCom = m_pTransformCom;
	desc.pVIBufferCom = m_pVIBufferCom;

	m_pParts[PART_HEAD] = CHead::Create(m_pGraphic_Device, m_eLevel, &desc, m_ePlayerID);

	m_pParts[PART_BODY] = CBody::Create(m_pGraphic_Device, m_eLevel, &desc, m_ePlayerID);

	m_pParts[PART_WEAPON] = CWeapon::Create(m_pGraphic_Device, m_eLevel, &desc, m_ePlayerID);

	return S_OK;
}

_float CPlayer::Calculate_MouseDir()
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
	_float3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDist = (vPlayerPos.y - vRayPos.y) / vRayDir.y;


	vTargetPos.x = vRayPos.x + vRayDir.x * fDist;
	vTargetPos.y = vPlayerPos.y;
	vTargetPos.z = vRayPos.z + vRayDir.z * fDist;

	m_vTargetPos = vTargetPos;

	_float3 vTargetDir = vTargetPos - vPlayerPos;

	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float fDot = D3DXVec3Dot(&vTargetDir, &_float3(1.f, 0.f, 0.f));
	_float fAngle = acosf(fDot);

	if (vTargetPos.z < vPlayerPos.z)
		fAngle *= -1.f;

	return fAngle;
}

void CPlayer::Set_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case PLAYER_STATE::STATE_IDLE:
			m_pFSM->Change_State(CState_Idle::Create(m_pParts, PART_END, m_pTransformCom, 
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp, 
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, ID_PLAYER, m_eLevel));
			break;

		case PLAYER_STATE::STATE_MOVE:
			m_pFSM->Change_State(CState_Move::Create(m_pParts, PART_END, m_pTransformCom, 
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp, 
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, ID_PLAYER, m_eLevel));
			break;

		case PLAYER_STATE::STATE_ATTACK:
			m_pFSM->Change_State(CState_Attack::Create(m_pParts, PART_END, m_pTransformCom, 
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp, 
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, ID_PLAYER, m_eLevel));
			break;

		case PLAYER_STATE::STATE_MOVEATTACK:
			m_pFSM->Change_State(CState_MoveAttack::Create(m_pParts, PART_END, m_pTransformCom, 
				m_fFireDelay, &m_vTargetPos, (_uint)m_ePlayerID, &m_bUseSkill, &m_tActorDesc.iHp, 
				&m_iBulletCount, &m_tActorDesc.iLevel, &m_tActorDesc.iAttack, &m_tActorDesc.iMaxHp, ID_PLAYER, m_eLevel));
			break;

		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}


HRESULT CPlayer::Player_Move(_float fTimeDelta)
{
	_float3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	HRESULT hr = E_FAIL;

	_float3 vMoveDir = { 0.f, 0.f, 0.f };

	CLayer* pLayer = m_pGameInstance->Find_Layer(m_eLevel, TEXT("Layer_TileMgr"));
	CTileMgr* pMgr = static_cast<CTileMgr*>(pLayer->Get_Objects().front());

	if (m_pGameInstance->Key_Pressing('W'))
	{
		vMoveDir.z += 1.f;
	}
	if (m_pGameInstance->Key_Pressing('S'))
	{
		vMoveDir.z -= 1.f;
	}
	if (m_pGameInstance->Key_Pressing('A'))
	{
		vMoveDir.x -= 1.f;
	}
	if (m_pGameInstance->Key_Pressing('D'))
	{
		vMoveDir.x += 1.f;
	}

	if (vMoveDir != _float3{ 0.f, 0.f, 0.f })
	{
		_float3 vMovePos = *D3DXVec3Normalize(&vMoveDir, &vMoveDir) * fTimeDelta * m_fSpeed;
		vPlayerPos += vMovePos;
		hr = S_OK;
	}

	if (FAILED(pMgr->Check_MoveTile(vPlayerPos)))
		return E_FAIL;

	if(LEVEL_MINIGAME == m_eLevel)
	{
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

		if (FAILED(pMgr->Change_Three(vPlayerPos, eTileType)))
			return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	return hr;
}

void CPlayer::Compute_StormLength()
{
	// 중점
	_float3 vCenter = { 59.5f, 1.f, 59.5f };
	_float3 vDir = vCenter - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_fStormLength = D3DXVec3Length(&vDir);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (size_t i = 0; i < PART_END; ++i)
		Safe_Release(m_pParts[i]);

	for (auto& Pair : m_Items)
		Safe_Release(Pair.second);

	Safe_Release(m_pFSM);
	Safe_Release(m_pColliderCom);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
}
