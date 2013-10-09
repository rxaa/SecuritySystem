#include "stdafx.h"
#include "MainSocket.h"
#include "../../df/cryptology/sha2.h"

MainConnecter::DirectProcFunc MainConnecter::FuncList[Direct::DirectEnd];
df::CryptAlg <df::CryptMode::AES_CBC> MainConnecter::VerifyCrypt_;



template<unsigned DirectI>
struct DirectProc
{
	static_assert(DirectI < Direct::DirectEnd, "Index out of range");

	static void Func(MainConnecter *, char *, uint)
	{
		static_assert(DirectI >= Direct::DirectEnd, "no implementation direct process function");
	}
};


template<>
void DirectProc<Direct::GetHost>::Func(MainConnecter *, char *, uint size)
{

};



void MainConnecter::OnConnect()
{
	::srand(GetTickCount());
	uint16_t sessionKey[8];
	sessionKey[0] = verifyPsw_;
	for (int i = 1; i < 8; i++)
	{
		sessionKey[i] = (uint16_t)rand();
	}

	SessionCrypt_.InitByteKey((unsigned char*)sessionKey);

	VerifyCrypt_.Encrypt((unsigned char*)sessionKey, (unsigned char*)sessionKey, 16);

	Send((char*)sessionKey, 16);

	hasSessionKey_ = true;
}

void MainConnecter::OnRecv(char * msg, uint length)
{
	//后续通讯
	if (hasSessionKey_)
	{

		if (length < 17)//包长度过小,非法连接
		{
			df::WriteLog(tcc_("包长度过小,非法连接:") + GetRemoteIpStr());
			Close();
			return;
		}

		//末尾补0数
		uint8_t footZero = (uint8_t)msg[0];
		msg++;
		length--;
		SessionCrypt_.Decrypt(msg, msg, length);

		if (footZero >= length) // 包长度过小, 非法连接
		{
			df::WriteLog(tcc_("包长小于补0,非法连接:") + GetRemoteIpStr());
			Close();
			return;
		}

		length -= footZero;
		//认证口令
		length -= 2;
		uint16_t word1 = *(uint16_t*)(msg + length);
		length -= 4;
		uint16_t word2 = *(uint16_t*)(msg + length);
		if ((word1 ^ word2) != verifyWord_)//认证失败,非法连接
		{
			df::WriteLog(tcc_("认证失败,非法连接:") + GetRemoteIpStr());
			Close();
			return;
		}
		//指令代号
		uint16_t directCode = *(uint16_t*)msg;
		msg += 2;
		length -= 2;

		//调用指令函数
		if (directCode < Direct::DirectEnd)
			FuncList[directCode](this, msg, length);


		return;
	}


	//首次连接,获取会话密钥
	if (length != 16)//会话密钥长度不对,非法连接
	{
		Close();
		return;
	}


	SessionCrypt_.Decrypt(msg, msg, 16);
	if (*(uint16_t*)msg != verifyPsw_)
	{
		Close();
		df::AsyncStart([]{
			df::msg(tcc_("密码错误!"));
		});
		return;
	}


	SessionCrypt_.InitByteKey((unsigned char*)msg);
	hasSessionKey_ = true;
}


template<unsigned I>
struct InitProc
{
	inline static void Init()
	{
		MainConnecter::FuncList[I] = DirectProc<I>::Func;

		InitProc<I + 1>::Init();
	}
};

void Sha2Password(SS & psw)
{
	unsigned char sha2Res[32];
	Sha2PasswordBuf(psw, sha2Res);
	psw.ByteToString(sha2Res, 16);
}

void Sha2PasswordBuf(SS & psw, unsigned char sha2Res[32])
{
	sha2((UCHAR*)psw.GetBuffer(), psw.Length(), sha2Res);
	int32_t * resPtr = (int32_t *)sha2Res;
	resPtr[0] ^= resPtr[4];
	resPtr[1] ^= resPtr[5];
	resPtr[2] ^= resPtr[6];
	resPtr[3] ^= resPtr[7];
}

template<>
struct InitProc< Direct::DirectEnd>
{
	static void Init()
	{
	}
};



void MainConnecter::InitFunc()
{
	InitProc<0>::Init();
}

void MainConnecter::InitVerifyKey()
{
	if (G::main.access_psw.Length() == 0)
	{
		G::main.access_psw = tcc_("rxaa+_)(*&^%$#@!");
		Sha2Password(G::main.access_psw);
	}
	unsigned char verifyKey[16] = { 0 };
	G::main.access_psw.StringToByte(verifyKey, 16);
	MainConnecter::VerifyCrypt_.InitByteKey(verifyKey);
}

