

#pragma once 

#ifdef WIN32

// http://blog.jobbole.com/95375/
// ������˵ ��������� _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
// �� _CrtDumpMemoryLeaks ���Զ����ã����������ֶ����ã������ԣ�����������
//
//
//

// can detect malloc/calloc/realloc/new
//

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// ����������ʹ�ã� ���֮��new(std::nothrow) ���޷�ʹ�� 
//
// #ifdef _DEBUG
// #ifndef DBG_NEW
// #define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
// #define new DBG_NEW
// #endif // !DBG_NEW
// #endif
//

inline void enable_memory_leak_detect(long break_alloc )
{
	int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) ;
	flag |= _CRTDBG_LEAK_CHECK_DF ;
	flag |= _CRTDBG_ALLOC_MEM_DF ;
	_CrtSetDbgFlag( flag );

	// ������Ҳ��һ��Ч�� 
	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);// ���� vs ��������ʱ����Ϣ��������� vs ������������ dbgview
	if ( break_alloc != 0 )
	{
		_CrtSetBreakAlloc(break_alloc); 
	}
}

inline void detect_memory_leak_at_end()
{
	int leak = _CrtDumpMemoryLeaks();
	if( leak != 0 )
	{
#ifdef _ATL
		MessageBox(NULL,TEXT("Memory Leak"),TEXT("MemCheck"),0);
#else 
		printf("\n\nMemory Leak\n");
#endif
	}
}

typedef struct _crt_dbg_leak_t
{
	_crt_dbg_leak_t(long break_alloc = 0 )
	{
		enable_memory_leak_detect(break_alloc);
	}
	~_crt_dbg_leak_t()
	{
		detect_memory_leak_at_end(); // �Զ����ã� ���ᡣ
	}
}_crt_dbg_leak_t;

#else

typedef struct _crt_dbg_leak_t
{
	_crt_dbg_leak_t(long  = 0 )
    {
    }
    ~_crt_dbg_leak_t()
    {
    }
}_crt_dbg_leak_t;
#endif // WIN32