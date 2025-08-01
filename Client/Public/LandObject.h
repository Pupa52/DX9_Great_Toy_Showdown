#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct tagLandObjectDesc
	{
		CVIBuffer_Terrain* pTerrainVIBuffer = { nullptr };
		CTransform* pTerrainTransform = { nullptr };
	}LANDOBJECT_DESC;
protected:
	CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLandObject(const CLandObject& Prototype);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY = 0.f);
	
protected:
	CVIBuffer_Terrain* m_pTerrainVIBuffer = { nullptr };
	CTransform* m_pTerrainTransform = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END