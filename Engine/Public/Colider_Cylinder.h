#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CColider_Cylinder final : public CCollider
{
private:
	CColider_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CColider_Cylinder(const CColider_Cylinder& Prototype);
	virtual ~CColider_Cylinder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CColider_Cylinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END