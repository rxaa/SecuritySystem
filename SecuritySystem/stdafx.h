#ifndef stdafx_h__2013_8_3__18_15
#define stdafx_h__2013_8_3__18_15

// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

///XP视觉样式
#ifdef _MSC_VER
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

// TODO: 在此处引用程序需要的其他头文件
#ifdef _DEBUG
#define LAST_ERR_OUT 1
#else
#define LAST_ERR_OUT 0
#endif

#define LOG_FILE_LINE 1


#include "../../window/Control.h"
using namespace sdf;

//////////////////////////////////////////////////////////////////////////

#include "Resource.h"

#include "DirectFunc.h"
#include "G.h"

#include "FormMain.h"


#endif // stdafx_h__2013_8_3__18_15