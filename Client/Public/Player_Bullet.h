#pragma once

#include "Bullet.h"

BEGIN(Client)

class CPlayer_Bullet final : public CBullet
{
public:
	typedef struct tagPlayerBulletDesc : public TAG_BULLET_DESC
	{
		_uint iPlayerCharacter;
		LEVELID eLevelID;
		Bullet_ID eBulletID;
	}PLAYER_BULLET_DESC;

protected:
	CPlayer_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer_Bullet(const CPlayer_Bullet& Prototype);
	virtual ~CPlayer_Bullet() = default;
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	//임시추가
public:
	void Set_Dead() { m_bDead = true; }
	_int Get_Player_mDamage() { return m_iDamage; }
	Bullet_ID Get_BulletID() { return m_eBullet_Id; }

private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

protected:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected:
	_bool	m_bDead = { false };
	_float m_fMoveDistance = { 0.f };
	_float m_fMaxDistance = { 0.f };
	_float m_fSpeed = { 0.f };
	_float m_fAngle = { 0.f };
	_uint m_iBulletNum = { 0 };
	_int m_iDamage = { 0 };

	BULLETTYPE m_eBulletType = { BULLETTYPE_END };
	LEVELID m_eLevelID = { LEVEL_END };
	
public:
	static CPlayer_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END