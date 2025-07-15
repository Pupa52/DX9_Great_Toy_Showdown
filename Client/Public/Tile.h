#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CTile final : public CGameObject
{
public:
	enum TILE_TYPE {	TILE_MAGIC_0, TILE_MAGIC_1,
						TILE_MUSEUM_0, TILE_MUSEUM_1,
						TILE_PHONE_0, TILE_PHONE_1,
						TILE_PIERROT_0, TILE_PIERROT_1,
						TILE_NORMAL, TILE_END };

	typedef struct
	{
		_float3		vPos;
		// 몇번째 파일인지
		_int		iTileIndex;
		// 텍스처 번호
		_int		iTileNum;
		LEVELID		eLevelID;
	}TILE_DESC;

private:
	CTile(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTile(const CTile& Prototype);
	virtual ~CTile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_CurrentTile(TILE_TYPE eType) { m_eCurTile = eType; };
	TILE_TYPE Get_CurrentTile() const { return m_eCurTile; }

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	TILE_TYPE	m_eCurTile = { TILE_END };
	_uint		m_iTileNum = { 0 };
	LEVELID		m_eLevelID = { LEVEL_END };

private:
	HRESULT Ready_Components();

public:
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END