#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Client)
class CFriend_Bot final : public CActor
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
	CFriend_Bot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFriend_Bot(const CFriend_Bot& Prototype);
	virtual ~CFriend_Bot() = default;

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


	class CPart* m_pParts[PART_END] = {};

	_float3 m_vPartsDist[PART_END] = {};

	CGameObject* m_pShadow = { nullptr };

	CFSM* m_pFSM = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
	PLAYER_CHARACTER m_ePlayerID = {};

	PLAYER_STATE m_eCurState = { PLAYER_STATE::STATE_END };
	PLAYER_STATE m_ePreState = { PLAYER_STATE::STATE_END };

	_float m_fFireDelay = { 0.f };
	_float3 m_vTargetPos = {};
	_bool m_bUseSkill = { false };
	_float m_fSpeed = { 0.f };

	_float m_fHitDelay = { 0.f };
	//스킬 쿨타임
	_float m_fCoolTime = { 0.f };
	_bool m_bDead = { false };

	// 총 범위 스케일링 테스트
	_float m_fScaling = { 0.f };
	// 전체 탄창수
	_int m_iTotalBulletCount = { 0 };
	// 잔탄수
	_int m_iBulletCount = { 0 };
	// 장전 시간
	_float m_fReloadTime = { 0.f };
	// --Bot--
	CTransform* m_pPlayerTransform = { nullptr };

	_float3 m_BotResultTarget = { 60.f, 1.f, 60.f };
	_float3 m_EnemyDistance = { 200.f, 200.f, 200.f };

	_float m_fMapFinder = 0.f;
	_float m_SkillDelay = 0.f;
	_float m_fTrunTime = 0.f;

	_float m_Calculate_Angle = 0.f;
	_float Calculate_Range = 0.5f;

	_bool m_Detected_Player = { false };
	_bool m_Detected_Enemy = { false };
	_bool m_Detected_EnemyBot = { false };
	_bool m_Follow_Player = { false };
	_bool m_bAttack = { false };

public:
	static CFriend_Bot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END