#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CDamage final : public CGameObject
{
public:
	typedef struct tagDamageDesc
	{
		_uint iDamage;
		_float3 vPos;
		D3DXCOLOR tColor = D3DCOLOR_ARGB(0, 0, 0, 0);
	}DAMAGE_DESC;

private:
	CDamage(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDamage(const CDamage& Prototype);
	virtual ~CDamage() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float2 Compute_WinPoint(_float3 vPos);

private:
	_uint m_iDamage = { 0 };
	_float3 m_vPos = {};
	D3DXCOLOR m_tColor = {};

	_float2 m_vPoint = {};

	_float m_fCheckTime = { 0.f };

	_bool m_bDead = { false };
public:
	static CDamage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
