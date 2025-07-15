#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)
class CSkill abstract : public CBlendObject
{
public:
	typedef struct
	{
		_uint iRank;
		_float3 vTargetPos;
		_float3 vPos;
		LEVELID eLevelID;
	}SKILL_DESC;
protected:
	CSkill(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkill(const CSkill& Prototype);
	virtual ~CSkill() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	_uint Get_Attack() { return m_iAttack; }
	_bool Get_Active() { return m_bActive; }
	PLAYER_CHARACTER Get_Character() { return m_eCharacter; }

protected:
	int GetRandomNum(int _first, int _second);

protected:
	class CTransform*		m_pTransformCom = { nullptr };
	class CTexture*			m_pTextureCom = { nullptr };
	class CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };
	class CCollider*		m_pColliderCom = { nullptr };
	
	CTransform* m_pPlayerTransform = { nullptr };

	// 범위
	_float m_fEffectArea = { 0.f };
	// 스킬 랭크
	_uint m_iRank = { 0 };
	// 목표 위치
	_float3 m_vTargetPos = {};
	// 발동 여부
	_bool m_bActive = { false };
	// 발사 방향
	_float3 m_vTargetDir = {};
	// 데미지
	_uint m_iAttack = { 0 };
	// 레벨
	LEVELID m_eLevelID = { LEVEL_END };
	// 캐릭터
	PLAYER_CHARACTER m_eCharacter = { PLAYER_CHARACTER::PLAYER_END };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
