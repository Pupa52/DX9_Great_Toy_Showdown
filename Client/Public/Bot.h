#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Client)
class CBot final : public CActor
{
public:
	enum BOT_STATUE
	{
		BOT_STATE_DEFAULT,
		BOT_STATE_STUN,
		BOT_STATE_SLOW,
		BOT_STATE_END,
	};

	enum PLAYER_PART { PART_BODY, PART_HEAD, PART_WEAPON, PART_END };

	typedef struct tagPlayerDesc : public LANDOBJECT_DESC
	{
		PLAYER_CHARACTER ePlayerID;
		_float3 vStartPos;
	}PLAYER_DESC;

private:
	CBot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBot(const CBot& Prototype);
	virtual ~CBot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

public:
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_Bot_Statue(BOT_STATUE _m_Bot_Statue) { m_Bot_Statue = _m_Bot_Statue; }
	_bool Get_Dead() { return m_bDead; }

	_int Get_Hp() { return m_tActorDesc.iHp; }
	void Set_Hp(_int iHp) { m_tActorDesc.iHp = iHp; }

	void Set_ID(Bullet_ID _eBullet) { m_eBullet_ID = _eBullet; }
	Bullet_ID Get_ID() { return m_eBullet_ID; }

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Add_Part();
	_float	Calculate_MouseDir();
	_float	Calculate_FireAngle();

	void Set_State();
	HRESULT Player_Move(_float fTimeDelta);

private:
	Bullet_ID m_eBullet_ID = { ID_END };

	_bool m_bPanic = false;
	_bool m_bRun = false;
	_float m_fTimer = 0.f;
	CTransform* m_pPlayerTransform = { nullptr };

	_bool m_bDead = false;
	class CPart* m_pParts[PART_END] = {};
	_float3 m_vPartsDist[PART_END] = {};
	CFSM* m_pFSM = { nullptr };

	CGameObject* m_pShadow = { nullptr };
	BOT_STATUE m_Bot_Statue = BOT_STATE_DEFAULT;

	CCollider_Cube* m_pColliderCom = { nullptr };
	PLAYER_CHARACTER m_ePlayerID = {};

	PLAYER_STATE m_eCurState = { PLAYER_STATE::STATE_END };
	PLAYER_STATE m_ePreState = { PLAYER_STATE::STATE_END };

	_float m_fFireDelay = { 0.f };
	_float3 m_vTargetPos = {};
	_bool m_bUseSkill = { false };
	_float m_fSpeed = { 0.f };

	_float m_fHitDelay = { 0.f };
	// --Bot--
	_float3 m_BotResultTarget = { 60.f, 1.f, 60.f };
	_float3 m_EnemyDistance = { 1000.f, 200.f, 1000.f };
	
	_float m_fMapFinder = 0.f;
	_float m_SkillDelay = 0.f;
	_float m_fTrunTime = 0.f;

	_float m_Calculate_Angle = 0.f;
	_float Calculate_Range = 0.5f;

	_bool m_Detected_Enemy = { false };
	_bool m_Detected_Player = { false };
	_bool m_bAttack = { false };

	_int m_iGet_dmg = 0;

	_float m_vTempPos = {};
	_float Range = 1000;
	// 전체 장전수
	_int m_iTotalBulletCount = { 0 };
	// 잔탄수
	_int m_iBulletCount = { 0 };
	// 장전 시간
	_float m_fReloadTime = { 0.f };
	// 자기장 중점으로부터 거리
	_float m_fStormLength = { 0.f };


public:
	static CBot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END