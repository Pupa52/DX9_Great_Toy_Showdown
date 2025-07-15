#pragma once

#include "Bullet.h"

BEGIN(Client)

class CBoss_Ball final : public CBullet
{
private:
	CBoss_Ball(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Ball(const CBoss_Ball& Prototype);
	virtual ~CBoss_Ball() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	HRESULT Calculate_Target();
private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);
private:
	_bool m_bBallPush = false;
	_float m_fTimer = 0.f;
	_float m_Calculate_Range = 50.f;
	_float m_Power = 0;
	_float m_fDeltaTime = 0.f;

	_float3 m_TargetPos = {};
	_float3 m_vEnemyDistance = {};
	_float3 m_vAttackPos = {};
	_int RandCard= 0;

	_float m_fSpeed = 0;
public:
	static CBoss_Ball* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END