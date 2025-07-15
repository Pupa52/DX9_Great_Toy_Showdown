#include "stdafx.h"
#include "..\Public\Bullet_Police_Laser.h"

#include "Player.h"

CBullet_Police_Laser::CBullet_Police_Laser(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CBullet_Police_Laser::CBullet_Police_Laser(const CBullet_Police_Laser& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CBullet_Police_Laser::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Police_Laser::Initialize(void* pArg)
{
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);
	m_ptBullet_DESC->iAttack = 0;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayerTransform = dynamic_cast<CTransform*>
		(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, m_ptBullet_DESC->m_Pos.y, m_ptBullet_DESC->m_Pos.z));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, m_ptBullet_DESC->m_Pos.y, m_ptBullet_DESC->m_Pos.z));



	m_pAnimatorCom->Change_Texture(TEXT("Bullet_Laser"));

	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Scaling(.05f, m_fScale, .05f);
	m_pColliderCubeCom->Scaling(.5f, 1.f, .5f);
	m_BulletRange = 3.5f;
	return S_OK;
}

_uint CBullet_Police_Laser::Priority_Update(_float fTimeDelta)
{
	if (m_bdead)
	{
		return OBJ_DEAD;
	}


	return OBJ_NOEVENT;
}

void CBullet_Police_Laser::Update(_float fTimeDelta)
{
	m_Sprite_Time += fTimeDelta;

	if (m_Sprite_Time <= 2)
	{
		m_vLastPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), Laser_Angle());
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 95.f);
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), Laser_YAngle());
		m_bFire = true;
	}
	else 
	{
		if (m_bFire)
		{


			m_bFire = false;
			m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), Laser_Angle());
			m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 95.f);
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), Laser_YAngle());
			m_vLastPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		}
		if (m_Sprite_Time >= 4)
		{
			m_pTransformCom->Go_Up(fTimeDelta);

			if (m_bFireSound)
			{
				m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_Laser));
				m_pGameInstance->SoundPlay(L"SkillParts_LaserBeam.wav", _uint(CHANNELID::SOUND_Laser), .5f);

				// 플레이어 멀어지면 소리 안남
				_float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
					PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
				{
					m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_Laser));
				}
				m_bFireSound = false;
			}
			m_fScale = 1.f;
			m_pTransformCom->Scaling(.05f, m_fScale, .05f);
		}
		if (m_Sprite_Time >= 5)
		{
			m_bdead = true;
		}
	}
	//m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), 90.f);
}

void CBullet_Police_Laser::Late_Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pAnimatorCom->Move_Frame(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBullet_Police_Laser::Render()
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

HRESULT CBullet_Police_Laser::Ready_Components()
{

	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;


	m_pAnimatorCom->Add_Texture(TEXT("Bullet_Laser"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Laser"))));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCubeCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(1.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Police_Laser::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	
	if (m_Sprite_Time >= 2)
	{
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 0, 0));

		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		// arg2 는 상수컬러
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);

		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 0, 255, 0));

		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
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

HRESULT CBullet_Police_Laser::Reset_RenderState()
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

	return S_OK;
}

_float CBullet_Police_Laser::Laser_Angle()
{
	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMonster_Pos, vPlayer_Pos;
	vPlayer_Pos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vMonster_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	MonsterMatrix = PlayerMatrix - MonsterMatrix;
	m_vLastPos = MonsterMatrix;
	D3DXVec3Normalize(&MonsterMatrix, &MonsterMatrix);

	_float fDot = D3DXVec3Dot(&MonsterMatrix, &_float3(0.f, 0.f, 1.f));
	_float fAngle = acosf(fDot);

	if (vMonster_Pos.x < vPlayer_Pos.x)
		fAngle *= -1.f;

	return fAngle;
}

_float CBullet_Police_Laser::Laser_YAngle()
{
	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 PlayerMatrix;
	_float3 MonsterMatrix;
	PlayerMatrix = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	MonsterMatrix = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMonster_Pos, vPlayer_Pos;
	vPlayer_Pos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vMonster_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	MonsterMatrix = PlayerMatrix - MonsterMatrix;
	m_vLastPos = MonsterMatrix;
	D3DXVec3Normalize(&MonsterMatrix, &MonsterMatrix);

	_float fDot = D3DXVec3Dot(&MonsterMatrix, &_float3(0.f, 1.f, 0.f));
	_float fAngle = acosf(fDot);

	if (vMonster_Pos.y	< vPlayer_Pos.y)
		fAngle *= -1.f;

	return fAngle;
}

HRESULT CBullet_Police_Laser::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player"))
	{
		m_bdead = true;
	}


	return S_OK;
}

CBullet_Police_Laser* CBullet_Police_Laser::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CBullet_Police_Laser* pInstance = new CBullet_Police_Laser(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet_Police_Laser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBullet_Police_Laser::Clone(void* pArg)
{
	CBullet_Police_Laser* pInstance = new CBullet_Police_Laser(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBullet_Police_Laser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet_Police_Laser::Free()
{
	__super::Free();
}
