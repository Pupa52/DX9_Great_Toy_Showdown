#pragma once

#include "Item.h"

class CItem_Object final : public CItem
{
protected:
	CItem_Object(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem_Object(const CItem_Object& Prototype);
	virtual ~CItem_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);


private:
	virtual HRESULT Ready_Component();

private:
	ITEM_TYPE m_eItemType = { ITEM_END };

public:
	static CItem_Object* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

