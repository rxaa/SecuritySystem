#pragma once

#include "../../df/IocpSocket.h"
#include "../../df/cryptology/Crypt.h"

//所有指令
enum Direct
{
	GetHost
	//,ResponseHost
	//,Ping
	//,ResponsePing

	
	
	,_DirectEnd
};

void Sha2PasswordBuf(SS & psw, unsigned char sha2Res[32]);
void Sha2Password(SS & psw);


class MainConnecter
	: public df::IocpConnecter
{
public:
	MainConnecter()
		: hasSessionKey_(false)
	{}

	//认证口令
	static const uint16_t verifyWord_ = 65535;

	//首次连接OnConnect()sessionKey[0]固定字符
	static const uint16_t verifyPsw_ = 12345;

	//包头长度:4字节包长度,1字节末尾补0,2字节指令代号......4字节认证口令
	static const uint32_t headerSize_ = 4 + 1 + 2 + 4;
	static const uint32_t packageHeaderSize_ = 1 + 2 + 4;
	static const uint32_t uncryptHeaderSize_ = 4;

	bool hasSessionKey_;
	
	//会话加密
	df::CryptAlg <df::CryptMode::AES_CBC> SessionCrypt_;
	//认证加密
	static df::CryptAlg <df::CryptMode::AES_CBC> VerifyCrypt_;

	//指令函数
	typedef void(*DirectProcFunc)(MainConnecter *, char *, uint);

	//指令函数表
	static DirectProcFunc FuncList[Direct::_DirectEnd];


	//初始化指令函数
	static void InitFunc();

	//初始化认证密钥
	static void InitVerifyKey();


	void OnConnect() override;
	void OnRecv(char *, uint) override;
	bool Send(uint16_t directive, const char *msg, uint len);
};


