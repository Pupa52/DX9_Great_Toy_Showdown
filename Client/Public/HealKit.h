#pragma once

#include "Item.h"

class CHealkit : public CItem
{
private:
	CHealkit(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHealkit(const CHealkit& Prototype);
	virtual ~CHealkit() = default;

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
	ITEM_TYPE m_eItemType = { ITEM_END };
	ITEM_DESC* m_pDesc = {};

	CTransform* m_pPlayerTransform = { nullptr };

	_float m_fSpeed = 1.0f;

	_int	m_iRandCount = 0;
	_int	m_iBounceCount = 2;


	_float3 m_vPos = {};
	_float m_fGravity = 9.6f;
	_float m_fDeltaCount = 0;
	_float m_fAirTime = 0.1f;
	_bool m_bCanGetExp = false;
private:
	virtual HRESULT Ready_Component();

public:
	static CHealkit* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

