#include "stdafx.h"
#include "MainSocket.h"
#include "../../df/cryptology/sha2.h"




df::CryptAlg <df::CryptMode::AES_CBC> MainConnecter::VerifyCrypt_;


MainConnecter::~MainConnecter()
{

}


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
	if (cmd_)
	{
		cmd_->Close();
	}

	if (fileConnect_)
	{
		fileConnect_->Close();
	}

	if (file_)
	{
		if (file_->state != FileConnect::StateNone)
		{
			try
			{
				file_->onError_(TRANS_ERRCODE_NETCLOSED, tcc_("连接断开!"));
			}CATCH_SEH;
		}

		file_->Clear();
	}

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
					FormMain::ptr_->UpdateHostCount();
				}

				return;
			}
		}
	}
}



void MainConnecter::OnRecv(char * msg, uint length)
{
	//COUT(tcc_("OnRecv") << length);
	//后续会话
	if (hasSessionKey_)
	{
		if (length % 16 > 0)//包长度不正确,非法连接
		{
			COUT(tcc_("包长不正确,非法连接:") + GetRemoteIpStr());
			df::WriteLog(tcc_("包长不正确,非法连接:") + GetRemoteIpStr());
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
			COUT(tcc_("包长小于补0,非法连接:") + GetRemoteIpStr());
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
			COUT(tcc_("认证失败,非法连接:") + GetRemoteIpStr());
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




	if (file_)//文件传输连接
	{
		if (file_->state == FileConnect::StateDownload)
			SendDownloadFile(file_->transferedSize_, file_->fileNameFrom_);
		else if (file_->state == FileConnect::StateUpload)
			SendUploadFile();
		return;
	}


	//普通连接,获取主机信息
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
		G::main.access_psw = tcc_("rxaa");
		Sha2Password(G::main.access_psw);
	}
	unsigned char verifyKey[16] = { 0 };
	G::main.access_psw.StringToByte(verifyKey, 16);
	MainConnecter::VerifyCrypt_.InitByteKey(verifyKey);
}

bool MainConnecter::Send(uint16_t directive, const void *msg, uint len, df::IocpOverlap ** overlapIO)
{
	if (msg == nullptr && len > 0)
	{
		COUT(tcc_("IocpConnecter::Send非法参数") << len);
		BREAK_POINT_MSG("IocpConnecter::Send非法参数");
		return false;
	}
	return SendMsg(directive, len, [&](uint8_t * buf){
		memcpy(buf, msg, len);
	}, overlapIO);
}

FileConnect * MainConnecter::InitTransferFile()
{
	if (fileConnect_ && !fileConnect_->IsClosed())
		return fileConnect_->file_.get();

	auto con = df::IocpSocket::Connect<MainConnecter, false>(GetRemoteIpStr(), G::main.listen_port);
	con->SessionCrypt_.InitByteKey(connectKey);
	con->file_.reset(new FileConnect);
	fileConnect_.Reset(con);

	return fileConnect_->file_.get();
}

bool MainConnecter::StartDownloadFile()
{
	MY_ASSERT(fileConnect_ != false);

	if (fileConnect_->file_->hasRecvIo_)
	{
		return fileConnect_->SendDownloadFile(fileConnect_->file_->transferedSize_, fileConnect_->file_->fileNameFrom_);
	}

	fileConnect_->file_->state = FileConnect::StateDownload;
	fileConnect_->StartRecvIo();
	fileConnect_->file_->hasRecvIo_ = true;
	return true;
}

bool MainConnecter::SendDownloadFile(int64_t pos, const CCa & fileName)
{
	if (pos < 0)
		pos = 0;
	uint len = sizeof(int64_t)+fileName.Length() + 1;

	if (!file_)
	{
		BREAK_POINT_MSG("file_ 为空");
		return false;
	}
	file_->state = FileConnect::StateDownload;

	return SendMsg(Direct::DownloadFile, len, [&](uint8_t * buf){
		*(int64_t*)buf = pos;
		buf += sizeof(int64_t);
		memcpy(buf, fileName.GetBuffer(), fileName.Size());
		buf[fileName.Size()] = 0;
	});
}


bool MainConnecter::SendTrandsferError(const CC & err)
{
	if (file_ && file_->state == FileConnect::StateNone)
		return true;
	SS s(err);
	DirectFunc::FuncList[Direct::TransferError](this, s.GetBuffer(), s.Length());
	return Send(Direct::TransferError, err);
}

bool MainConnecter::StartUploadFile()
{
	MY_ASSERT(fileConnect_ != false);

	if (fileConnect_->file_->hasRecvIo_)
	{
		return fileConnect_->SendUploadFile();
	}

	fileConnect_->file_->state = FileConnect::StateUpload;
	fileConnect_->StartRecvIo();
	fileConnect_->file_->hasRecvIo_ = true;
	return true;
}

bool MainConnecter::SendUploadFile()
{
	int64_t pos = file_->transferedSize_;
	if (pos < 0)
	{
		BREAK_POINT_MSG("pos小于0");
		pos = 0;
	}


	uint len = sizeof(int64_t)* 2 + file_->fileNameTo_.Length() + 1;

	if (!file_)
	{
		BREAK_POINT_MSG("file_ 为空");
		return false;
	}

	if (!file_->file_.Open(file_->fileNameFrom_, false, false, true))
	{
		file_->Clear();
		file_->Release();
		return false;
	}

	file_->file_.SeekStart(pos);


	int64_t size = file_->file_.GetFileSize();

	file_->state = FileConnect::StateUpload;
	return SendMsg(Direct::UploadFile, len, [&](uint8_t * buf){
		*(int64_t*)buf = size;
		buf += sizeof(int64_t);

		*(int64_t*)buf = pos;
		buf += sizeof(int64_t);

		memcpy(buf, file_->fileNameTo_.GetBuffer(), file_->fileNameTo_.Length());
		buf[file_->fileNameTo_.Size()] = 0;
	});
}


void MainConnecter::OnSend(df::IocpOverlap*& overlapIO)
{
	if (!file_)
		return;

	if (file_->state == FileConnect::StateUpload)
	{
		if (file_->file_.IsClosed())//文件已关闭.返回错误
		{
			BREAK_POINT_MSG("文件句柄被关闭");
			SendTrandsferError(TRANS_ERRCODE_HANDLE);
			return;
		}

		if (!file_->file_.Read(file_->buf_, FileConnect::FileBufferSize))//读取完成
		{
			file_->state = FileConnect::StateNone;
			Send(Direct::TransferComplete, nullptr, 0, &overlapIO);
			ON_EXIT({
				file_->Clear();
				file_->Release();
			});
			file_->onCompleted_();

			return;
		}

		file_->transferedSize_ += file_->file_.succeedByte_;

		Send(Direct::RecvFileData, file_->buf_, file_->file_.succeedByte_, &overlapIO);

		file_->onTransfer_();


		return;
	}
}

