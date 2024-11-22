#pragma once

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;
//
#define			OBJ_DEAD				-1

/** @note - 매크로함수: 매크로함수는 함수와 인자를 모두 소괄호로 묶어주어야 기호 테이블에 넘어갈 때 문제를 방지할 수 있습니다. (오버헤드 방지)*/
/** 클램프: value 값이 minValue 이하이면 minValue, maxValue 이상이면 maxValue, 범위 이내이면 value를 반환합니다. */
#define			CLAMP(value, minValue, maxValue)	( (value) < (minValue) ? (minValue) : (value) > (maxValue) ? (maxValue) : (value) )

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NO_COPY(CLASSNAME)									\
		private:											\
		CLASSNAME(const CLASSNAME&) = delete;				\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)						\
		NO_COPY(CLASSNAME)									\
		private:											\
		static CLASSNAME*	m_pInstance;					\
		public:												\
		static CLASSNAME*	GetInstance( void );			\
		static unsigned long  DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)						\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;		\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{		\
			if(nullptr == m_pInstance) {					\
				m_pInstance = new CLASSNAME;				\
			}												\
			return m_pInstance;								\
		}													\
		unsigned long CLASSNAME::DestroyInstance( void ) {	\
			unsigned long dwRefCnt = 0;						\
			if(nullptr != m_pInstance)	{					\
				dwRefCnt = m_pInstance->Release();			\
				if(0 == dwRefCnt)							\
					m_pInstance = nullptr;					\
			}												\
			return dwRefCnt;								\
		}

// char* ->tchar *
#define CToWC(IN_C,OUT_WC){															\
         int strSize = MultiByteToWideChar(CP_ACP, 0, IN_C, -1, NULL, NULL);		\
         OUT_WC = new _tchar[strSize];												\
         MultiByteToWideChar(CP_ACP,0,IN_C,(int)strlen(IN_C) + 1, OUT_WC,strSize);}

// tchar* -> char*
#define WCToC(IN_WC,OUT_C){															\
      int strSize = WideCharToMultiByte(CP_ACP, 0, IN_WC, -1, NULL, 0, NULL, NULL);	\
      OUT_C = new char[strSize];													\
      WideCharToMultiByte(CP_ACP, 0, IN_WC, -1, OUT_C, strSize, 0, 0);}
