#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)

class CRoom final : public CGameObject
{
public:
	enum ROOM { ROOM_MAGIC, ROOM_MUSEUM, ROOM_PHONE, ROOM_PIERROT, ROOM_BACK, ROOM_DOOR, ROOM_GROUND, ROOM_ROBBY, ROOM_TOY, ROOM_END };
public:
	typedef struct
	{
		_float3		vPos;
		_uint		iRoom;
		_uint		iIndex;
		_float		fSize;
		_float		fCollSize;
		_bool		bColl;
		_bool		bLayDown;
		LEVELID		eLevelID;
	}ROOM_DESC;

private:
	CRoom(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRoom(const CRoom& Prototype);
	virtual ~CRoom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT OnCollision(CGameObject* pOther, const _wstring& strLayerTag);

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CTexture* m_pTexture = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };

	_uint			m_iTextureIndex = { 0 };
	_float			m_fSize = { 0.f };
	_float			m_fCollSize = { 0.f };
	_bool			m_bColl = { false };
	_bool			m_bLayDown = { false };
	ROOM			m_eRoom = { ROOM_END };
	LEVELID			m_eLevelID = { LEVEL_END };

public:
	static CRoom* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END