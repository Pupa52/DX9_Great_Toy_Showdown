#pragma once

#include "Bullet.h"

BEGIN(Client)

class CLego_LandMine final : public CBullet
{
private:
	CLego_LandMine(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLego_LandMine(const CLego_LandMine& Prototype);
	virtual ~CLego_LandMine() = default;
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

private:
	_float m_fExplosiveDelay = 0.f;
protected:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	
public:
	static CLego_LandMine* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END