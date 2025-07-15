#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Collision_Manager.h"
#include "Picking.h"
#include "Key_Manager.h"
#include "Sound_Manager.h"
#include "Font_Manager.h"
#include "Culling.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice);
	if (nullptr == m_pRenderer)
		return E_FAIL;


	/* 사운드 카드를 초기화하낟. */
	m_pSound_Manager = CSound_Manager::Create(EngineDesc.iMaxSoundChannel);
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	/* 입력장치를 초기화하낟. */
	m_pPicking = CPicking::Create(*ppDevice, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create();
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create(iNumLevels);
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pCulling = CCulling::Create(*ppDevice);
	if (nullptr == m_pCulling)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Priority_Update(fTimeDelta);

	/* View 행렬이 확정되는 뒤에 *///
	m_pPicking->Update();

	m_pCulling->Culling_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pCollision_Manager->Update();

	m_pLevel_Manager->Update(fTimeDelta);	
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);

	return S_OK;
}

void CGameInstance::Render_Begin()
{
	m_pGraphic_Device->Render_Begin();
}

void CGameInstance::Render_End(HWND hWnd)
{
	m_pGraphic_Device->Render_End(hWnd);
}

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);	
}

CLevel* CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

HRESULT CGameInstance::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Update(strTimerTag);
}

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject * pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

void CGameInstance::Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix)
{
	return m_pPicking->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::isPicked_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	return m_pPicking->isPicked_InWorldSpace(vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::Key_Pressing(int _iKey)
{
	return m_pKey_Manager->Key_Pressing(_iKey);
}

_bool CGameInstance::Key_Down(int _iKey, _uint keyBuffer)
{
	return m_pKey_Manager->Key_Down(_iKey, (CKey_Manager::KEYBUFFER)keyBuffer);
}

_bool CGameInstance::Key_Up(int _iKey)
{
	return m_pKey_Manager->Key_Up(_iKey);
}

void CGameInstance::SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume)
{
	return m_pSound_Manager->SoundPlay(pSoundKey, iChannelID, fVolume);
}

void CGameInstance::PlayBGM(TCHAR* pSoundKay, _uint iBGMChannel, _float fVolume)
{
	return m_pSound_Manager->PlayBGM(pSoundKay, iBGMChannel, fVolume);
}

void CGameInstance::StopSound(_uint iChannelID)
{
	return m_pSound_Manager->StopSound(iChannelID);
}

void CGameInstance::StopAll()
{
	return m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(_uint iChannelID, _float fVolume)
{
	return m_pSound_Manager->SetChannelVolume(iChannelID, fVolume);
}

void CGameInstance::LoadSoundFile()
{
	return m_pSound_Manager->LoadSoundFile();
}

HRESULT CGameInstance::Draw_Text(_uint iFontSize, const _wstring& strText, RECT& rt, D3DCOLOR tColor)
{
	return m_pFont_Manager->Draw_Text(iFontSize, strText, rt, tColor);
}

HRESULT CGameInstance::Draw_Text(_uint iFontSize, const _wstring& strText, _uint iX, _uint iY, D3DCOLOR tColor)
{
	return m_pFont_Manager->Draw_Text(iFontSize, strText, iX, iY, tColor);
}

HRESULT CGameInstance::is_Culling(CTransform* pTransform)
{
	return m_pCulling->is_Culling(pTransform);
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pCulling);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	__super::Free();



}
