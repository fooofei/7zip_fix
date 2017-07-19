# 7zip_fix


I found 7-Zip's memory leak in extract rar files, this is a fix version of it.

The repository is base on 7-Zip 16.04, you can download it from https://sourceforge.net/projects/sevenzip/files/7-Zip/16.04/.

Changes files include:

CPP/7zip/Archive/DllExports2.cpp
CPP/7zip/Crypto/MyAes.h 
CPP/7zip/Crypto/RarAes.h 


git diff:

diff --git a/7z1604-src/CPP/7zip/Archive/DllExports2.cpp b/7zip_fix/CPP/7zip/Archive/DllExports2.cpp
index c43e72a..2d8fb0e 100755
--- a/7z1604-src/CPP/7zip/Archive/DllExports2.cpp
+++ b/7zip_fix/CPP/7zip/Archive/DllExports2.cpp
@@ -22,6 +22,15 @@
 
 #include "IArchive.h"
 
+//
+// fooofei 2017.01.09
+#ifdef _DEBUG
+#include <stdio.h>
+#include "detectmemleak.h"
+struct _crt_dbg_leak_t g_crt_dbg_leak;
+#endif
+//
+
 HINSTANCE g_hInstance;
 
 #define NT_CHECK_FAIL_ACTION return FALSE;
diff --git a/7zip_fix/CPP/7zip/Archive/detectmemleak.h b/7zip_fix/CPP/7zip/Archive/detectmemleak.h
new file mode 100755
index 0000000..f3e0bc1
--- /dev/null
+++ b/7zip_fix/CPP/7zip/Archive/detectmemleak.h
@@ -0,0 +1,82 @@
+
+
+#pragma once 
+
+#ifdef WIN32
+
+// http://blog.jobbole.com/95375/
+// 此文中说 如果设置了 _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
+// 则 _CrtDumpMemoryLeaks 会自动调用，可以无需手动调用，经测试，不是这样的
+//
+//
+//
+
+// can detect malloc/calloc/realloc/new
+//
+
+#define _CRTDBG_MAP_ALLOC
+#include <stdlib.h>
+#include <crtdbg.h>
+
+// 不可以这样使用， 如此之后，new(std::nothrow) 将无法使用 
+//
+// #ifdef _DEBUG
+// #ifndef DBG_NEW
+// #define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
+// #define new DBG_NEW
+// #endif // !DBG_NEW
+// #endif
+//
+
+inline void enable_memory_leak_detect(long break_alloc )
+{
+	int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) ;
+	flag |= _CRTDBG_LEAK_CHECK_DF ;
+	flag |= _CRTDBG_ALLOC_MEM_DF ;
+	_CrtSetDbgFlag( flag );
+
+	// 不设置也是一样效果 
+	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);// 当从 vs 调试运行时，信息还是输出在 vs 里，其他则输出到 dbgview
+	if ( break_alloc != 0 )
+	{
+		_CrtSetBreakAlloc(break_alloc); 
+	}
+}
+
+inline void detect_memory_leak_at_end()
+{
+	int leak = _CrtDumpMemoryLeaks();
+	if( leak != 0 )
+	{
+#ifdef _ATL
+		MessageBox(NULL,TEXT("Memory Leak"),TEXT("MemCheck"),0);
+#else 
+		printf("\n\nMemory Leak\n");
+#endif
+	}
+}
+
+typedef struct _crt_dbg_leak_t
+{
+	_crt_dbg_leak_t(long break_alloc = 0 )
+	{
+		enable_memory_leak_detect(break_alloc);
+	}
+	~_crt_dbg_leak_t()
+	{
+		detect_memory_leak_at_end(); // 自动调用？ 不会。
+	}
+}_crt_dbg_leak_t;
+
+#else
+
+typedef struct _crt_dbg_leak_t
+{
+	_crt_dbg_leak_t(long  = 0 )
+    {
+    }
+    ~_crt_dbg_leak_t()
+    {
+    }
+}_crt_dbg_leak_t;
+#endif // WIN32
\ No newline at end of file
diff --git a/7z1604-src/CPP/7zip/Crypto/MyAes.h b/7zip_fix/CPP/7zip/Crypto/MyAes.h
index 2c10048..192f543 100755
--- a/7z1604-src/CPP/7zip/Crypto/MyAes.h
+++ b/7zip_fix/CPP/7zip/Crypto/MyAes.h
@@ -29,6 +29,12 @@ class CAesCbcCoder:
 
 public:
   CAesCbcCoder(bool encodeMode, unsigned keySize);
+
+  // fooofei 2017.01.09
+  virtual ~CAesCbcCoder()
+  {
+	  ;
+  }
   
   MY_UNKNOWN_IMP3(ICompressFilter, ICryptoProperties, ICompressSetCoderProperties)
   
@@ -43,11 +49,23 @@ public:
 struct CAesCbcEncoder: public CAesCbcCoder
 {
   CAesCbcEncoder(unsigned keySize = 0): CAesCbcCoder(true, keySize) {}
+
+  // fooofei 2017.01.09
+  virtual ~CAesCbcEncoder()
+  {
+	  ;
+  }
 };
 
 struct CAesCbcDecoder: public CAesCbcCoder
 {
   CAesCbcDecoder(unsigned keySize = 0): CAesCbcCoder(false, keySize) {}
+
+  // fooofei 2017.01.09
+  virtual ~CAesCbcDecoder() 
+  {
+	  ;
+  }
 };
 
 }
diff --git a/7z1604-src/CPP/7zip/Crypto/RarAes.h b/7zip_fix/CPP/7zip/Crypto/RarAes.h
index 0f5bd41..499e266 100755
--- a/7z1604-src/CPP/7zip/Crypto/RarAes.h
+++ b/7zip_fix/CPP/7zip/Crypto/RarAes.h
@@ -32,6 +32,12 @@ class CDecoder:
   Byte _iv[AES_BLOCK_SIZE];
 
   void CalcKey();
+protected:
+	// fooofei 2017.01.09
+	~CDecoder()
+  {
+
+  }
 public:
   /*
   MY_UNKNOWN_IMP1(
