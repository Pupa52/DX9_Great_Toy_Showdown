#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

#include "GameInstance.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CAnimator;
END

BEGIN(Client)

class CEffect : public CBlendObject
{
public:
	typedef struct TAG_Effect_DESC : public CLandObject::LANDOBJECT_DESC
	{
		_int iHp{};
		_int iMaxHp{};

		_uint iAttack{};

		_uint iExp{};
		_uint iMaxExp{};

		_uint iLevel{};
		_float fAngle{};
		_float3 m_Pos{};
		_float3 m_targetPos{};
	}EFFECT_DESC;
protected:
	CEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float3	Get_EffectPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_EffectPos(_float3 _SetPos) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, _SetPos); }

protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CAnimator* m_pAnimatorCom = { nullptr };

protected:
	_bool					m_bdead = false;
	_float3					m_fScale = {0,0,0};
	_float					m_fFrame = {};

	_float					m_FrameCount = 0;
	EFFECT_DESC*	m_Edesc = {};
	_float					m_EffectCount = 0;

public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END