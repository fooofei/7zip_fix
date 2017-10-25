
/*
  call this at the executable/library main/.cpp's file
  with a (static) global variable



  a blog http://blog.jobbole.com/95375/
  this blog say if called _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  then _CrtDumpMemoryLeaks() will auto called, no need to manual call. On my test, it was wrong.



  this can detect malloc/calloc/realloc/new



  some way will use:
  // #ifdef _DEBUG
  // #ifndef DBG_NEW
  // #define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
  // #define new DBG_NEW
  // #endif // !DBG_NEW
  // #endif
  if use this way, we cannot use new(std::nothrow), I will not accept it

*/

#pragma once 
#ifdef WIN32

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


/* return the before flag */
inline
int
enable_memory_leak_detect()
{
  int flag_bk;
  int flag;

  flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  flag_bk = flag;
  flag |= _CRTDBG_LEAK_CHECK_DF;
  flag |= _CRTDBG_ALLOC_MEM_DF;
  _CrtSetDbgFlag(flag);

  // called or not called all the same
  //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
  // ���� vs ��������ʱ����Ϣ��������� vs ������������ dbgview

  return flag_bk;
}

inline
void
detect_memory_leak_at_end()
{
  int leak = _CrtDumpMemoryLeaks();
  if (leak != 0)
  {
#ifdef _ATL
    MessageBox(NULL, TEXT("Memory Leak"), TEXT("MemCheck"), 0);
#else 
    const char * pv = "\n\nMemory Leak\n";
    fprintf(stdout, pv);
    fprintf(stderr, pv);
#endif
  }
}

/* also we can use #pragma init_seg(compiler) at the global variable defined location
   to make sure the check is before at all static global variable init 
   NOT must
*/
typedef struct _crt_dbg_leak_t
{

  int flag_bk;

  void lock()
  {
    flag_bk = enable_memory_leak_detect();

  }
  void unlock()
  {
    detect_memory_leak_at_end(); // �Զ����ã� ���ᡣ
    if (flag_bk) {
      /*
        !!!WARNING
        if not restore this, sometimes FreeLibrary will cause an exception
        the exception stack is :
        kernel32.dll!7533136d()	δ֪	���û����롣�޷����һ�� PDB �ļ���
        [����Ŀ�ܿ��ܲ���ȷ��/��ȱʧ��û��Ϊ kernel32.dll ���ط���]		����ע��֡
        [�ⲿ����]		����ע��֡
        xxx.dll!__crtFlsFree(unsigned long dwFlsIndex) �� 377	C	�Ѽ��ط��š�
        xxx.dll!_mtterm() �� 168	C	�Ѽ��ط��š�
        xxx.dll!_CRT_INIT(void * hDllHandle, unsigned long dwReason, void * lpreserved) �� 187	C	�Ѽ��ط��š�
        xxx.dll!__DllMainCRTStartup(void * hDllHandle, unsigned long dwReason, void * lpreserved) �� 390	C	�Ѽ��ط��š�
        >	xxx.dll!_DllMainCRTStartup(void * hDllHandle, unsigned long dwReason, void * lpreserved) �� 331	C	�Ѽ��ط��š�
        ...
        > FreeLibrary()

        exception location is "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\crt\src\winapisupp.c":375
        at function _CRTIMP BOOL __crtFlsFree( DWORD dwFlsIndex);

        when set a breakpoint at this funtion, the callstack is:
        xxx.dll!__crtFlsFree(unsigned long dwFlsIndex) �� 375	C	�Ѽ��ط��š�
        xxx.dll!_mtterm() �� 168	C	�Ѽ��ط��š�
        >	xxx.dll!_CRT_INIT(void * hDllHandle, unsigned long dwReason, void * lpreserved) �� 187	C	�Ѽ��ط��š�
        xxx.dll!__DllMainCRTStartup(void * hDllHandle, unsigned long dwReason, void * lpreserved) �� 390	C	�Ѽ��ط��š�
        xxx.dll!_DllMainCRTStartup(void * hDllHandle, unsigned long dwReason, void * lpreserved) �� 331	C	�Ѽ��ط��š�
        ...
        > FreeLibrary()

        I see before the call of _heap_term() called __crtFlsFree(), there is a commment at
        _heap_term() call, "heap is now invalid!" , so I think we must restore the _CrtSetDbgFlag() flag to stop
        the check.

     */
      _CrtSetDbgFlag(flag_bk);
    }
  }


  _crt_dbg_leak_t(long break_alloc = 0) :flag_bk(0)
  {
    lock();
    if (break_alloc != 0)
    {
      _CrtSetBreakAlloc(break_alloc);
    }
  }
  ~_crt_dbg_leak_t()
  {
    unlock();
  }
}_crt_dbg_leak_t;

#else

typedef struct _crt_dbg_leak_t
{
  _crt_dbg_leak_t(long = 0)
  {
  }
  ~_crt_dbg_leak_t()
  {
  }
}_crt_dbg_leak_t;
#endif // WIN32

