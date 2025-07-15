#include "stdafx.h"
#include "LobbyPlayer.h"
#include "Level_Loddy.h"

CLobbyPlayer::CLobbyPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CActor(pGraphic_Device)
{
}

CLobbyPlayer::CLobbyPlayer(const CLobbyPlayer& Prototype)
    : CActor(Prototype)
{
}

HRESULT CLobbyPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLobbyPlayer::Initialize(void* pArg)
{
    LOBBYPlAYER_DESC* pDesc = static_cast<LOBBYPlAYER_DESC*>(pArg);
    m_ePlayerCharacter = pDesc->ePlayerCharacter;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    pDesc->m_Pos.y -= .5f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->m_Pos);
    m_pTransformCom->Scaling(2.f, 2.f, 2.f);

    m_ePlayerState = IDLE;
    m_pAnimatorCom->Change_Texture(TEXT("Texture_Idle_Character"));
    m_pGunAnimatorCom->Change_Texture(TEXT("Texture_Idle_Gun"));

    return S_OK;
}

_uint CLobbyPlayer::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CLobbyPlayer::Update(_float fTimeDelta)
{
    if (m_ePlayerCharacter == static_cast<CLevel_Loddy*>(m_pGameInstance->Get_CurrentLevel())->Get_PlayerCharacter())
    {
        if (CHANGE != m_ePlayerState)
        {
            m_ePlayerState = CHANGE;
            m_pAnimatorCom->Change_Texture(TEXT("Texture_Change_Character"));
            m_pGunAnimatorCom->Change_Texture(TEXT("Texture_Change_Gun"));
        }
    }
    else
    {
        if(IDLE != m_ePlayerState)
        {
            m_ePlayerState = IDLE;
            m_pAnimatorCom->Change_Texture(TEXT("Texture_Idle_Character"));
            m_pGunAnimatorCom->Change_Texture(TEXT("Texture_Idle_Gun"));
        }
    }

    m_pAnimatorCom->Move_Frame(fTimeDelta);
    m_pGunAnimatorCom->Move_Frame(fTimeDelta);
}

void CLobbyPlayer::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobbyPlayer::Render()
{
    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pAnimatorCom->Bind_Texture()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;



    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pGunAnimatorCom->Bind_Texture()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLobbyPlayer::Ready_Components()
{
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 20.0f;
    TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
    /* For.Com_Transform */
    if(FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"),
        reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    /* For.Com_Animator */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"), TEXT("Com_Animator"),
        reinterpret_cast<CComponent**>(&m_pAnimatorCom), nullptr)))
        return E_FAIL;

    /* For.Com_Animator_Gun */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Animator"), TEXT("Com_Animator_Gun"),
        reinterpret_cast<CComponent**>(&m_pGunAnimatorCom), nullptr)))
        return E_FAIL;


    wstring strChangeCharacter = {};
    wstring strChangeGun = {};
    wstring strIdleCharacter = {};
    wstring strIdleGun = {};

    if (PLAYER_CHARACTER::PLAYER_CAT == m_ePlayerCharacter)
    {
        strChangeCharacter = TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Change_Character");
        strChangeGun = TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Change_Gun");
        strIdleCharacter = TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Idle_Character");
        strIdleGun = TEXT("Prototype_Component_Texture_LobbyPlayer_Cat_Idle_Gun");
    }
    else if(PLAYER_CHARACTER::PLAYER_DOG == m_ePlayerCharacter)
    {
        strChangeCharacter = TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Change_Character");
        strChangeGun = TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Change_Gun");
        strIdleCharacter = TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Idle_Character");
        strIdleGun = TEXT("Prototype_Component_Texture_LobbyPlayer_Dog_Idle_Gun");
    }
    else if (PLAYER_CHARACTER::PLAYER_PENGUIN == m_ePlayerCharacter)
    {
        strChangeCharacter = TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Change_Character");
        strChangeGun = TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Change_Gun");
        strIdleCharacter = TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Idle_Character");
        strIdleGun = TEXT("Prototype_Component_Texture_LobbyPlayer_Penguin_Idle_Gun");
    }

    // 선택시
    CTexture* Change_Character = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_LOBBY,
        strChangeCharacter, nullptr));
    CTexture* Change_Gun = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_LOBBY,
        strChangeGun, nullptr));

    m_pAnimatorCom->Add_Texture(TEXT("Texture_Change_Character"), Change_Character);
    m_pGunAnimatorCom->Add_Texture(TEXT("Texture_Change_Gun"), Change_Gun);


    // 기본
    CTexture* Idle_Character = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_LOBBY,
        strIdleCharacter, nullptr));
    CTexture* Idle_Gun = static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_LOBBY,
        strIdleGun, nullptr));

    m_pAnimatorCom->Add_Texture(TEXT("Texture_Idle_Character"), Idle_Character);
    m_pGunAnimatorCom->Add_Texture(TEXT("Texture_Idle_Gun"), Idle_Gun);

    return S_OK;
}

HRESULT CLobbyPlayer::SetUp_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    /* 픽셀이 알파 테스트를 통과하여 그려질 조건을 설정한다. */
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    /* 알파값이 0보다 큰 것들만 그리겠다. */
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);

    return S_OK;
}

HRESULT CLobbyPlayer::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    return S_OK;
}

CLobbyPlayer* CLobbyPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CLobbyPlayer* pInstance = new CLobbyPlayer(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLobbyPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLobbyPlayer::Clone(void* pArg)
{
    CLobbyPlayer* pInstance = new CLobbyPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLobbyPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLobbyPlayer::Free()
{
    __super::Free();
    Safe_Release(m_pGunAnimatorCom);
}
