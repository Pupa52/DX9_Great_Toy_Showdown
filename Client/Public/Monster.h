#pragma once

#include "Actor.h"

BEGIN(Client)
class CBullet;

class CMonster abstract : public CActor
{
public:
	enum MONSTER_STATUE
	{
		MONSTER_STATE_DEFAULT,
		MONSTER_STATE_STUN,
		MONSTER_STATE_SLOW,
		MONSTER_STATE_END,
	};
	typedef struct tagMonsterDesc : public ACTOR_DESC
	{

	}MONSTER_DESC;
protected:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Monster_Statue(MONSTER_STATUE _eStatue) { m_Monster_Statue = _eStatue; }
	void Set_Hp(_int iHp) { m_tActorDesc.iHp = iHp; }
	void Set_Shadow() { m_bShadow = true; }
	void Set_Dead() { m_bdead = true; }
	void Set_Damage(_int _dmg) { m_iGet_dmg = _dmg; }

	_bool Get_Shadow() { return m_bShadow; }
	_bool Get_Dead() { return m_bDeadMotion; }
	_int Get_Damage() { return m_tActorDesc.iAttack; }
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	_int Get_Hp() { return m_tActorDesc.iHp; }
protected:
	CTransform* m_pPlayerTransform = { nullptr };

	MONSTER_STATUE m_Monster_Statue = MONSTER_STATE_DEFAULT;
	CGameObject* m_pShadow = { nullptr };

	_float m_fHitEffect_Time = 0.f;
	_float m_fAnim_Time = 0.f;
	_float MoveFrame = 0.f;
	_float m_fHittimer = 0.f;

	_bool m_bHitEffect = false;
	_bool m_bShadow = false;
	_bool m_bDeadMotion = false;
	_bool m_bdead = false;
	_bool m_bDangerous_Monster = false;
	_bool m_bStuned = false;
	_bool m_bDropExp = false;

	_float m_iSpeed = 1.f;
	_int m_iGet_dmg = 0;
	_int m_iFrameCounter = 0;
	_int m_FireTime = 0;
	
protected:
	virtual void Set_State();
private:
	virtual HRESULT Ready_Components();
	
public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END