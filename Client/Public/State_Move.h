#pragma once

#include "Player_State.h"
#include "Client_Defines.h"

BEGIN(Client)
class CState_Move final : public CPlayer_State
{
private:
	CState_Move(class CPart** pParts, _uint iNumParts, class CTransform* pTransform, 
		_float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
		_int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer, LEVELID eLevelID);
	virtual ~CState_Move() = default;

public:
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT StartState() override;
	HRESULT ExitState() override;

public:
	static CState_Move* Create(class CPart** pParts, _uint iNumParts, class CTransform* pTransform, 
		_float& fFireDelay, _float3* pTargetPos, _uint iPlayerCharacter, _bool* pUseSkill, 
		_int* pHp, _int* iBulletCount, _uint* pLevel, _uint* pAttack, _int* iMaxHp, _int iPlayer = 0, LEVELID eLevelID = LEVEL_GAMEPLAY);
	virtual void Free() override;
};
END