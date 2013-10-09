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

void Sha2PasswordBuf(SS & psw, unsigned char sha2Res[32]);
void Sha2Password(SS & psw);

class MainConnecter
	: public df::IocpConnecter
{
public:
	static const uint16_t verifyWord_ = 65535;
	static const uint16_t verifyPsw_ = 12345;

	bool hasSessionKey_;
	
	//会话加密
	df::CryptAlg <df::CryptMode::AES_CBC> SessionCrypt_;
	//认证加密
	static df::CryptAlg <df::CryptMode::AES_CBC> VerifyCrypt_;

	//指令函数
	typedef void(*DirectProcFunc)(MainConnecter *, char *, uint);

	//指令函数表
	static DirectProcFunc FuncList[Direct::DirectEnd];

	MainConnecter()
		: hasSessionKey_(false)
	{}

	static void InitFunc();

	static void InitVerifyKey();



	void OnConnect() override;
	void OnRecv(char *, uint) override;
};


