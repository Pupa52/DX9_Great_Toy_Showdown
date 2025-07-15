#pragma once

#include "Bullet.h"

BEGIN(Client)

class CBullet_Police_Laser final : public CBullet
{
private:
	CBullet_Police_Laser(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet_Police_Laser(const CBullet_Police_Laser& Prototype);
	virtual ~CBullet_Police_Laser() = default;
	
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
	_float Laser_Angle();
	_float Laser_YAngle();
private:
	_bool m_bFireSound = true;
	_bool m_bFire = false;
	_float3 m_vLastPos = {};

	_float m_fFireSpeed = 10.f;
	_float m_fScale = 11.f;
private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

public:
	static CBullet_Police_Laser* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END