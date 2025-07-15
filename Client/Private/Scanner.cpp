#include "stdafx.h"
#include "Scanner.h"
#include "Level_Loddy.h"

#include "Layer.h"
#include "TileMgr.h"

CScanner::CScanner(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CActor(pGraphic_Device)
{
}

CScanner::CScanner(const CScanner& Prototype)
    : CActor(Prototype)
{
}

HRESULT CScanner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CScanner::Initialize(void* pArg)
{
    _float3* pPos = static_cast<_float3*>(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(pPos->x, pPos->y, pPos->z));
    m_pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    
    m_pPlayerTransform = dynamic_cast<CTransform*>
        (m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

    m_fFrameCount = 0.f;
    m_eScanner_State = SCANNER_START;

    m_pTransformCom->Scaling(4.f, 4.f, 4.f);
    m_pAnimatorCom->Change_Texture(TEXT("Scanner_Start"));

    return S_OK;
}

_uint CScanner::Priority_Update(_float fTimeDelta)
{
    if (m_pos.y >= 10.f)
        m_bDead = true;

    if(m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CScanner::Update(_float fTimeDelta)
{
    m_fDeltaCount += fTimeDelta;

    if (S_OK == m_pAnimatorCom->Move_Frame(fTimeDelta))
    {
        m_fFrameCount++;
    }

    if (m_eScanner_State == SCANNER_FLYING_START)
    {
        m_pos.y += 1.5f * fTimeDelta;
    }

    if (m_bCanScan && (m_fDeltaCount >= 1.f))
    {
    }
    if (m_eScanner_State == SCANNER_FLYING_END)
    {
        m_pos.y += 5.5f * fTimeDelta;
    }

    if (m_eScanner_State == SCANNER_START && (m_fFrameCount > 0))
    {
        m_fDeltaCount = 0;
        m_eScanner_State = SCANNER_FLYING_START;
        m_pAnimatorCom->Change_Texture(TEXT("Scanner_Flying_Start"));
        m_fFrameCount = 0;
    }
    else if (m_eScanner_State == SCANNER_FLYING_START && (m_fFrameCount > 0))
    {
        m_eScanner_State = SCANNER_FLYING_IDLE;
        m_pAnimatorCom->Change_Texture(TEXT("Scanner_Flying_Idle"));

        m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SCANNER));
        m_pGameInstance->SoundPlay(L"sfx_Install_ChickDrone.wav", _uint(CHANNELID::SOUND_SCANNER), .5f);
        // 플레이어 멀어지면 소리 안남
        _float3 PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _float3 myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        if (PlayerPos.x > (myPos.x + 10) || PlayerPos.x <(myPos.x - 10) ||
            PlayerPos.z >(myPos.z + 10) || PlayerPos.z < (myPos.z - 10))
        {
            m_pGameInstance->StopSound(_uint(CHANNELID::SOUND_SCANNER));
        }


        m_tActorDesc.m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_tActorDesc.m_Pos.y -= 0.4f;
        for (int i = 0; i < 32; i++)
        {
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Scanner_Laser"), &m_tActorDesc);
        }


        CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TileMgr"));
        CTileMgr* pMgr = static_cast<CTileMgr*>(pLayer->Get_Objects().front());
        pMgr->Change_RangeTile(m_pos);
        m_bCanScan = false;

        m_fDeltaCount = 0;
        m_fFrameCount = 0;
        m_bCanScan = true;
    }
    else if (m_eScanner_State == SCANNER_FLYING_IDLE && (m_fFrameCount > 3))
    {
        m_eScanner_State = SCANNER_FLYING_END;
        m_pAnimatorCom->Change_Texture(TEXT("Scanner_Flying_End"));
        m_fFrameCount = 0;
    }
    else if (m_eScanner_State == SCANNER_FLYING_END && (m_fFrameCount > 3))
    {
        m_eScanner_State = SCANNER_END_START;
        m_pAnimatorCom->Change_Texture(TEXT("Scanner_End_Start"));
        m_fFrameCount = 0;
    }


    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pos);
}

void CScanner::Late_Update(_float fTimeDelta)
{

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CScanner::Render()
{
    if (FAILED(m_pTransformCom->Bind_Transform()))
        return E_FAIL;

    if (FAILED(m_pAnimatorCom->Bind_Texture()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Reset_RenderState()))
        return E_FAIL;

    return S_OK;
}

HRESULT CScanner::Ready_Components()
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

    /* For.Com_Texture */
    m_pAnimatorCom->Add_Texture(TEXT("Scanner_End_Start"),
        static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_End_Start"))));

    m_pAnimatorCom->Add_Texture(TEXT("Scanner_Flying_End"),
        static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Flying_End"))));

    m_pAnimatorCom->Add_Texture(TEXT("Scanner_Flying_Idle"),
        static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Flying_Idle"))));

    m_pAnimatorCom->Add_Texture(TEXT("Scanner_Flying_Start"),
        static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Flying_Start"))));

    m_pAnimatorCom->Add_Texture(TEXT("Scanner_Start"),
        static_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Scanner_Start"))));

    return S_OK;
}

HRESULT CScanner::SetUp_RenderState()
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

HRESULT CScanner::Reset_RenderState()
{
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CScanner* CScanner::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CScanner* pInstance = new CScanner(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CScanner"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CScanner::Clone(void* pArg)
{
    CScanner* pInstance = new CScanner(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CScanner"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CScanner::Free()
{
    __super::Free();
}
