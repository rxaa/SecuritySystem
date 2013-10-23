#pragma once

#include "../../df/socket/IocpSocket.h"
#include "../../df/cryptology/Crypt.h"


void Sha2PasswordBuf(SS & psw, unsigned char sha2Res[32]);
void Sha2Password(SS & psw);


class MainConnecter
	: public df::IocpConnecter
{
public:
	MainConnecter()
		: hasSessionKey_(false)
		, isClient_(false)
	{}

	//认证口令
	static const uint16_t verifyWord_ = 65535;

	//会话密钥前2个字节
	static const uint16_t verifyPsw_ = 12345;

	//包头长度:4字节包长度,1字节末尾补0,2字节指令代号......4字节认证口令
	static const uint32_t headerSize_ = 4 + 1 + 2 + 4;
	static const uint32_t packageHeaderSize_ = 1 + 2 + 4;
	static const uint32_t uncryptHeaderSize_ = 4;

	bool hasSessionKey_;
	bool isClient_;

	//会话加密
	df::CryptAlg <df::CryptMode::AES_CBC> SessionCrypt_;
	//认证加密
	static df::CryptAlg <df::CryptMode::AES_CBC> VerifyCrypt_;


	//初始化认证密钥
	static void InitVerifyKey();

	void OnConnect() override;
	void OnRecv(char *, uint) override;
	void OnClosed() override;
	bool Send(uint16_t directive, const char *msg, uint len);
	bool Send(uint16_t directive, const CC & str)
	{
		return Send(directive, (char*)str.char_, (str.length_ + 1) * sizeof(TCHAR));
	}
};


