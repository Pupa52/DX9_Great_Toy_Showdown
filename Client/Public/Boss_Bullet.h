#pragma once

#include "Bullet.h"

BEGIN(Client)

class CBoss_Bullet final : public CBullet
{
private:
	CBoss_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Bullet(const CBoss_Bullet& Prototype);
	virtual ~CBoss_Bullet() = default;

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
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);
private:
	_int RandCard= 0;
public:
	static CBoss_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END