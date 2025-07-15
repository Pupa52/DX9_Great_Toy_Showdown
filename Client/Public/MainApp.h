#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Ready_Prototype_Component_Static();
	HRESULT Open_Level(LEVELID eStartLevelID);

private:
	HRESULT Texture_Cat();
	HRESULT Texture_Dog();
	HRESULT Texture_Penguin();

	HRESULT Character_Skill();
	HRESULT Character_Bullet();
	HRESULT Effect();
	HRESULT Object();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END