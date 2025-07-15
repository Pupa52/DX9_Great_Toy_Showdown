#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CWall final : public CGameObject
{
public:
	enum TILE_TYPE {
		TILE_MAGIC_0, TILE_MAGIC_1,
		TILE_MUSEUM_0, TILE_MUSEUM_1,
		TILE_PHONE_0, TILE_PHONE_1,
		TILE_PIERROT_0, TILE_PIERROT_1,
		TILE_NORMAL, TILE_END
	};

	enum WALL_TYPE { WALL_FRONT, WALL_RIGHT, WALL_LEFT, WALL_END };

	typedef struct
	{
		_float3		vPos;
		// 몇번째 파일인지
		_int		iWallIndex;
		// 텍스처 번호
		_int		iWallNum;
		// 벽 타입(어케 새울 건지)
		WALL_TYPE	eWallType;
		LEVELID		eLevelID;
	}WALL_DESC;

private:
	CWall(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWall(const CWall& Prototype);
	virtual ~CWall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_CurrentTile(TILE_TYPE eType) { m_eCurWall = eType; };

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	TILE_TYPE	m_eCurWall = { TILE_END };
	WALL_TYPE	m_eWallType = { WALL_END };
	_uint		m_iWallNum = { 0 };
private:
	HRESULT Ready_Components();

public:
	static CWall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END