#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Shadow final : public CEffect
{
private:
	CEffect_Shadow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Shadow(const CEffect_Shadow& Prototype);
	virtual ~CEffect_Shadow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Player() { m_bPlayer = true; }
	void Set_pMonster(CGameObject* _pMonster) { m_pMonster = _pMonster; }
	CGameObject* Get_pMonster() { return m_pMonster; }
private:
	HRESULT Find_Obj();
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
private:
	CGameObject* m_pMonster = { nullptr };

	_bool m_bPlayer = false;
	_bool m_bShadow = false;
	_float3* m_pos = {};
	_int RandPos = 0;
public:
	static CEffect_Shadow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END