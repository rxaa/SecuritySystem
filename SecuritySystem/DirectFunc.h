#pragma once

//所有指令
enum Direct
{
	GetHost,
	ResponseHost,
	GetCmd,
	ResponseCmd,
	//Ping,
	//ResponsePing,



	_DirectEnd,
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

