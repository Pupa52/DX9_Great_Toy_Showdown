#include "stdafx.h"
#include "..\Public\Player_Bullet.h"

#include "Monster.h"
#include "Player.h"
#include "Effect.h"

CPlayer_Bullet::CPlayer_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CPlayer_Bullet::CPlayer_Bullet(const CPlayer_Bullet& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CPlayer_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Bullet::Initialize(void* pArg)
{
	// 잠시 추가함 
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);
	PLAYER_BULLET_DESC* m_pPlayerBullet_Desc = static_cast<PLAYER_BULLET_DESC*>(pArg);

	m_iDamage = m_pPlayerBullet_Desc->iAttack;
	m_eLevelID = m_pPlayerBullet_Desc->eLevelID;
	m_eBullet_Id = m_pPlayerBullet_Desc->eBulletID;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	switch (m_pPlayerBullet_Desc->iPlayerCharacter)
	{
	case (_uint)PLAYER_CHARACTER::PLAYER_CAT:
		m_eBulletType = BULLETTYPE_PISTOL;
		m_fMaxDistance = 7.f;
		break;

	case (_uint)PLAYER_CHARACTER::PLAYER_DOG:
		m_eBulletType = BULLETTYPE_RIFLE;
		m_fMaxDistance = 5.f;
		break;

	case (_uint)PLAYER_CHARACTER::PLAYER_PENGUIN:
		m_eBulletType = BULLETTYPE_SHOTGUN;
		m_fMaxDistance = 3.f;
		break;
	default:
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 각도
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_pPlayerBullet_Desc->fAngle - 90));
	// 발사되는 위치
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pPlayerBullet_Desc->m_Pos);
	// 크기
	m_pTransformCom->Scaling(.5f, .5f, .5f);

	_float3 vDist = m_pPlayerBullet_Desc->m_targetPos - m_pPlayerBullet_Desc->m_Pos;

	// 목표로 가는 벡터
	m_fTarget_Vector = *D3DXVec3Normalize(&vDist, &vDist);

	m_fSpeed = 10.f;
	m_fAngle = m_pPlayerBullet_Desc->fAngle;

	if (BULLETTYPE_PISTOL == m_eBulletType)
	{
		srand((_uint)time(nullptr));
		m_iBulletNum = rand() % 4;
	}
	m_pColliderCubeCom->Scaling(.2f, .2f, .2f);

	return S_OK;
}

_uint CPlayer_Bullet::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		if (LEVEL_GAMEPLAY == m_eLevelID)
		{
			CEffect::EFFECT_DESC desc = {};
			desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Blast"), &desc)))
				return OBJ_NOEVENT;
		}
		else if (LEVEL_MINIGAME == m_eLevelID)
		{
			CEffect::EFFECT_DESC desc = {};
			desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MINIGAME, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_MiniGame_Hit"), &desc)))
				return OBJ_NOEVENT;
		}

		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CPlayer_Bullet::Update(_float fTimeDelta)
{
	if(BULLETTYPE_SHOTGUN != m_eBulletType)
	{
		_float3 vTargetDir = { m_fTarget_Vector * fTimeDelta * m_fSpeed };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vTargetDir);

		m_fMoveDistance += D3DXVec3Length(&vTargetDir);
	}
	else
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.x += cos(D3DXToRadian(m_fAngle)) * fTimeDelta * m_fSpeed;
		vPos.z += sin(D3DXToRadian(m_fAngle)) * fTimeDelta * m_fSpeed;

		_float3 vMoveVec = vPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		m_fMoveDistance += D3DXVec3Length(&vMoveVec);
	}

}

void CPlayer_Bullet::Late_Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	// 사거리 수정.
	if (m_fMoveDistance > m_fMaxDistance)
	{
		m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Player_Bullet_Hit_Effect"), (void*)m_ptBullet_DESC);
		m_bDead = true;
	}
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer_Bullet::Render()
{
	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(m_iBulletNum)))
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

HRESULT CPlayer_Bullet::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if(LEVEL_GAMEPLAY == m_eLevelID)
	{
		if (strLayerTag == TEXT("Layer_Monster"))
		{
			if (!(static_cast<CMonster*>(pOther)->Get_Dead()))
				m_bDead = true;
		}
	}


	return S_OK;
}

HRESULT CPlayer_Bullet::Ready_Components()
{
	wstring strPrototypeTag = L"";

	switch (m_eBulletType)
	{
	case BULLETTYPE_PISTOL:
		strPrototypeTag = TEXT("Prototype_Component_Texture_Player_Bullet_Pistol");
		break;

	case BULLETTYPE_RIFLE:
		strPrototypeTag = TEXT("Prototype_Component_Texture_Player_Bullet_Rifle");
		break;

	case BULLETTYPE_SHOTGUN:
		strPrototypeTag = TEXT("Prototype_Component_Texture_Player_Bullet_Shotgun");
		break;

	default:
		break;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Collider_Cube */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Bullet::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	/* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);

	return S_OK;

}

HRESULT CPlayer_Bullet::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPlayer_Bullet* CPlayer_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer_Bullet* pInstance = new CPlayer_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Create Failed : CPlayer_Bullet"));
	}

	return pInstance;
}

CGameObject* CPlayer_Bullet::Clone(void* pArg)
{
	CPlayer_Bullet* pInstance = new CPlayer_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Clone Failed : CPlayer_Bullet"));
	}

	return pInstance;
}

void CPlayer_Bullet::Free()
{
	__super::Free();
}

