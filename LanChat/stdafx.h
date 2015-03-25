
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include <afxsock.h>

#include <mmsystem.h>//wave支持头文件
#pragma comment(lib,"winmm.lib")


//消息最大尺寸
#define MAX_MSG_SIZE		512
//最长发送尝试时间:600秒（10分钟）
#define MAX_MSG_TIME		600
//发送接收文件（夹）用缓存区大小
#define MAX_BUF_SIZE	1024*1024
//发送文件时用到的具体操作
#define FILE_SEND			1	//请求发送文件
#define FILE_ACCEPT			2	//接收发送的文件
#define FILE_REFUSE			3	//拒绝接收
#define FILE_CANCEL			4	//取消文件的发送或接收

//操作码
enum Operate
{
	OVR,//操作结束
	CIN,//上线
	ONL,//在线
	QUT,//下线
	TXT,//聊天内容
	ACK,//确认包
	FIL,//发送文件
	FLD,//文件夹
	VOX//语音聊天
};
//#define CIN				0x0000	//上线
//#define QUT				0x0001	//下线
//#define TXT				0x0002	//发送聊天内容
//#define ACK				0x0003	//确认包
//#define FIL				0x0004	//请求发送文件（夹）
//#define VOX				0x0005	//请求语音聊天

//MyUdp报内容结构
struct MyData
{
	WORD operate;
	WORD number;
	TCHAR data[1];
};

//发送文件时用到的文件信息结构
struct MyFileInfo
{
	WORD subOper;
	ULONGLONG fileSize;
	TCHAR fileName[1];
};

//将文件大小转换成适合人类阅读的格式
CString GetSizeString(ULONGLONG &ullSize);
//发送文件线程函数
UINT SendFileFunc(LPVOID pParam);
//接收文件线程函数
UINT RecvFileFunc(LPVOID pParam);
//发送文件夹线程函数
UINT SendFolderFunc(LPVOID pParam);
//接收文件夹线程函数
UINT RecvFolderFunc(LPVOID pParam);





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


