#pragma once

#include "../../df/IocpSocket.h"
#include "../../df/cryptology/Crypt.h"

//����ָ��
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

	//��֤����
	static const uint16_t verifyWord_ = 65535;

	//�״�����OnConnect()sessionKey[0]�̶��ַ�
	static const uint16_t verifyPsw_ = 12345;

	//��ͷ����:4�ֽڰ�����,1�ֽ�ĩβ��0,2�ֽ�ָ�����......4�ֽ���֤����
	static const uint32_t headerSize_ = 4 + 1 + 2 + 4;
	static const uint32_t packageHeaderSize_ = 1 + 2 + 4;
	static const uint32_t uncryptHeaderSize_ = 4;

	bool hasSessionKey_;
	
	//�Ự����
	df::CryptAlg <df::CryptMode::AES_CBC> SessionCrypt_;
	//��֤����
	static df::CryptAlg <df::CryptMode::AES_CBC> VerifyCrypt_;

	//ָ���
	typedef void(*DirectProcFunc)(MainConnecter *, char *, uint);

	//ָ�����
	static DirectProcFunc FuncList[Direct::_DirectEnd];


	//��ʼ��ָ���
	static void InitFunc();

	//��ʼ����֤��Կ
	static void InitVerifyKey();


	void OnConnect() override;
	void OnRecv(char *, uint) override;
	bool Send(uint16_t directive, const char *msg, uint len);
};


