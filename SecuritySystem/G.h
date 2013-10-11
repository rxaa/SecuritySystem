#pragma once
#include "MainModel.h"
#include "MainSocket.h"



class G
{
private:
	//配置文件
	static df::Config mainIni_;
	//默认监听端口
	static const int defaultPort_ = 19836;

public:
	//连接对象智能指针
	typedef df::IntoPtr<MainConnecter> ConnectPtr;

	//已连接的服务器列表
	static List < ConnectPtr> serverList_;

	//serverList_锁
	static df::Lock listLock_;

	//配置文件模型
	static MainModel main;
	

	//启动监听端口
	static SOCKET MainListen()
	{
		DirectFunc::InitFunc();
		return df::IocpSocket::Listen<MainConnecter>(main.listen_port);
	}


	//读配置
	static bool ReadMainIni()
	{
		bool res = mainIni_.ReadToObject(main);
		if (G::main.listen_port == 0)
			G::main.listen_port = defaultPort_;
		return res;
	}

	//写配置
	static bool WriteMainIni()
	{
		return mainIni_.WriteFromObject(main);
	}
};

