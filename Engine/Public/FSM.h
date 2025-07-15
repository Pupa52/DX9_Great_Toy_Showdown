#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CFSM : public CComponent
{
private:
	CFSM(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFSM(const CFSM& Prototype);
	virtual ~CFSM() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

	HRESULT Change_State(class CState* _state);

private:
	class CState* m_pState = nullptr;

public:
	static CFSM* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFSM* Clone(void* pArg) override;
	virtual void Free();
};
END
