#pragma once

#include "Tile.h"
#include "Monster.h"

BEGIN(Client)
class CTileMgr final : public CGameObject
{
public:
	typedef struct tagTileMgrDesc {
		_uint iSizeX; // 타일 가로 (x축) 갯수
		_uint iSizeZ; // 타일 세로 (z축) 갯수
		LEVELID eLevelID;
	}TILEMGR_DESC;

private:
	CTileMgr(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTileMgr(const CTileMgr& Prototype);
	virtual ~CTileMgr() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Change_SingleTile(_uint iTileIndex);
	HRESULT Change_RangeTile(_float3 vPos);
	HRESULT Check_MoveTile(_float3 vPos);

	HRESULT Spawn_Monster(_uint iX ,_uint iZ);
	HRESULT Change_Three(_float3 vPos, CTile::TILE_TYPE eTileType);
	_float3 Search_NearTile(_float3 vPos, CTile::TILE_TYPE eTileIndex);
	_uint Tile_Count(CTile::TILE_TYPE eTileType);

private:
	HRESULT Load_Tile(LEVELID eLevelID);
	HRESULT Load_Room(LEVELID eLevelID);
	HRESULT Load_TileWall(LEVELID eLevelID);
	HRESULT SetUp_LandObject(LEVELID eLevelID);

	HRESULT Set_Obstacle();

private:
	vector<class CTile*>				m_vecTile;
	vector<class CObstacle*>			m_vecObstacle;
	TILEMGR_DESC						m_tTileMgrDesc = {};
	CMonster::MONSTER_DESC				m_tMonsterDesc = {};

public:
	static CTileMgr* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END