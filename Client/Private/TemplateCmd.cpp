#include "MoveCmd.h"
#include "Player.h"
// 추후 몬스터도 추가

/** @note - 템플릿은 템플릿 클래스/메소드 상관없이 함수 구현부는 메소드마다
template <typename T> 선언하고, 클래스<타입명>::함수명()으로 선언해야 한다.
그리고 헤더에 선언하거나 cpp에 선언했으면 cpp를 헤더에 포함해야 함. */
template<typename T>
CTemplateCmd<T>::CTemplateCmd(const CONTROL_KEY eMove)
    : m_eMoveInfo(eMove)
{
}

template<typename T>
CTemplateCmd<T>::~CTemplateCmd()
{
}

template<typename T>
void CTemplateCmd<T>::Execute(T& pObject)
{
    pObject->Move(m_eMoveInfo);
}

/** @note - 템플릿 인스턴스화 대상 타입 제한 (p.466)
* 구현부를 헤더에 적거나 포함하지 않고,
인스턴스화될 타입을 명시해 독립적으로 빌드와 링킹되게 하면
해당 타입 외에는 헤더에서 타입을 알 수 없기에 컴파일이 불가능하다. */
template class CTemplateCmd<CPlayer*>;

