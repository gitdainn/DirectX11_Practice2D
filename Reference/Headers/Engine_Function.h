#pragma once

namespace Engine
{
	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() = default;
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))			
				return true;		

			return false;
		}

	private:
		const wchar_t*		m_pTargetTag = { nullptr };
	};

	template <typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
			delete pPointer;
		pPointer = nullptr;
	}

	template <typename T>
	void Safe_Delete_Array(T& pArray)
	{
		if (nullptr != pArray)
			delete [] pArray;
		pArray = nullptr;
	}

	template <typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pInstance)	
		{
			dwRefCnt = pInstance->Release();
			if (0 == dwRefCnt)
				pInstance = nullptr;
		}		

		return dwRefCnt;
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pInstance)		
			dwRefCnt = pInstance->AddRef();			

		return dwRefCnt;
	}

#pragma region operator 연산자
	/** @qurious - 왜 _float2 기준 operator를 했는데 vector에 += 한 게 인식 못한다고 오류가 뜨나? */
	//const _float2 operator *=(_float2& fA, const _float2& fB)
	//{
	//	fA.x *= fB.x;
	//	fA.y *= fB.y;
	//}

	//const _float2 operator +=(_float2& fA, const _float2& fB)
	//{
	//	fA.x += fB.x;
	//	fA.y += fB.y;
	//}
#pragma endregion
}