#pragma once

#include "UI.h"

BEGIN(Engine)
class CAnimator;
END

BEGIN(Client)

class CWinResult_BG final : public CUI
{
private:
	CWinResult_BG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWinResult_BG(const CWinResult_BG& Prototype);
	virtual ~CWinResult_BG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components();

private:
	_float m_fDeadTime = { 0.f };
	_float m_fSpeed = { 0.f };
	CAnimator* m_pAnimatorCom = { nullptr };

public:
	static CWinResult_BG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END