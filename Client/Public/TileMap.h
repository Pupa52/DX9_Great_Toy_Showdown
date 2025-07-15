#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTileMap final : public CGameObject
{
public:
	typedef struct
	{
		LEVELID eLevelID;
	}TILEMAP_DESC;

private:
	CTileMap(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTileMap(const CTileMap& Prototype);
	virtual ~CTileMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	LEVELID m_eLevelID = { LEVEL_END };

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();


public:
	static CTileMap* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END