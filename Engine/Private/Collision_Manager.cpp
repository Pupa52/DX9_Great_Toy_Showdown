#include "../Public/Collision_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CCollision_Manager::CCollision_Manager()
    : m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize(_uint iLevelIndex)
{
    m_iLevelIndex = iLevelIndex;

    return S_OK;
}

void CCollision_Manager::Update()
{
    
    for (_uint i = 0; i < m_iLevelIndex; i++)
    {
        if (5 == i)
            int stop = 0;

        Collision_Cube(i, TEXT("Layer_Monster"), TEXT("Layer_Monster"));

        Collision_Cube(i, TEXT("Layer_Monster"), TEXT("Layer_Player_Bullet"));

        Collision_Cube(i, TEXT("Layer_Monster"), TEXT("Layer_Friend_Bullet"));

        Collision_Cube(i, TEXT("Layer_Monster"), TEXT("Layer_Bot_Bullet"));
        
        Collision_Cube(i, TEXT("Layer_Monster"), TEXT("Layer_SECOND_Bot_Bullet"));
        
        Collision_Cube(i, TEXT("Layer_Monster"), TEXT("Layer_THIRD_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_Monster_Bullet"));
        
        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_SECOND_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_THIRD_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_Bot_Skill"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_SECOND_Bot_Skill"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_THIRD_Bot_Skill"));

        Collision_Cube(i, TEXT("Layer_Friend_Bot"), TEXT("Layer_Monster_Bullet"));
        
        Collision_Cube(i, TEXT("Layer_Friend_Bot"), TEXT("Layer_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Friend_Bot"), TEXT("Layer_SECOND_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Friend_Bot"), TEXT("Layer_THIRD_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Friend_Bot"), TEXT("Layer_Monster"));

        Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_Player_Bullet"));

        Collision_Cube(i, TEXT("Layer_Monster_Bullet"), TEXT("Layer_Monster_Bullet"));

       // Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_Friend_Bullet"));

        Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_Bot_Bullet"));
        
        Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_SECOND_Bot_Bullet"));
        
        Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_THIRD_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_Bot"), TEXT("Layer_Monster"));

        Collision_Cube(i, TEXT("Layer_Player"), TEXT("Layer_Monster"));

        Collision_Cube(i, TEXT("Layer_Player_Skill"), TEXT("Layer_Monster"));

        Collision_Cube(i, TEXT("Layer_Item"), TEXT("Layer_Player"));

        Collision_Cube(i, TEXT("Layer_ItemBullet"), TEXT("Layer_Monster"));

        //
        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_Player_Bullet"));

        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_SECOND_Bot_Bullet"));

        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_THIRD_Bot_Bullet"));


        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_Player_Skill"));

        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_Bot_Skill"));

        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_SECOND_Bot_Skill"));

        Collision_Cube(i, TEXT("Layer_MiniGame_Bot"), TEXT("Layer_THIRD_Bot_Skill"));
    }
}

HRESULT CCollision_Manager::Collision_Cube(int iLevelIndex, const _wstring& strSourceLayerTag, const _wstring& strDestLayerTag)
{
    CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLevelIndex, strSourceLayerTag);
    if (nullptr == pSrcLayer)
        return E_FAIL;

    CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLevelIndex, strDestLayerTag);
    if (nullptr == pDstLayer)
        return E_FAIL;

    list<CGameObject*> SrcObjects = pSrcLayer->Get_Objects();
    list<CGameObject*> DstObjects = pDstLayer->Get_Objects();

    CComponent* _pSrcComponent{};
    CComponent* _pDstComponent{};

    for (auto& pSrc : SrcObjects)
    {
        _pSrcComponent = pSrc->Find_Component(TEXT("Com_Collider_Cube"));
        if (nullptr == _pSrcComponent)
            continue;

        for (auto& pDst : DstObjects)
        {
            if (pSrc == pDst)
                continue;

            _pDstComponent = pDst->Find_Component(TEXT("Com_Collider_Cube"));
            if (nullptr == _pDstComponent)
                continue;

            if (Check_Collision_Cube(pSrc, pDst, _pSrcComponent, _pDstComponent))
            {
                pSrc->OnCollision(pDst, strDestLayerTag);
                pDst->OnCollision(pSrc, strSourceLayerTag);
            }
        }
    }

    return S_OK;
}

_bool CCollision_Manager::Check_Collision_Cube(CGameObject* pSrc, CGameObject* pDst,
    CComponent* pSrcComopnent, CComponent* pDstComponent)
{
    CCollider_Cube* SrcCollider = static_cast<CCollider_Cube*>(pSrcComopnent);
    CCollider_Cube* DstCollider = static_cast<CCollider_Cube*>(pDstComponent);

    _float3 vSrcWorldPos = SrcCollider->Get_State(CCollider::STATE_POSITION);
    _float3 vDstWorldPos = DstCollider->Get_State(CCollider::STATE_POSITION);

    _float3 vSrcScale = SrcCollider->Get_Scaled();
    _float3 vDstScale = DstCollider->Get_Scaled();

    // Src Left - Button
    _float3 vMinSrc = vSrcWorldPos - _float3{
        vSrcScale.x * 0.5f,
        vSrcScale.y * 0.5f,
        vSrcScale.z * 0.5f
    };

    // Src Right - Up
    _float3 vMaxSrc = vMinSrc + vSrcScale;

    // Dst Left - Button
    _float3 vMinDst = vDstWorldPos - _float3{
        vDstScale.x * 0.5f,
        vDstScale.y * 0.5f,
        vDstScale.z * 0.5f
    };

    // Dst Right - Up
    _float3 vMaxDst = vMinDst + vDstScale;

    if (vMaxSrc.x < vMinDst.x || vMinSrc.x > vMaxDst.x) return false;
    //if (vMaxSrc.y < vMinDst.y || vMinSrc.y > vMaxDst.y) return false;
    if (vMaxSrc.z < vMinDst.z || vMinSrc.z > vMaxDst.z) return false;
    return true;
}

HRESULT CCollision_Manager::Collision_Sphere(int iLevelIndex, const _wstring& strSourceLayerTag, const _wstring& strDestLayerTag)
{
    CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLevelIndex, strSourceLayerTag);
    CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLevelIndex, strDestLayerTag);

    if (nullptr == pSrcLayer || nullptr == pDstLayer)
        return E_FAIL;

    list<CGameObject*> SrcObjects = pSrcLayer->Get_Objects();
    list<CGameObject*> DstObjects = pDstLayer->Get_Objects();

    CComponent* _pSrcComponent{};
    CComponent* _pDstComponent{};

    for (auto& pSrc : SrcObjects)
    {
        _pSrcComponent = pSrc->Find_Component(TEXT("Com_Collider_Sphere"));
        if (nullptr == _pSrcComponent)
            continue;

        for (auto& pDst : DstObjects)
        {
            _pSrcComponent = pDst->Find_Component(TEXT("Com_Collider_Sphere"));
            if (nullptr == _pSrcComponent)
                continue;

            if (pSrc == pDst)
                continue;

            if (Check_Collision_Sphere(pSrc, pDst, _pSrcComponent, _pSrcComponent))
            {
                pSrc->OnCollisionSphere(pDst, strDestLayerTag);
                pDst->OnCollisionSphere(pSrc, strSourceLayerTag);
            }
        }
    }

    return S_OK;
}

_bool CCollision_Manager::Check_Collision_Sphere(CGameObject* pSrc, CGameObject* pDst,
    CComponent* pSrcComopnent, CComponent* pDstComponent)
{
    CCollider_Sphere* SrcCollider = dynamic_cast<CCollider_Sphere*>(pSrcComopnent);
    CCollider_Sphere* DstCollider = dynamic_cast<CCollider_Sphere*>(pDstComponent);

    /* right 벡터를 반지름으로 기준 잡음 */
    _float3 vSrcRight = SrcCollider->Get_State(CCollider::STATE_RIGHT);
    _float3 vDstRight = DstCollider->Get_State(CCollider::STATE_RIGHT);

    /* 두 충돌체의 위치를 빼서 나온 방향 벡터 = 두 물체 간의 거리 */
    _float3 vSrcPos = SrcCollider->Get_State(CCollider::STATE_POSITION);
    _float3 vDstPos = DstCollider->Get_State(CCollider::STATE_POSITION);
    
    _float3 vDir = vSrcPos - vDstPos;
    _float fDistance = D3DXVec3Length(&vDir);

    _float fRadius = D3DXVec3Length(&vSrcRight) + D3DXVec3Length(&vDstRight);

    /* 두 충돌체 간의 거리 크기가 두 충돌체의 반지름 크기의 합보다 크다면 */
    /* 충돌 하지 않았다. */
    /* 만약 작거나 같다면 충돌 중이다. */
    if (fDistance > fRadius) return false;

    return true;
}

CCollision_Manager* CCollision_Manager::Create(_uint iLevelIndex)
{
    CCollision_Manager* pInstance = new CCollision_Manager();
    if (FAILED(pInstance->Initialize(iLevelIndex)))
    {
        MSG_BOX(TEXT("Failed to Created : CCollision_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollision_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
