#pragma once

#include "Bullet.h"

BEGIN(Client)

class CBoss_PaperPlane final : public CBullet
{
private:
	CBoss_PaperPlane(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_PaperPlane(const CBoss_PaperPlane& Prototype);
	virtual ~CBoss_PaperPlane() = default;
	
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

public:
	static CBoss_PaperPlane* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END