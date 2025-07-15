#pragma once

#include "UI.h"

BEGIN(Engine)
class CAnimator;
END

BEGIN(Client)

class CWinResult_Char final : public CUI
{
private:
	CWinResult_Char(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWinResult_Char(const CWinResult_Char& Prototype);
	virtual ~CWinResult_Char() = default;

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
	CAnimator* m_pAnimatorCom = { nullptr };
	_float m_fSpeed = { 0.f };
	PLAYER_CHARACTER m_ePlayerID = { PLAYER_CHARACTER::PLAYER_END };

public:
	static CWinResult_Char* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END