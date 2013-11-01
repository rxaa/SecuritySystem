#pragma once
#include "MainSocket.h"
#include "MainModel.h"


class G
{
public:

	//配置文件
	static const SS & IniFile()
	{
		static SS name = df::GetProgressDir() + tcc_("SecuritySystemMain.ini");
		return name;
	}

	//连接对象智能指针
	typedef df::IntoPtr<MainConnecter> ConnectPtr;

	//已连接的服务器列表
	static List < ConnectPtr> serverList_;

	//serverList_锁
	static df::Lock listLock_;

	//配置文件模型
	static MainModel main;
	

	//启动监听端口
	inline static SOCKET MainListen()
	{
		DirectFunc::InitFunc();
		return df::IocpSocket::Listen<MainConnecter>(main.listen_port);
	}


	//读配置
	inline static bool ReadMainIni()
	{
		bool res = df::Config::Read(IniFile(), main);
		if (G::main.listen_port == 0)
			G::main.listen_port = MainModel::defaultPort_;
		return res;
	}

	//写配置
	inline static bool WriteMainIni()
	{
		return df::Config::Write(IniFile(), main);
	}
};

