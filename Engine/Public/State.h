#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CState abstract: public CBase
{
protected:
	CState();
	virtual ~CState() = default;

public:
	virtual HRESULT StartState() PURE;
	virtual void	Priority_Update(_float fTimeDelta) PURE;
	virtual void	Update(_float fTimeDelta) PURE;
	virtual void	Late_Update(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;
	virtual HRESULT ExitState() PURE;

protected:
	class CFSM* m_pFSM = { nullptr };

public:
	virtual void Free() override;
};
END

// IdleState
// MoveState