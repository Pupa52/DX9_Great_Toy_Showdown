#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CObstacle final : public CGameObject
{
public:
	typedef struct tagObstacleDesc
	{
		_float3		vPos;
		_uint		iIndex;
		_float		fSize;
	}OBSTACLE_DESC;

private:
	CObstacle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CObstacle(const CObstacle& Prototype);
	virtual ~CObstacle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Dead() { m_bDead = true; };

private:
	CTexture* m_pTexture = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
	_uint			m_iTextureIndex = { 0 };
	_bool			m_bDead = { false };
	
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CObstacle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END