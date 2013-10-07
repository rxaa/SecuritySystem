#pragma once

#include "../../df/IocpSocket.h"
#include "../../df/cryptology/Crypt.h"

enum Direct
{
	GetHost
	//,ResponseHost
	//,Ping
	//,ResponsePing

	
	
	,DirectEnd
};


class MainConnecter
	: public df::IocpConnecter
{
public:
	//df::CryptAlg <df::CryptMode::AES_CBC> crypt_;

	typedef void(*DirectProcFunc)(MainConnecter *, char *, uint);

	static DirectProcFunc FuncList[Direct::DirectEnd];

	static void InitFunc();


	void OnConnect() override;
	void OnRecv(char *, uint) override;
};


