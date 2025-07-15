#include "stdafx.h"
#include "../Public/TileMgr.h"

#include "GameInstance.h"
#include "Tile.h"

#include "Layer.h"
#include "Room.h"
#include "Wall.h"
#include "Obstacle.h"

#include "Monster_Heart.h"
#include "Monster_Purple_Apple.h"
#include "Monster_Red_Apple.h"
#include "Monster_Som.h"

CTileMgr::CTileMgr(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CTileMgr::CTileMgr(const CTileMgr& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CTileMgr::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTileMgr::Initialize(void* pArg)
{
	m_tTileMgrDesc = *(static_cast<TILEMGR_DESC*>(pArg));

	_uint iNumX = m_tTileMgrDesc.iSizeX;
	_uint iNumZ = m_tTileMgrDesc.iSizeZ;
	
	m_vecTile.reserve(iNumX * iNumZ);
	m_vecObstacle.resize(iNumX * iNumZ, nullptr);
	m_vecObstacle.resize(iNumX * iNumZ, nullptr);

	for (_uint i = 0; i < iNumZ; i++)
	{
		for (_uint j = 0; j < iNumX; j++)
		{
			_uint iIndex = i * iNumZ + j;

			CTile::TILE_DESC TileDesc{};
			TileDesc.iTileIndex = iIndex;
			TileDesc.iTileNum = CTile::TILE_END;
			TileDesc.vPos = _float3(0.5f + _float(j), 1.f, 0.5f + _float(i));
			TileDesc.eLevelID = m_tTileMgrDesc.eLevelID;

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tTileMgrDesc.eLevelID,
				TEXT("Layer_Tile"), TEXT("Prototype_GameObject_Tile"), &TileDesc)))
				return E_FAIL;
		}
	}

	CLayer* pLayer = m_pGameInstance->Find_Layer(m_tTileMgrDesc.eLevelID, TEXT("Layer_Tile"));
	list<CGameObject*> Objects = pLayer->Get_Objects();
	for (auto& iter : Objects)
	{
		m_vecTile.emplace_back(dynamic_cast<CTile*>(iter));
		Safe_AddRef(iter);
	}

	if(LEVEL_MINIGAME != m_tTileMgrDesc.eLevelID)
	{
		if (FAILED(Load_Tile(m_tTileMgrDesc.eLevelID)))
			return E_FAIL;

		if (FAILED(Load_Room(m_tTileMgrDesc.eLevelID)))
			return E_FAIL;
	}

	if(LEVEL_LOBBY == m_tTileMgrDesc.eLevelID)
	{
		if (FAILED(Load_TileWall(m_tTileMgrDesc.eLevelID)))
			return E_FAIL;
	}

	if (LEVEL_GAMEPLAY == m_tTileMgrDesc.eLevelID)
	{
		if (FAILED(Set_Obstacle()))
			return E_FAIL;

		if (FAILED(SetUp_LandObject(m_tTileMgrDesc.eLevelID)))
			return E_FAIL;
	}
	if (LEVEL_MINIGAME == m_tTileMgrDesc.eLevelID)
	{
		for (auto& iter : m_vecTile)
		{
			iter->Set_CurrentTile(CTile::TILE_PIERROT_0);
		}
	}
	return S_OK;
}

_uint CTileMgr::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CTileMgr::Update(_float fTimeDelta)
{
}

void CTileMgr::Late_Update(_float fTimeDelta)
{
}

HRESULT CTileMgr::Render()
{
	return S_OK;
}

HRESULT CTileMgr::Change_SingleTile(_uint iTileIndex)
{
	m_vecTile[iTileIndex]->Set_CurrentTile(CTile::TILE_NORMAL);

	return S_OK;
}

HRESULT CTileMgr::Change_RangeTile(_float3 vPos)
{
	_uint iPosX = _uint(vPos.x);
	_uint iPosZ = _uint(vPos.z);

	if (iPosX < 3 || iPosZ < 3)
		return E_FAIL;
	if (iPosX > m_tTileMgrDesc.iSizeX - 3 || iPosZ > m_tTileMgrDesc.iSizeZ - 3)
		return E_FAIL;

	for (_uint i = iPosZ - 3; i <= iPosZ + 3; i++)
	{
		for (_uint j = iPosX - 3; j <= iPosX + 3; j++)
		{
			if (i == iPosZ - 3 || i == iPosZ + 3)
				if (j == iPosX - 3 || j == iPosX - 2 || j == iPosX + 2 || j == iPosX + 3) continue;
			if (i == iPosZ - 2 || i == iPosZ + 2)
				if (j == iPosX - 3 || j == iPosX + 3) continue;

			_uint iIndex = i * m_tTileMgrDesc.iSizeZ + j;
			
			_float3 _vPos = { _float(j) + 0.5f, 0.f, _float(i) + 0.5f };
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY,
				TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Tile_Rect"), (void*)&_vPos)))
				return E_FAIL;

			if (nullptr != m_vecObstacle[iIndex])
			{
				m_vecObstacle[iIndex]->Set_Dead();
				Safe_Release(m_vecObstacle[iIndex]);
				m_vecObstacle[iIndex] = nullptr;
			}

			if (m_vecTile[iIndex]->Get_CurrentTile() == CTile::TILE_END)
				m_vecTile[iIndex]->Set_CurrentTile(CTile::TILE_NORMAL);

			// 1/3 확률
			if ((rand() % 9) == 0)
				Spawn_Monster(j, i);
		}
	}

	CLayer* pLayer = m_pGameInstance->Find_Layer(m_tTileMgrDesc.eLevelID, TEXT("Layer_Obstacle"));
	
	for (_uint i = iPosZ - 4; i <= iPosZ + 4; i++)
	{
		for (_uint j = iPosX - 4; j <= iPosX + 4; j++)
		{
			_uint iIndex = i * m_tTileMgrDesc.iSizeZ + j;
			if (i == iPosZ - 4 || i == iPosZ + 4)
				if (j != iPosX - 1 && j != iPosX && j != iPosX + 1) continue;
			if (i == iPosZ - 3 || i == iPosZ + 3)
				if (j != iPosX - 2 && j != iPosX + 2) continue;
			if (i == iPosZ - 2 || i == iPosZ + 2)
				if (j != iPosX - 3 && j != iPosX + 3) continue;
			if (i == iPosZ - 1 || i == iPosZ || i == iPosZ + 1)
				if (j != iPosX - 4 && j != iPosX + 4) continue;
	
			if (CTile::TILE_END == m_vecTile[iIndex]->Get_CurrentTile() &&
				nullptr == m_vecObstacle[iIndex])
			{
				CObstacle::OBSTACLE_DESC tDesc{};
				tDesc.vPos = _float3{ _float(j) + 0.5f, 0.f, _float(i) + 0.5f };
				tDesc.iIndex = rand() % 28;
				tDesc.fSize = 1.f;
	
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tTileMgrDesc.eLevelID,
					TEXT("Layer_Obstacle"), TEXT("Prototype_GameObject_Obstacle"), (void*)&tDesc)))
					return E_FAIL;
	
				CGameObject* pObject = pLayer->Get_Objects().back();
				m_vecObstacle[iIndex] = dynamic_cast<CObstacle*>(pObject);
				Safe_AddRef(m_vecObstacle[iIndex]);
			}
		}
	}

	return S_OK;
}

HRESULT CTileMgr::Check_MoveTile(_float3 vPos)
{
	_uint iIndex = _uint(vPos.z) * m_tTileMgrDesc.iSizeZ + _uint(vPos.x);
	if (CTile::TILE_END == m_vecTile[iIndex]->Get_CurrentTile())
		return E_FAIL;

	return S_OK;
}

HRESULT CTileMgr::Spawn_Monster(_uint iX, _uint iZ)
{
	_int iMonsterRand = rand() % 4;
	_int iSpawnCount = 1;

	_float3 fMinPos = { (_float)iX, 0.f, (_float)iZ };
	_float fRandX = (rand() % 32767) / 10000.f * 0.8f;
	_float fRandZ = (rand() % 32767) / 10000.f * 0.8f;
	fMinPos.x += fRandX;
	fMinPos.z += fRandZ;

	m_tMonsterDesc.m_targetPos = fMinPos;
	switch (iMonsterRand)
	{
	case 0:
		for (size_t i = 0; i < iSpawnCount; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"),
				TEXT("Prototype_GameObject_Monster_Som"), &m_tMonsterDesc)))
				return E_FAIL;
		}
		break;
	case 1:
		for (size_t i = 0; i < iSpawnCount; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"),
				TEXT("Prototype_GameObject_Monster_Apple_Red"), &m_tMonsterDesc)))
				return E_FAIL;
		}
		break;
	case 2:
		for (size_t i = 0; i < iSpawnCount; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"),
				TEXT("Prototype_GameObject_Monster_Apple_Purple"), &m_tMonsterDesc)))
				return E_FAIL;
		}
		break;
	case 3:
		for (size_t i = 0; i < iSpawnCount; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"),
				TEXT("Prototype_GameObject_Monster_Heart"), &m_tMonsterDesc)))
				return E_FAIL;
		}
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CTileMgr::Change_Three(_float3 vPos, CTile::TILE_TYPE eTileType)
{
	_int iPosX = (_int)vPos.x;
	_int iPosZ = (_int)vPos.z;

	_int iLength = m_tTileMgrDesc.iSizeZ;
	for (_int i = iPosZ - 1; i <= iPosZ + 1; i++)
	{
		for (_int j = iPosX - 1; j <= iPosX + 1; j++)
		{
			if (i % iLength == 0 || j % iLength == 0) continue;
			if (i < 0 || j < 0) continue;
			_int iIndex = i * iLength + j;

			m_vecTile[iIndex]->Set_CurrentTile(eTileType);
		}
	}
	return S_OK;

}

_float3 CTileMgr::Search_NearTile(_float3 vPos, CTile::TILE_TYPE eTileIndex)
{
	_int iLength = m_tTileMgrDesc.iSizeZ;

	for (_int i = 0; i < 50; i++)
	{
		_int iRandX = rand() % iLength;
		_int iRandZ = rand() % iLength;
		_int iIndex = iRandZ * iLength + iRandX;

		if (eTileIndex != m_vecTile[iIndex]->Get_CurrentTile())
			return _float3(_float(iRandX), 0.f, _float(iRandZ));
	}

	return _float3(0.f, 0.f, 0.f);
}

HRESULT CTileMgr::Load_Tile(LEVELID eLevelID)
{
	HANDLE hFile = {};
	LPCWSTR pFilePath = L"";
	if (eLevelID == LEVEL_GAMEPLAY)
		pFilePath = L"../Bin/Tile.dat";
	else if (eLevelID == LEVEL_LOBBY)
		pFilePath = L"../Bin/Lobby_Tile.dat";
	else
		return E_FAIL;

	hFile = CreateFile(pFilePath,		// 파일 경로(이름을 포함)
		GENERIC_READ,			// 파일 접근 모드(GENERIC_READ : 읽기 전용)
		NULL,					// 공유 방식, 파일이 열려 있는 상태에서 다른 프로세서에서 오픈하고자 할 때 허용할 지에 대한 여부(NULL인 경우 공유하지 않음)
		NULL,					// 보안 모드(NULL인 경우 기본 보안 상태)
		OPEN_EXISTING,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기), (OPEN_EXISTING : 파일이 있을 때만 열기)
		FILE_ATTRIBUTE_NORMAL,  // 파일 속성(아무런 특수 속성이 없는 파일 생성)
		NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(우리는 사용 안하기 때문에 NULL)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}

	while (true)
	{
		CTile::TILE_DESC desc = {};
		DWORD		dwByte(0);	// 예외 처리 변수
		desc.eLevelID = eLevelID;
		ReadFile(hFile, &desc.vPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &desc.iTileIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &desc.iTileNum, sizeof(_int), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		_uint iIndex = _uint(desc.vPos.z) * m_tTileMgrDesc.iSizeX + _uint(desc.vPos.x);
		m_vecTile[iIndex]->Set_CurrentTile(CTile::TILE_TYPE(desc.iTileIndex * 2 + desc.iTileNum));
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTileMgr::Load_Room(LEVELID eLevelID)
{
	HANDLE hFile = {};
	LPCWSTR pFilePath = {};
	if (eLevelID == LEVEL_GAMEPLAY)
		pFilePath = L"../Bin/Room.dat";
	else if (eLevelID == LEVEL_LOBBY)
		pFilePath = L"../Bin/Lobby_Room.dat";
	else
		return E_FAIL;

	hFile = CreateFile(pFilePath,		// 파일 경로(이름을 포함)
		GENERIC_READ,			// 파일 접근 모드(GENERIC_READ : 읽기 전용)
		NULL,					// 공유 방식, 파일이 열려 있는 상태에서 다른 프로세서에서 오픈하고자 할 때 허용할 지에 대한 여부(NULL인 경우 공유하지 않음)
		NULL,					// 보안 모드(NULL인 경우 기본 보안 상태)
		OPEN_EXISTING,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기), (OPEN_EXISTING : 파일이 있을 때만 열기)
		FILE_ATTRIBUTE_NORMAL,  // 파일 속성(아무런 특수 속성이 없는 파일 생성)
		NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(우리는 사용 안하기 때문에 NULL)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}

	while (true)
	{
		CRoom::ROOM_DESC desc = {};
		DWORD		dwByte(0);	// 예외 처리 변수
		desc.eLevelID = eLevelID;
		ReadFile(hFile, &desc.vPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &desc.iIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &desc.fSize, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &desc.fCollSize, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &desc.iRoom, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &desc.bColl, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &desc.bLayDown, sizeof(_bool), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		desc.vPos.y += 1.f;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(eLevelID, TEXT("Layer_Room"), TEXT("Prototype_GameObject_Room"), &desc)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTileMgr::Load_TileWall(LEVELID eLevelID)
{
	HANDLE hFile = {};
	LPCWSTR pFilePath = {};
	if (eLevelID == LEVEL_GAMEPLAY)
		pFilePath = L"../Bin/Wall.dat";
	else if (eLevelID == LEVEL_LOBBY)
		pFilePath = L"../Bin/Lobby_Wall.dat";
	else
		return E_FAIL;

	hFile = CreateFile(pFilePath,		// 파일 경로(이름을 포함)
		GENERIC_READ,			// 파일 접근 모드(GENERIC_READ : 읽기 전용)
		NULL,					// 공유 방식, 파일이 열려 있는 상태에서 다른 프로세서에서 오픈하고자 할 때 허용할 지에 대한 여부(NULL인 경우 공유하지 않음)
		NULL,					// 보안 모드(NULL인 경우 기본 보안 상태)
		OPEN_EXISTING,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기), (OPEN_EXISTING : 파일이 있을 때만 열기)
		FILE_ATTRIBUTE_NORMAL,  // 파일 속성(아무런 특수 속성이 없는 파일 생성)
		NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(우리는 사용 안하기 때문에 NULL)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}

	while (true)
	{
		CWall::WALL_DESC desc = {};
		DWORD		dwByte(0);	// 예외 처리 변수
		desc.eLevelID = eLevelID;
		_int iWallType = {0};
		ReadFile(hFile, &desc.vPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &desc.iWallIndex, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &desc.iWallNum, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &iWallType, sizeof(_int), &dwByte, nullptr);
		desc.eWallType = CWall::WALL_TYPE(iWallType);
		if (0 == dwByte)
			break;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(eLevelID, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_Wall"), &desc)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTileMgr::SetUp_LandObject(LEVELID eLevelID)
{
	m_tMonsterDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	if (nullptr == m_tMonsterDesc.pTerrainTransform)
		return E_FAIL;

	m_tMonsterDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	if (nullptr == m_tMonsterDesc.pTerrainVIBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CTileMgr::Set_Obstacle()
{
	for (_uint i = 1; i < m_tTileMgrDesc.iSizeZ - 1; i++)
	{
		for (_uint j = 1; j < m_tTileMgrDesc.iSizeX - 1; j++)
		{
			_uint iIndex = i * m_tTileMgrDesc.iSizeX + j;
			if (CTile::TILE_END == m_vecTile[iIndex]->Get_CurrentTile())
				continue;

			// 오른쪽
			if (CTile::TILE_END == m_vecTile[iIndex + 1]->Get_CurrentTile() &&
				nullptr == m_vecObstacle[iIndex + 1])
			{
				CObstacle::OBSTACLE_DESC tDesc{};
				tDesc.vPos = _float3{ _float(j) + 0.5f + 1.f, 0.f, _float(i) + 0.5f};
				tDesc.iIndex = rand() % 28;
				tDesc.fSize = 1.f;

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tTileMgrDesc.eLevelID,
					TEXT("Layer_Obstacle"), TEXT("Prototype_GameObject_Obstacle"), (void*)&tDesc)))
					return E_FAIL;

				CLayer* pLayer = m_pGameInstance->Find_Layer(m_tTileMgrDesc.eLevelID, TEXT("Layer_Obstacle"));
				CObstacle* pObstacle = dynamic_cast<CObstacle*>(pLayer->Get_Objects().back());
				m_vecObstacle[iIndex + 1] = pObstacle;
				Safe_AddRef(pObstacle);
			}

			// 왼쪽
			if (CTile::TILE_END == m_vecTile[iIndex - 1]->Get_CurrentTile() &&
				nullptr == m_vecObstacle[iIndex - 1])
			{
				CObstacle::OBSTACLE_DESC tDesc{};
				tDesc.vPos = _float3{ _float(j) + 0.5f - 1.f, 0.f, _float(i) + 0.5f};
				tDesc.iIndex = rand() % 28;
				tDesc.fSize = 1.f;

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tTileMgrDesc.eLevelID,
					TEXT("Layer_Obstacle"), TEXT("Prototype_GameObject_Obstacle"), (void*)&tDesc)))
					return E_FAIL;

				CLayer* pLayer = m_pGameInstance->Find_Layer(m_tTileMgrDesc.eLevelID, TEXT("Layer_Obstacle"));
				CObstacle* pObstacle = dynamic_cast<CObstacle*>(pLayer->Get_Objects().back());
				m_vecObstacle[iIndex - 1] = pObstacle;
				Safe_AddRef(pObstacle);
			}

			// 아래
			if (CTile::TILE_END == m_vecTile[(i - 1) * m_tTileMgrDesc.iSizeZ + j]->Get_CurrentTile() &&
				nullptr == m_vecObstacle[(i - 1) * m_tTileMgrDesc.iSizeZ + j])
			{
				CObstacle::OBSTACLE_DESC tDesc{};
				tDesc.vPos = _float3{ _float(j) + 0.5f, 0.f, _float(i) + 0.5f - 1.f};
				tDesc.iIndex = rand() % 28;
				tDesc.fSize = 1.f;

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tTileMgrDesc.eLevelID,
					TEXT("Layer_Obstacle"), TEXT("Prototype_GameObject_Obstacle"), (void*)&tDesc)))
					return E_FAIL;

				CLayer* pLayer = m_pGameInstance->Find_Layer(m_tTileMgrDesc.eLevelID, TEXT("Layer_Obstacle"));
				CObstacle* pObstacle = dynamic_cast<CObstacle*>(pLayer->Get_Objects().back());
				m_vecObstacle[(i - 1) * m_tTileMgrDesc.iSizeZ + j] = pObstacle;
				Safe_AddRef(pObstacle);
			}

			// 위
			if (CTile::TILE_END == m_vecTile[(i + 1) * m_tTileMgrDesc.iSizeZ + j]->Get_CurrentTile() &&
				nullptr == m_vecObstacle[(i + 1) * m_tTileMgrDesc.iSizeZ + j])
			{
				CObstacle::OBSTACLE_DESC tDesc{};
				tDesc.vPos = _float3{ _float(j) + 0.5f, 0.f, _float(i) + 0.5f + 1.f};
				tDesc.iIndex = rand() % 28;
				tDesc.fSize = 1.f;

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_tTileMgrDesc.eLevelID,
					TEXT("Layer_Obstacle"), TEXT("Prototype_GameObject_Obstacle"), (void*)&tDesc)))
					return E_FAIL;

				CLayer* pLayer = m_pGameInstance->Find_Layer(m_tTileMgrDesc.eLevelID, TEXT("Layer_Obstacle"));
				CObstacle* pObstacle = dynamic_cast<CObstacle*>(pLayer->Get_Objects().back());
				m_vecObstacle[(i + 1) * m_tTileMgrDesc.iSizeZ + j] = pObstacle;
				Safe_AddRef(pObstacle);
			}
		}
	}

	return S_OK;
}

_uint CTileMgr::Tile_Count(CTile::TILE_TYPE eTileType)
{
	_uint iResult = 0;
	for (auto& iter : m_vecTile)
		if (eTileType == iter->Get_CurrentTile())
			iResult++;

	return iResult;
}

CTileMgr* CTileMgr::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTileMgr* pInstnace = new CTileMgr(pGraphic_Device);
	if (FAILED(pInstnace->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTileMgr"));
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

CGameObject* CTileMgr::Clone(void* pArg)
{
	CTileMgr* pInstnace = new CTileMgr(*this);
	if (FAILED(pInstnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTileMgr"));
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

void CTileMgr::Free()
{
	__super::Free();

	for (auto& iter : m_vecObstacle)
		Safe_Release(iter);
	m_vecObstacle.clear();

	for (auto& iter : m_vecTile)
		Safe_Release(iter);
	m_vecTile.clear();
}
