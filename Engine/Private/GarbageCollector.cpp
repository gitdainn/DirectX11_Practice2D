#pragma once
#include "GarbageCollector.h"

IMPLEMENT_SINGLETON(CGarbageCollector)

CGarbageCollector::CGarbageCollector()
{
}

HRESULT CGarbageCollector::Initialize()
{
	return S_OK;
}

void CGarbageCollector::Add_Garbage(char* pChar)
{
	m_CharList.push_back(pChar);
}

void CGarbageCollector::Add_Garbage(_tchar* pTChar)
{
	m_TCharList.push_back(pTChar);
}

void CGarbageCollector::Add_CurLevelGarbage(char* pChar)
{
	m_CurLevelCharList.push_back(pChar);
}

void CGarbageCollector::Add_CurLevelGarbage(_tchar* pTChar)
{
	m_CurLevelTCharList.push_back(pTChar);
}

void CGarbageCollector::Clear_CurLevelGarbage()
{
	for (auto& pChar : m_CurLevelCharList)
		Safe_Delete_Array(pChar);
	m_CurLevelCharList.clear();

	for (auto& pTChar : m_CurLevelTCharList)
		Safe_Delete_Array(pTChar);
	m_CurLevelTCharList.clear();
}


void CGarbageCollector::Free()
{
	Clear_CurLevelGarbage();

	for (auto& pChar : m_CharList)
		Safe_Delete_Array(pChar);
	m_CharList.clear();

	for (auto& pTChar : m_TCharList)
		Safe_Delete_Array(pTChar);
	m_TCharList.clear();
}
