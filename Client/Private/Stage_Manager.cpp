#include "stdafx.h"
#include "Stage_Manager.h"

IMPLEMENT_SINGLETON(CStage_Manager)

CStage_Manager::CStage_Manager()
{
}

CStage_Manager::~CStage_Manager()
{
}

void CStage_Manager::Enter(const ROOM_TYPE& eRoomType)
{
}

void CStage_Manager::Free()
{
	__super::Free();
}
