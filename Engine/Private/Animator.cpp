#include "Animator.h"

CAnimator::CAnimator(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent{ pGraphic_Device }
{
}

CAnimator::CAnimator(const CAnimator& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CAnimator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAnimator::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CAnimator::Add_Texture(const _wstring& strFrameKey, CTexture* pTextures)
{
    auto iter = m_pAnimations.find(strFrameKey);
    if (iter != m_pAnimations.end())
    {
        Safe_Release(pTextures);
        return E_FAIL;
    }

    if (TEXT("Pierrot_End" == strFrameKey))
        int stop = 0;

    m_pAnimations.emplace(strFrameKey, pTextures);

    return S_OK;
}

HRESULT CAnimator::Bind_Texture()
{
    if (nullptr == m_pCurTexture)
        return E_FAIL;

    return m_pCurTexture->Bind_Texture();
}

HRESULT CAnimator::Bind_Second_Texture()
{
    if (nullptr == m_pCurTexture)
        return E_FAIL;

    return m_pCurTexture->Bind_Second_Texture();
}

HRESULT CAnimator::Change_Texture(const _wstring& strFrameKey)
{
    auto iter = m_pAnimations.find(strFrameKey);
    if (iter == m_pAnimations.end())
        return E_FAIL;

    Safe_Release(m_pCurTexture);

    m_pCurTexture = (*iter).second;
    
    m_pCurTexture->Reset_Frame();

    Safe_AddRef(m_pCurTexture);

    return S_OK;
}

HRESULT CAnimator::Move_Frame(_float fTimeDelta)
{
    if (nullptr == m_pCurTexture)
        return E_FAIL;

    return m_pCurTexture->Move_Frame(fTimeDelta);
}

CAnimator* CAnimator::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CAnimator* pInstance = new CAnimator(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CAnimator Prototype"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CAnimator::Clone(void* pArg)
{
    CAnimator* pInstance = new CAnimator(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAnimator"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimator::Free()
{
    __super::Free();

    Safe_Release(m_pCurTexture);

    for (auto& MyPair : m_pAnimations)
        Safe_Release(MyPair.second);

    m_pAnimations.clear();
}
