#pragma once

#include "Renderer.h"
#include "Component_Manager.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);
	
#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();
	void Render_End(HWND hWnd = nullptr);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	CLevel* Get_CurrentLevel();
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
#pragma endregion

#pragma region PICKING
	void Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix);
	_bool isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool isPicked_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
#pragma endregion

#pragma region KEY_MANAGER
	_bool	Key_Pressing(int _iKey);
	_bool	Key_Down(int _iKey, _uint keyBuffer = 0);
	_bool	Key_Up(int _iKey);
#pragma endregion

#pragma region SOUND_MANGER
	void SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume);
	void PlayBGM(TCHAR* pSoundKay, _uint iBGMChannel, _float fVolume);
	void StopSound(_uint iChannelID);
	void StopAll();
	void SetChannelVolume(_uint iChannelID, _float fVolume);
	void LoadSoundFile();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Draw_Text(_uint iFontSize, const _wstring& strText, RECT& rt, D3DCOLOR tColor);
	HRESULT Draw_Text(_uint iFontSize, const _wstring& strText, _uint iX, _uint iY, D3DCOLOR tColor);
#pragma endregion

#pragma region CULLING
	HRESULT is_Culling(class CTransform* pTransform);
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CCulling*					m_pCulling = { nullptr };

public:	
	void Release_Engine();
	virtual void Free() override;
};

END