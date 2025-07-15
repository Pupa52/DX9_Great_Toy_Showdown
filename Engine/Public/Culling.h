#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CCulling final : public CBase
{
private:
	CCulling(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CCulling() = default;

public:
	HRESULT Initialize();
	void Culling_Update(_float fDeltaTime);
	HRESULT is_Culling(class CTransform* pTransform, _float fOffset = 2.f);

private:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	
private:
	VTXTEX			m_CulVertices[8];
	VTXTEX			m_WorldVertices[8];
	D3DXPLANE		m_Plane[6];

public:
	static CCulling* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END