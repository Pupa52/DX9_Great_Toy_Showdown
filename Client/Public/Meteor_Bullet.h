#pragma once
#include "Bullet.h"

BEGIN(Client)
class CMeteor_Bullet final : public CBullet
{
private:
	CMeteor_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMeteor_Bullet(const CMeteor_Bullet& Prototype);
	virtual ~CMeteor_Bullet() = default;
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

private:
	_int m_iDamage = { 0 };
	_float m_fSpeed = { 0.f };

public:
	static CMeteor_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
