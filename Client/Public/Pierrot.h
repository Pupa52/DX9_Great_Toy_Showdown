#pragma once

#include "Monster.h"

BEGIN(Client)

class CPierrot final : public CMonster
{
private:
	enum PIERROT_STATE {
		PIERROT_IDLE,
		PIERROT_CARDREADY,
		PIERROT_CARDSHFFLE,
		PIERROT_CARDTHROW,
		PIERROT_CARDEND,
		PIERROT_GHOSTIDLE,
		PIERROT_TRANSFORM,
		PIERROT_PAPERPLANE_READY,
		PIERROT_PAPERPLANE_END,
		PIERROT_PAPERPLANE_REST,
		PIERROT_PAPERPLANE_SUMMON,
		PIERROT_JUGGLING_END,
		PIERROT_JUGGLING_READY,
		PIERROT_JUGGLING_JUGGLING,
		PIERROT_JUMP_START,
		PIERROT_JUMP_LAND,
		PIERROT_END,
		PIERROT_DIE
	};

	enum PIERROT_BULLET {
		PIERROT_BULLET_Paper,
		PIERROT_BULLET_Ball,
		PIERROT_BULLET_Card,
		PIERROT_BULLET_END
	};
private:
	CPierrot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPierrot(const CPierrot& Prototype);
	virtual ~CPierrot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);
	HRESULT Calculate_Target();
private:
	HRESULT Fire_Bullet(_float FtimeDelta, PIERROT_BULLET _m_Bullet);
	_float Fire_BulletPlayerAngle();
private:
	CTransform* m_pPlayerTransform;
	_float m_Calculate_Range = 30.f;
	_float3 m_vEnemyDistance = {};
	_float3 m_vTargetPos = {};

	_bool m_bGround = true;
	_bool m_bWake = false;
	_bool m_bAnimChange_Ready = false;
	_bool m_bLandingReady = false;
	_float m_fDieTimer = 0; 

	_float3 m_vAttackPos = {};
	PIERROT_STATE m_pState = PIERROT_GHOSTIDLE;
	PIERROT_BULLET m_Bullet = PIERROT_BULLET_END;

public:
	static CPierrot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END