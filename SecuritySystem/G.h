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
	//配置文件模型
	static MainModel main;

	//启动监听端口
	static SOCKET MainListen()
	{
		MainConnecter::InitFunc();
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

