#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer;
class CGameObject;
class CComponent;

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize(_uint iLevelIndex);
	void Update();

private:
	HRESULT Collision_Cube(int iLevelIndex, const _wstring& strSourceLayerTag, const _wstring& strDestLayerTag);
	_bool  Check_Collision_Cube(CGameObject* pSrc, CGameObject* pDst, CComponent* pSrcComopnent, CComponent* pDstComponent);

	HRESULT Collision_Sphere(int iLevelIndex, const _wstring& strSourceLayerTag, const _wstring& strDestLayerTag);
	_bool  Check_Collision_Sphere(CGameObject* pSrc, CGameObject* pDst, CComponent* pSrcComopnent, CComponent* pDstComponent);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint					m_iLevelIndex = {};

public:
	static CCollision_Manager* Create(_uint iLevelIndex);
	virtual void Free() override;
};

END