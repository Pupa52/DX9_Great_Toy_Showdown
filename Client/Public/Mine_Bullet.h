#pragma once
#include "Bullet.h"

BEGIN(Client)
class CMine_Bullet final : public CBullet
{
private:
	CMine_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMine_Bullet(const CMine_Bullet& Prototype);
	virtual ~CMine_Bullet() = default;
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

	void Boom();

private:
	_int m_iDamage = { 0 };
	_float m_fBoomCount = { 0.f };

public:
	static CMine_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
