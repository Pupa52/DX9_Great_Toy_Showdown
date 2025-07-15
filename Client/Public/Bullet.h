#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "LandObject.h"

BEGIN(Engine)
class CAnimator;
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)

class CBullet abstract : public CGameObject
{
protected:
	enum BULLETTYPE	{ BULLETTYPE_RIFLE, BULLETTYPE_PISTOL, BULLETTYPE_SHOTGUN, BULLETTYPE_MONSTER, BULLETTYPE_BOSS_CARD, BULLETTYPE_END};
	
public:
	typedef struct TAG_BULLET_DESC : public CLandObject::LANDOBJECT_DESC
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
	}TAG_BULLET_DESC;

protected:
	CBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet(const CBullet& Prototype);
	virtual ~CBullet() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	TAG_BULLET_DESC* Get_Bullet_DESC() { return m_ptBullet_DESC; }
	_float3 Get_BulletPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_Dead() { m_bdead = true; }
	_bool Get_Ball() { return m_bBall; }
protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CAnimator* m_pAnimatorCom = { nullptr };
	CCollider_Cube* m_pColliderCubeCom = { nullptr };
	
	CTransform* m_pPlayerTransform = { nullptr };

protected:
	virtual HRESULT Ready_Components() = 0;

protected:
	Bullet_ID m_eBullet_Id = ID_END;
	_bool m_bdead = false;
	_bool m_bBall = false;

	int m_SpiretCount = 0;
	
	_float3 m_fTarget_Vector = {};
	_float3 m_OriginPos = {};

	_float3 m_vOtherPos = {};

	_float m_BulletRange = 0;

	_float m_Sprite_Time = 0;

	TAG_BULLET_DESC* m_ptBullet_DESC = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END