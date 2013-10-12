#include "stdafx.h"
#include "MainSocket.h"
#include "../../df/cryptology/sha2.h"


df::CryptAlg <df::CryptMode::AES_CBC> MainConnecter::VerifyCrypt_;


void MainConnecter::OnConnect()
{
	::srand(GetTickCount());
	
	uint16_t sessionKey[8];

	sessionKey[0] = verifyPsw_;

	//对每个客户端的每个连接生成14字节随机会话密钥
	for (int i = 1; i < 8; i++)
	{
		sessionKey[i] = (uint16_t)rand();
	}

	SessionCrypt_.InitByteKey((unsigned char*)sessionKey);

	VerifyCrypt_.Encrypt((unsigned char*)sessionKey, (unsigned char*)sessionKey, 16);

	df::IocpConnecter::Send((char*)sessionKey, 16);

	hasSessionKey_ = true;
}

void MainConnecter::OnClosed()
{
	if (isClient_)
	{
		LOCKED(G::listLock_);
		//查找客户端位置
		for (int i = 0; i < G::serverList_.Size(); i++)
		{
			if (G::serverList_[i].Get() == this)
			{
				//移除
				G::serverList_.Del(i);
				if (FormMain::ptr_)
				{
					FormMain::ptr_->viewHost_.Delete(i);
					FormMain::ptr_->hostCount_.SetText(tcc_("已连接主机数:") + G::serverList_.Count());
				}
					
				return;
			}
		}		
	}
}



void MainConnecter::OnRecv(char * msg, uint length)
{
	//后续会话
	if (hasSessionKey_)
	{
		if (length < 16)//包长度过小,非法连接
		{
			df::WriteLog(tcc_("包长度过小,非法连接:") + GetRemoteIpStr());
			Close();
			return;
		}

		//解密
		SessionCrypt_.Decrypt(msg, msg, length);

		//末尾补0数
		uint8_t footZero = (uint8_t)msg[0];
		msg++;
		length--;

		if (footZero >= length) // 包长度过小, 非法连接
		{
			df::WriteLog(tcc_("包长小于补0,非法连接:") + GetRemoteIpStr());
			Close();
			return;
		}

		//认证口令
		length -= 2;
		uint16_t word1 = *(uint16_t*)(msg + length);
		length -= 2;
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
		length -= footZero;
		if (directCode < Direct::_DirectEnd)
			DirectFunc::FuncList[directCode](this, msg, length);


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

	//连接成功
	SessionCrypt_.InitByteKey((unsigned char*)msg);
	hasSessionKey_ = true;

	//获取主机信息
	Send(Direct::GetHost, 0, 0);

}


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

bool MainConnecter::Send(uint16_t directive, const char *msg, uint len)
{
	MY_ASSERT(directive < Direct::_DirectEnd);
	MY_ASSERT(hasSessionKey_);

	if ((msg == nullptr && len > 0) || len > df::IocpOverlap::MAX_PACKAGE_SIZE)
	{
		BREAK_POINT_MSG("IocpConnecter::Send非法参数");
		return false;
	}

	if (sock_ == 0)
	{
		return false;
	}

	//计算末尾补0数
	uint8_t footZero = (len + packageHeaderSize_) % SessionCrypt_.KeySize;
	if (footZero > 0)
	{
		footZero = SessionCrypt_.KeySize - footZero;
	}


	//新长度=包内容长+包头长+补0
	uint newSize = len + headerSize_ + footZero;
	auto io = df::IocpOverlap::New(newSize, this);

	uint8_t * hp = (uint8_t *)io->buffer_;

	//4字节包长度
	*(uint32_t*)hp = newSize - uncryptHeaderSize_;
	hp += 4;

	//1字节补0
	*hp = footZero;
	hp++;

	//2字节指令码
	*(uint16_t*)hp = directive;
	hp += 2;

	//包内容;
	memcpy(hp, msg, len);
	hp += len;

	//补0
	memset(hp, 0, footZero);
	hp += footZero;

	//4字节认证指令
	uint16_t word1 = (uint16_t)rand();
	uint16_t word2 = verifyWord_ ^ word1;
	((uint16_t*)hp)[0] = word1;
	((uint16_t*)hp)[1] = word2;

	MY_ASSERT((newSize - uncryptHeaderSize_) % 16 == 0);

	//从补0处开始加密
	SessionCrypt_.Encrypt(io->buffer_ + uncryptHeaderSize_, io->buffer_ + uncryptHeaderSize_, newSize - uncryptHeaderSize_);

	return SendIocpOverlap(io, newSize);
}

