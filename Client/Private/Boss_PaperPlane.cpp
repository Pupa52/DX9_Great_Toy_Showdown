#include "stdafx.h"
#include "..\Public\Boss_PaperPlane.h"

#include "Player.h"

CBoss_PaperPlane::CBoss_PaperPlane(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBullet{ pGraphic_Device }
{
}

CBoss_PaperPlane::CBoss_PaperPlane(const CBoss_PaperPlane& Prototype)
	: CBullet{ Prototype }
{
}


HRESULT CBoss_PaperPlane::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_PaperPlane::Initialize(void* pArg)
{
	m_ptBullet_DESC = static_cast<TAG_BULLET_DESC*>(pArg);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_ptBullet_DESC->m_Pos.x, m_ptBullet_DESC->m_Pos.y, m_ptBullet_DESC->m_Pos.z));
	
	
	m_fTarget_Vector = m_ptBullet_DESC->m_Pos - m_ptBullet_DESC->m_targetPos;
	_float3  _NvecTargetpos = m_ptBullet_DESC->m_targetPos;
	_float3  _Nvecpos =  m_ptBullet_DESC->m_Pos;
	_float Rad = atan2f(m_ptBullet_DESC->m_targetPos.z - m_ptBullet_DESC->m_Pos.z, m_ptBullet_DESC->m_targetPos.x - m_ptBullet_DESC->m_Pos.x);
	
	_float _Rad = (m_ptBullet_DESC->m_targetPos.x * m_ptBullet_DESC->m_Pos.x + m_ptBullet_DESC->m_targetPos.z * m_ptBullet_DESC->m_Pos.z);

	D3DXVec3Normalize(&_NvecTargetpos, &_NvecTargetpos);
	D3DXVec3Normalize(&_Nvecpos, &_Nvecpos);

	//_float _Rad = (_NvecTargetpos.x * _Nvecpos.x + _NvecTargetpos.z * _Nvecpos.z);

	//m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_ptBullet_DESC->fAngle));

	m_pTransformCom->Rotation(_float3(0.f, 0.f, 1.f), D3DXToRadian(m_ptBullet_DESC->fAngle));
	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));

	m_pAnimatorCom->Change_Texture(TEXT("PaperPlane_Bullet"));

	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Scaling(0.5f, 0.5f, 0.5f);
	m_pColliderCubeCom->Scaling(1.f, 1.f, 1.f);
	m_BulletRange = 3.5f;
	return S_OK;
}

_uint CBoss_PaperPlane::Priority_Update(_float fTimeDelta)
{
	if (m_bdead == true)
	{
		return OBJ_DEAD;
	}

	_float fResult, fr;
	fResult = powf(m_OriginPos.x - m_pTransformCom->Get_State(CTransform::STATE_POSITION).x, 2) +
		powf(m_OriginPos.z - m_pTransformCom->Get_State(CTransform::STATE_POSITION).z , 2);

	fr = (_float)pow(m_BulletRange + 0.5f, 2);

	if (fResult >= fr)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CBoss_PaperPlane::Update(_float fTimeDelta)
{
	m_Sprite_Time = fTimeDelta;
	m_ptBullet_DESC->m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	D3DXVec3Normalize(&m_fTarget_Vector, &m_fTarget_Vector);

	m_ptBullet_DESC->m_Pos = m_ptBullet_DESC->m_Pos - m_fTarget_Vector * 2 * fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ptBullet_DESC->m_Pos);

//	m_pTransformCom->Go_Straight(fTimeDelta);
//	m_pTransformCom->Go_Up(fTimeDelta);
}

void CBoss_PaperPlane::Late_Update(_float fTimeDelta)
{
	m_pColliderCubeCom->Update_Collider(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pAnimatorCom->Move_Frame(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_PaperPlane::Render()
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

HRESULT CBoss_PaperPlane::Ready_Components()
{

	/* For.Com_Animator*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"),
		TEXT("Com_Animator"), reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
		return E_FAIL;


	m_pAnimatorCom->Add_Texture(TEXT("PaperPlane_Bullet"),
		static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PaperPlane"))));

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

HRESULT CBoss_PaperPlane::SetUp_RenderState()
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

HRESULT CBoss_PaperPlane::Reset_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBoss_PaperPlane::OnCollision(CGameObject* pOther, const _wstring& strLayerTag)
{
	if (strLayerTag == TEXT("Layer_Player"))
	{
		m_bdead = true;
	}


	return S_OK;
}

CBoss_PaperPlane* CBoss_PaperPlane::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	
	CBoss_PaperPlane* pInstance = new CBoss_PaperPlane(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_PaperPlane"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CBoss_PaperPlane::Clone(void* pArg)
{
	CBoss_PaperPlane* pInstance = new CBoss_PaperPlane(*this);
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_PaperPlane"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_PaperPlane::Free()
{
	__super::Free();
}
