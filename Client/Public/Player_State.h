#pragma once

#include "State.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CTransform;
class CFSM;
class CGameInstance;
END

BEGIN(Client)
class CPlayer_State abstract : public CState
{
protected:
	CPlayer_State(class CPart** pParts, _uint iNumParts, class CTransform* pTransform, 
		_float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
		_int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID);
	virtual ~CPlayer_State() = default;

public:
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT StartState() override;
	HRESULT ExitState() override;

protected:
	HRESULT Fire_Bullet(_float fTimeDelta);
	HRESULT Use_Skill_GumBoom(_wstring strSkillLayerTag);
	HRESULT Use_Skill_Shield(_wstring strSkillLayerTag);
	HRESULT Use_Skill_CarrotBoom(_wstring strSkillLayerTag);

protected:
	Bullet_ID m_eBullet_Id = ID_END;
	class CPart** m_pParts = { nullptr };
	_uint m_iNumParts = { 0 };

	// 플레이어 위치 받아오는 용도.
	class CTransform* m_pTransformCom = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	// 발사 간격
	_float m_fFireLimit = { 0.f };
	_float& m_fFireDelay;
	// 발사 거리(총구 위치)
	_float m_fFireDistance = { 0.f };
	// 목표 위치(마우스 위치)
	_float3* m_pTargetPos = { nullptr };
	// 플레이어 캐릭터
	_uint m_iPlayerCharacter = { 0 };
	// 스킬 사용
	_bool* m_pUseSkill = { nullptr };
	// 레벨에 따른 스킬 랭크
	_uint m_iRank = { 0 };
	// 플레이어 체력
	_int* m_pHp = { nullptr };
	// 탄수
	_int* m_iBulletCount = { nullptr };
	// 레벨
	_uint* m_pLevel = { nullptr };
	// 데미지
	_uint* m_pAttack = { 0 };
	// 최대 체력
	_int* m_iMaxHp = { nullptr };
	// 스킬 레이어 태그
	_int m_iPlayer = { false };
	// 레벨 아이디
	LEVELID m_eLevelID = { LEVEL_END };

public:
	virtual void Free() override;
};
END