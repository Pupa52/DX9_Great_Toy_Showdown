#include "stdafx.h"
#include "..\Public\Lego_LandMine.h"

#include "Effect_LandMine_Explosion.h"
#include "Player.h"

CLego_LandMine::CLego_LandMine(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CLego_LandMine::CLego_LandMine(const CLego_LandMine& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CLego_LandMine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLego_LandMine::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);

	m_pAnimatorCom->Change_Texture(TEXT("Lego_LandMine"));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, 1.65f, m_ptBullet_DESC->m_Pos.z));
	m_pTransformCom->Scaling(.5f, .5f, .5f);
	
	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	return S_OK;
}

_uint CLego_LandMine::Priority_Update(_float fTimeDelta)
{

	if (m_bdead == true)
	{
		m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_EXPLOSIVE));
		m_pGameInstance->SoundPlay(L"Skill_Explosion.wav", _uint(CHANNELID::SOUND_EXPLOSIVE), .5f);
		return OBJ_DEAD;
	}

	if (m_fExplosiveDelay >= 1.8f)
	{
		m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		
		CEffect_LandMine_Explosion::EXPLOSION_DESC ptExplosion_Desc;
		ptExplosion_Desc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		ptExplosion_Desc.fScale = 3.f;

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Mine_Explosive"), &ptExplosion_Desc);
		m_bdead = true;
	}
	
	return OBJ_NOEVENT;
}

void CLego_LandMine::Update(_float fTimeDelta)
{
	m_fExplosiveDelay += fTimeDelta;

	if (fTimeDelta * 60 >= 1)
	{
		m_Sprite_Time++;
	}

	//m_pTransformCom->Go_Straight(fTimeDelta);
	//m_pTransformCom->Go_Up(fTimeDelta);
}

void CLego_LandMine::Late_Update(_float fTimeDelta)
{
	m_pAnimatorCom->Move_Frame(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLego_LandMine::Render()
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

	return S_OK;
}

HRESULT CLego_LandMine::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player")&& (m_Sprite_Time > 90.f))
	{
		static_cast<CPlayer*>(pOther)->m_tActorDesc.iHp -= m_ptBullet_DESC->iAttack;
		m_bdead = true;
	}

	return S_OK;
}

HRESULT CLego_LandMine::Ready_Components()
{

	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;


	m_pAnimatorCom->Add_Texture(TEXT("Lego_LandMine"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Lego_LandMine"))));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
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

HRESULT CLego_LandMine::SetUp_RenderState()
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

HRESULT CLego_LandMine::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CLego_LandMine* CLego_LandMine::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CLego_LandMine* pInstance = new CLego_LandMine(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLego_LandMine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CLego_LandMine::Clone(void* pArg)
{
	CLego_LandMine* pInstance = new CLego_LandMine(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLego_LandMine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLego_LandMine::Free()
{
	__super::Free();
}
