#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)
class CPlayer final : public CActor
{
public:
	enum PLAYER_PART { PART_BODY, PART_HEAD, PART_WEAPON, PART_END };

	typedef struct tagPlayerDesc : public LANDOBJECT_DESC
	{
		PLAYER_CHARACTER ePlayerID;
		_float3 vStartPos;
		LEVELID eLevel;
	}PLAYER_DESC;

private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

public:
	HRESULT Add_Item(wstring strItemKey, class CItem_Possesion* pItem_Possesion);

public:
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	PLAYER_CHARACTER Get_Character() { return m_ePlayerID; }

	// 페시브 아이템으로 조정.
	ACTOR_DESC Get_Player_Desc() { return m_tActorDesc; }
	_uint Get_Attack() { return m_tActorDesc.iAttack; }
	_float Get_Speed() { return m_fSpeed; }
	_int Get_TotalBulletCount() { return m_iTotalBulletCount; }
	_int Get_BulletCount() { return m_iBulletCount; }
	_float3 Get_TargetPos() { return m_vTargetPos; }
	_bool Get_Reloading() { return m_bReloading; }
	_bool Get_Dead() { return m_bDead; }
	_float Get_SkillCoolTime() { return m_fRealCoolTime; }
	_float Get_CurSkillCoolTime() { return m_fCoolTime; }

	void Set_Exp(_uint iexp) { m_tActorDesc.iExp += iexp; }
	void Set_Attack(_uint iAttack) { m_tActorDesc.iAttack = iAttack; }
	void Set_Hp(_uint iHp) { m_tActorDesc.iHp += iHp; }
	void Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	void Set_TotalBulletCount(_int iTotalBulletCount) { m_iTotalBulletCount = iTotalBulletCount; }

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Add_Part();
	_float	Calculate_MouseDir();
	void Set_State();
	HRESULT Player_Move(_float fTimeDelta);
	void Compute_StormLength();

private:
	_bool m_bDead = { false };
	class CPart* m_pParts[PART_END] = {};
	_float3 m_vPartsDist[PART_END] = {};
	CGameObject* m_pShadow = { nullptr };

	CFSM* m_pFSM = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
	PLAYER_CHARACTER m_ePlayerID = {};

	PLAYER_STATE m_eCurState = { PLAYER_STATE::STATE_END };
	PLAYER_STATE m_ePreState = { PLAYER_STATE::STATE_END };

	// 발사 딜레이
	_float m_fFireDelay = { 0.f };
	// 목표(마우스) 위치
	_float3 m_vTargetPos = {};
	// 스킬 사용
	_bool m_bUseSkill = { false };
	// 이동 속도
	_float m_fSpeed = { 0.f };
	// 피격 간격
	_float m_fHitDelay = { 0.f };
	//스킬 쿨타임 확인용
	_float m_fCoolTime = { 0.f };
	// 총 범위 스케일링 테스트
	_float m_fScaling = { 0.f };
	// 전체 탄창수
	_int m_iTotalBulletCount = { 0 };
	// 잔탄수
	_int m_iBulletCount = { 0 };
	// 장전 시간
	_float m_fReloadTime = { 0.f };
	// 장전 진행중
	_bool m_bReloading = { false };
	// 스킬 쿨타임
	_float m_fRealCoolTime = { 0.f };
	// 타일 깔기 쿨타임
	_float m_fTileCoolTime = { 0.f };
	// 타일 깔기 쿨타임 확인
	_float m_fTileCheckTime = { 0.f };
	// 자기장 중점으로부터 거리
	_float m_fStormLength = { 0.f };
	// 무슨 레벨인지
	LEVELID m_eLevel = { LEVEL_GAMEPLAY };
	// 스턴 시간
	_float m_fStunTime = { 0.f };
	// 스턴 여부
	_bool m_bStun = { false };
	// 슬로우 여부
	_bool m_bSlow = { false };
	// 아이템 저장용 맵
	map<wstring, class CItem_Possesion*> m_Items;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END