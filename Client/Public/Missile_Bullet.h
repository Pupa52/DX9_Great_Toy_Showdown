#pragma once
#include "Bullet.h"

BEGIN(Client)
class CMissile_Bullet final : public CBullet
{
private:
	CMissile_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMissile_Bullet(const CMissile_Bullet& Prototype);
	virtual ~CMissile_Bullet() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag) override;


private:
	virtual HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	HRESULT Find_Target();

private:
	_int m_iDamage = { 0 };
	_float m_fSpeed = { 0.f };

	_bool m_bActive = { false };

public:
	static CMissile_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
