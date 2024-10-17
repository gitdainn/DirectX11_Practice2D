#include "stdafx.h"
#include "UI_Handler.h"

IMPLEMENT_SINGLETON(CUI_Handler)

CUI_Handler::CUI_Handler()
{
}

CUI_Handler::~CUI_Handler()
{
}

HRESULT CUI_Handler::Initialize()
{
    Ready_PrototypeTag();

    return S_OK;
}

void CUI_Handler::Ready_PrototypeTag()
{
    PROTOTAG_TYPE   tPrototype;
    //ZeroMemory(&tPrototype, sizeof(PROTOTAG_TYPE)); // 구조체 안에 컨테이너(vector)가 있기에 ZeroMemory 금물!

    /* 리틀본 */
    //tPrototype.SkillTagVec.emplace_back(TEXT("../../ㅇㄹㄴㄹㅇ"));
    //tPrototype.SkillTagVec.emplace_back(TEXT("../../ㅇㄹㄴㄹㅇ"));
    //tPrototype.pProfileTag = TEXT("djfsl");
    //m_UIPrototypeMap.emplace(TEXT("리틀본"), tPrototype);

    /* 그림리퍼 */
    tPrototype.SkillTagVec.emplace_back(TEXT("GrimReaper_Harvest"));
    tPrototype.SkillTagVec.emplace_back(TEXT("GrimReaper_Passive"));
    tPrototype.pProfileTag = TEXT("GrimReaper");
    m_UIPrototypeMap.emplace(TEXT("그림리퍼"), tPrototype);
    tPrototype.SkillTagVec.clear();

    /* 워터스컬 */
    tPrototype.SkillTagVec.emplace_back(TEXT("AquaSkull_3_Passive"));
    tPrototype.SkillTagVec.emplace_back(TEXT("AquaSkull_3_SpiritsLake"));
    tPrototype.pProfileTag = TEXT("AquaSkull");
    m_UIPrototypeMap.emplace(TEXT("워터스컬"), tPrototype);
    tPrototype.SkillTagVec.clear();

    return;
}

void CUI_Handler::Free()
{
    __super::Free();
    
    /* @note - Pair 원본 참조(&)시 오류 뜨는 이유
    *  for(pair<const _tchar*, PROTOTAG_TYPE>& Pair : m_UIPrototypeMap) 오류!
    1. &원본 참조를 하지 않으면 복사본에 대한 clear()가 수행되므로 실제 원본에는 clear가 적용되지 않음.
    2. 아래 pair을 & 참조 시 오류가 뜸 
    -> key값인 const _tchar*은 상수라 수정X가 원칙인데 Pair에 대해 비참조 연산자 & 사용 시 전체 pair 객체의 모든 멤버에 대한 참조가 생김
    -> auto&를 이용하면 타입 추론을 통해 정확한 타입을 가져와 수정이 필요한 PROTOTAG_TYPE에만 참조를 적용시킴 
    */
    for(auto& Pair : m_UIPrototypeMap)
    {
       Pair.second.SkillTagVec.clear();
    }
    m_UIPrototypeMap.clear();

    return;
}
