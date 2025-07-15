#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

#include "GameInstance.h"

BEGIN(Client)

class CTelephone_Meteor_Bullet final : public CBullet
{
private:
	CTelephone_Meteor_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTelephone_Meteor_Bullet(const CTelephone_Meteor_Bullet& Prototype);
	virtual ~CTelephone_Meteor_Bullet() = default;
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

protected:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
public:
	static CTelephone_Meteor_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END