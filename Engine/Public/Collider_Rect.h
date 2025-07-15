#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Rect final : public CCollider
{
private:
	CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Rect(const CCollider_Rect& Prototype);
	virtual ~CCollider_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CCollider_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCollider_Rect* Clone(void* pArg) override;
	virtual void Free() override;
};

END