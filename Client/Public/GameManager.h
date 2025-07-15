#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CLayer;
END

BEGIN(Client)

class CGameManager final : public CGameObject
{
public:
	typedef struct tagGMDesc {
		PLAYER_CHARACTER ePlayerID;
		LEVELID	eLevelIndex;
	}GM_DESC;

private:
	CGameManager(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameManager(const CGameManager& Prototype);
	virtual ~CGameManager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Spawn_Player(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = {0.f, 0.f, 0.f});
	HRESULT Spawn_Bot(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = { 0.f, 0.f, 0.f });
	HRESULT Spawn_Enemy(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = { 0.f, 0.f, 0.f });

private:
	HRESULT Spawn_Pierrot(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = { 0.f, 0.f, 0.f });
	HRESULT Spawn_Telephone(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = { 0.f, 0.f, 0.f });
	HRESULT Spawn_Bird(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = { 0.f, 0.f, 0.f });
	HRESULT Spawn_Police(CLandObject::LANDOBJECT_DESC tLandObjectDesc, _float3 vSpawnPos = { 0.f, 0.f, 0.f });
private:
	_uint m_iBotCount = 1;
private:
	HRESULT Set_MonsterUI();

private:
	CLayer*	m_pEnemyLayer = { nullptr };
	GM_DESC			m_tGMDesc = {};
	_bool			m_bResult = { false };

public:
	static CGameManager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameManager* Clone(void* pArg) override;
	virtual void Free() override;
};

END