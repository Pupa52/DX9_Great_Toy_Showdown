#include "stdafx.h"
#include "..\Public\Boss_Ball.h"

#include "Player.h"
#include "Layer.h"

CBoss_Ball::CBoss_Ball(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CBoss_Ball::CBoss_Ball(const CBoss_Ball& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CBoss_Ball::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Ball::Initialize(void* pArg)
{
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bBall = true;

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_fSpeed = 2.f;

	m_Sprite_Time = 0;

	m_TargetPos = m_ptBullet_DESC->m_targetPos;

	//m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_ptBullet_DESC->fAngle));
	//m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
	_float frand = rand() % 5 * 0.1f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, 1.5f + frand, m_ptBullet_DESC->m_Pos.z));
	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Scaling(2.5f, 2.5f, 2.5f);

	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);
	m_BulletRange = 500.f;

	RandCard = rand() % 3;

	return S_OK;
}

_uint CBoss_Ball::Priority_Update(_float fTimeDelta)
{

	if (m_fDeltaTime >= 5.f)
	{
		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CBoss_Ball::Update(_float fTimeDelta)
{
	m_fTimer = fTimeDelta;
	m_fDeltaTime += fTimeDelta;
	m_Calculate_Range += 0.5f;
	if (m_Calculate_Range >= 30.f)
		m_Calculate_Range = 10.f;


	Calculate_Target();
	m_Sprite_Time += fTimeDelta;
	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVECTOR3 dir = MonsterMatrix - m_vAttackPos;
	D3DXVec3Normalize(&dir, &dir);
	MonsterMatrix = MonsterMatrix - dir * fTimeDelta * m_fSpeed;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, MonsterMatrix);
}

void CBoss_Ball::Late_Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (FAILED(m_pGameInstance->is_Culling(m_pTransformCom)))
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Ball::Render()
{

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture(RandCard)))
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

HRESULT CBoss_Ball::Ready_Components()
{
	/* For.Com_Textrue */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Boss_Balls "),
		TEXT("Com_Textrue"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Cube */
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

HRESULT CBoss_Ball::SetUp_RenderState()
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

HRESULT CBoss_Ball::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBoss_Ball::Calculate_Target()
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
	return S_OK;
}

HRESULT CBoss_Ball::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player"))
	{
		m_bdead = true;
	}
	if (strLayerTag == TEXT("Layer_Friend_Bot"))
	{
		m_bdead = true;
	}
	if (strLayerTag == TEXT("Layer_Monster_Bullet") && static_cast<CBullet*>(pOther)->Get_Ball())
	{
		//m_bBallPush = true;
		_float3 _vpOther_Pos, _vmyPos;

		_vpOther_Pos = static_cast<CBullet*>(pOther)->Get_BulletPos();
		_vmyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (_vmyPos.x < _vpOther_Pos.x)
			m_pTransformCom->Go_Left(m_fTimer * 0.3f);
		else if (_vmyPos.x >= _vpOther_Pos.x)
			m_pTransformCom->Go_Right(m_fTimer * 0.3f);

		if (_vmyPos.z < _vpOther_Pos.z)
			m_pTransformCom->Go_Backward(m_fTimer * 0.3f);
		else if (_vmyPos.z >= _vpOther_Pos.z)
			m_pTransformCom->Go_Straight(m_fTimer * 0.3f);
	}
	if (strLayerTag == TEXT("Layer_Bot"))
	{
		m_bdead = true;
	}

	return S_OK;
}

CBoss_Ball* CBoss_Ball::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CBoss_Ball* pInstance = new CBoss_Ball(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Ball"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBoss_Ball::Clone(void* pArg)
{
	CBoss_Ball* pInstance = new CBoss_Ball(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Ball"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Ball::Free()
{
	__super::Free();
}
