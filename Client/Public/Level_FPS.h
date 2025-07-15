#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_FPS final : public CLevel
{
private:
	CLevel_FPS(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_FPS() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround();

public:
	static CLevel_FPS* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
