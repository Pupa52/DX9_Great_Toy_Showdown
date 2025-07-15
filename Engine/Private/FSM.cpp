#include "FSM.h"
#include "State.h"


CFSM::CFSM(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CFSM::CFSM(const CFSM& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM::Initialize(void* pArg)
{

	return S_OK;
}

void CFSM::Priority_Update(_float fTimeDelta)
{
	m_pState->Priority_Update(fTimeDelta);
}

void CFSM::Update(_float fTimeDelta)
{
	m_pState->Update(fTimeDelta);
}

void CFSM::Late_Update(_float fTimeDelta)
{
	m_pState->Late_Update(fTimeDelta);
}

HRESULT CFSM::Render()
{
	if (FAILED(m_pState->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFSM::Change_State(CState* _pState)
{
	if(nullptr != m_pState)
	{
		if (FAILED(m_pState->ExitState()))
			return E_FAIL;
		Safe_Release(m_pState);
	}

	m_pState = _pState;

	if (FAILED(m_pState->StartState()))
		return E_FAIL;

	return S_OK;
}

CFSM* CFSM::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFSM* pInstance = new CFSM(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CFSM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFSM* CFSM::Clone(void* pArg)
{
	CFSM* pInstance = new CFSM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CFSM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM::Free()
{
	__super::Free();
	Safe_Release(m_pState);
}
