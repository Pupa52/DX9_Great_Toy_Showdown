#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cylinder final : public CVIBuffer
{
private:
	CVIBuffer_Cylinder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Cylinder(const CVIBuffer_Cylinder& Prototype);
	virtual ~CVIBuffer_Cylinder() = default;

public:
	virtual HRESULT Initialize_Prototype(_float fBottomRadius, _float fTopRadius, _float fHeight, int iSliceCount);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Cylinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _float fBottomRadius, _float fTopRadius, _float fHeight, int iSliceCount);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END