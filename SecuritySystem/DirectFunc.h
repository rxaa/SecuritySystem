#pragma once


//错误码
#define TRANS_ERRCODE_UNKNOWN tcc_("0\n未知错误!")
#define TRANS_ERRCODE_HANDLE tcc_("1\n文件句柄被关闭!")
#define TRANS_ERRCODE_WRITE tcc_("2\n文件写入失败!")
#define TRANS_ERRCODE_NETCLOSED tcc_("3") //连接关闭
#define TRANS_ERRCODE_USER_CLOSED tcc_("4\n用户强行中断")
#define TRANS_ERRCODE_INVALID_FILE tcc_("5\n非法文件")
#define TRANS_ERRCODE_OPEN_FAILED tcc_("6\n文件打开失败")

//所有指令
struct Direct
{
	enum
	{
		GetHost,
		ResponseHost,
		GetCmd,
		ResponseCmd,
		CloseCmd,
		GetMenu,
		ResponseMenu,
		DownloadFile,
		ResponseDownload,
		Message,
		RecvFileData,
		TransferComplete,
		TransferError,
		UploadFile,
		DeleteFile,
		GetProcList,
		ResponseProc,
		KillProc,
		GetAttr,
		ResponseAttr,
		ReName,
		ReAttr,

		//Ping,
		//ResponsePing,



		_DirectEnd,
	};

};

class MainConnecter;

class DirectFunc
{
public:
	//指令函数
	typedef void(*DirectProcFunc)(MainConnecter *, char *, uint);

	//指令函数表
	static DirectProcFunc FuncList[Direct::_DirectEnd];


	//初始化指令函数
	static void InitFunc();
};

