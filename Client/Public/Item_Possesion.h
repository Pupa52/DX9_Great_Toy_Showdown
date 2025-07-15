#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CTexture;
END

BEGIN(Client)
class CItem_Possesion abstract : public CBase
{
protected:
	CItem_Possesion(class CPlayer* pPlayer);
	virtual ~CItem_Possesion() = default;

public:
	virtual HRESULT Initialize();
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

protected:
	virtual HRESULT Ready_Component() = 0;
	int GetRandomNum(int _first, int _second);

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	class CPlayer* m_pPlayer = { nullptr };

public:
	virtual void Free() override;
};
END
