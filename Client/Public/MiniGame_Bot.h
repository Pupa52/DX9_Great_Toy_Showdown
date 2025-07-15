#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Client)
class CMiniGame_Bot final : public CActor
{
public:
	enum PLAYER_PART { PART_BODY, PART_HEAD, PART_WEAPON, PART_END };

	typedef struct tagPlayerDesc : public LANDOBJECT_DESC
	{
		PLAYER_CHARACTER ePlayerID;
		Bullet_ID eBulletID;
		_float3 vStartPos;
	}MINIGAME_BOT_DESC;

private:
	CMiniGame_Bot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMiniGame_Bot(const CMiniGame_Bot& Prototype);
	virtual ~CMiniGame_Bot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

public:
	PLAYER_CHARACTER Get_Character() { return m_ePlayerID; }
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	_bool Get_Dead() { return m_bDead; }
	_int Get_Hp() { return m_tActorDesc.iHp; }

	void Set_Hp(_int iHp) { m_tActorDesc.iHp = iHp; }

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Add_Part();
	_float	Calculate_FireAngle();
	_float Compute_RadianAngle(_float3 vDir);

	void Set_State();
	HRESULT Player_Move(_float fTimeDelta);

private:
	Bullet_ID m_eBullet_ID = { ID_END };
	CTransform* m_pPlayerTransform = { nullptr };

	_bool m_bDead = false;
	class CPart* m_pParts[PART_END] = {};
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
	_float m_SkillDelay = 0.f;

	// 전체 장전수
	_int m_iTotalBulletCount = { 0 };
	// 잔탄수
	_int m_iBulletCount = { 0 };
	// 장전 시간
	_float m_fReloadTime = { 0.f };

	_bool m_bStun = { false };
	_bool m_bSlow = { false };
	_float m_fStunTime = { 0.f };

	_bool m_bFire = { false };
	_float m_fFireDistance = { 0.f };

	_float3 m_vMarkPos = {};
	_float m_fFindNearTime = { 0.f };

	
public:
	static CMiniGame_Bot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END