#pragma once
#include "Actor.h"

BEGIN(Client)
class CScanner final : public CActor
{
private:
	enum SCANNER_STATE { SCANNER_START,
		SCANNER_FLYING_START,
		SCANNER_FLYING_IDLE,
		SCANNER_FLYING_END,
		SCANNER_END_START,
	};
private:
	CScanner(LPDIRECT3DDEVICE9 pGraphic_Device);
	CScanner(const CScanner& Prototype);
	virtual ~CScanner() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	ACTOR_DESC Get_ActorDesc() const { return m_tActorDesc; };


private:
	virtual HRESULT Ready_Components() override;
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CTransform* m_pPlayerTransform = { nullptr };

	SCANNER_STATE m_eScanner_State = SCANNER_START;

	_bool m_bDead = false;
	_bool m_bCanScan = false;
	_float m_fFrameCount = 0;
	_float m_fDeltaCount = 0;

	_float3 m_pos = {};
public:
	static CScanner* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END


